from ubinascii import b2a_base64

from .. import encoding
from . import constants, error


class SuggestedParams:
    """
    Contains various fields common to all transaction types.

    Args:
        fee (int): transaction fee (per byte if flat_fee is false). When flat_fee is true,
            fee may fall to zero but a group of N atomic transactions must
            still have a fee of at least N*min_txn_fee.
        first (int): first round for which the transaction is valid
        last (int): last round for which the transaction is valid
        gh (str): genesis hash
        gen (str, optional): genesis id
        flat_fee (bool, optional): whether the specified fee is a flat fee
        consensus_version (str, optional): the consensus protocol version as of 'first'
        min_fee (int, optional): the minimum transaction fee (flat)

    Attributes:
        fee (int)
        first (int)
        last (int)
        gen (str)
        gh (str)
        flat_fee (bool)
        consensus_version (str)
        min_fee (int)
    """

    def __init__(
        self,
        fee,
        first,
        last,
        gh,
        gen=None,
        flat_fee=False,
        consensus_version=None,
        min_fee=None,
    ):
        self.first = first
        self.last = last
        self.gh = gh
        self.gen = gen
        self.fee = fee
        self.flat_fee = flat_fee
        self.consensus_version = consensus_version
        self.min_fee = min_fee


class Transaction:
    """
    Superclass for various transaction types.
    """

    def __init__(self, sender, sp, note, lease, txn_type, rekey_to):
        self.sender = sender
        self.fee = sp.fee
        self.first_valid_round = sp.first
        self.last_valid_round = sp.last
        self.note = self.as_note(note)
        self.genesis_id = sp.gen
        self.genesis_hash = sp.gh
        self.group = None
        self.lease = self.as_lease(lease)
        self.type = txn_type
        self.rekey_to = rekey_to

    @staticmethod
    def as_hash(hash):
        """Confirm that a value is 32 bytes. If all zeros, or a falsy value, return None"""
        if not hash:
            return None
        assert isinstance(hash, (bytes, bytearray)), f"{hash} is not bytes"
        if len(hash) != constants.hash_len:
            raise error.WrongHashLengthError
        if not any(hash):
            return None
        return hash

    @staticmethod
    def as_note(note):
        if not note:
            return None
        if not isinstance(note, (bytes, bytearray, str)):
            raise error.WrongNoteType
        if isinstance(note, str):
            note = note.encode()
        if len(note) > constants.note_max_length:
            raise error.WrongNoteLength
        return note

    @classmethod
    def as_lease(cls, lease):
        try:
            return cls.as_hash(lease)
        except error.WrongHashLengthError:
            raise error.WrongLeaseLengthError

    def estimate_size(self):
        sig = b"0000000000"  # todo
        sig = b2a_base64(sig)[:-1].decode()
        stx = SignedTransaction(self, sig)
        return len(b2a_base64(bytes(encoding.msgpack_encode(stx), "utf-8"))[:-1])

    @staticmethod
    def undictify(d):
        txn = None
        sp = SuggestedParams(
            d["fee"] if "fee" in d else 0,
            d["fv"] if "fv" in d else 0,
            d["lv"],
            b2a_base64(d["gh"])[:-1].decode(),
            d["gen"] if "gen" in d else None,
            flat_fee=True,
        )
        args = {
            "sp": sp,
            "sender": encoding.encode_address(d["snd"]),
            "note": d["note"] if "note" in d else None,
            "lease": d["lx"] if "lx" in d else None,
            "rekey_to": encoding.encode_address(d["rekey"]) if "rekey" in d else None,
        }
        txn_type = d["type"]
        if not isinstance(d["type"], str):
            txn_type = txn_type.decode()
        if txn_type == constants.payment_txn:
            args.update(PaymentTxn._undictify(d))
            txn = PaymentTxn(**args)
        elif txn_type == constants.keyreg_txn:
            if "nonpart" in d and d["nonpart"]:
                args.update(KeyregNonparticipatingTxn._undictify(d))
                txn = KeyregNonparticipatingTxn(**args)
            else:
                if (
                    "votekey" not in d
                    and "selkey" not in d
                    and "votefst" not in d
                    and "votelst" not in d
                    and "votekd" not in d
                ):
                    args.update(KeyregOfflineTxn._undictify(d))
                    txn = KeyregOfflineTxn(**args)
                else:
                    args.update(KeyregOnlineTxn._undictify(d))
                    txn = KeyregOnlineTxn(**args)
        elif txn_type == constants.assetconfig_txn:
            args.update(AssetConfigTxn._undictify(d))
            txn = AssetConfigTxn(**args)
        elif txn_type == constants.assetfreeze_txn:
            args.update(AssetFreezeTxn._undictify(d))
            txn = AssetFreezeTxn(**args)
        elif txn_type == constants.assettransfer_txn:
            args.update(AssetTransferTxn._undictify(d))
            txn = AssetTransferTxn(**args)
        elif txn_type == constants.appcall_txn:
            args.update(ApplicationCallTxn._undictify(d))
            txn = ApplicationCallTxn(**args)
        elif txn_type == constants.stateproof_txn:
            # a state proof txn does not have these fields
            args.pop("note"), args.pop("rekey_to"), args.pop("lease")
            args.update(StateProofTxn._undictify(d))
            txn = StateProofTxn(**args)

        if txn is not None and "grp" in d:
            txn.group = d["grp"]
        return txn

    def __eq__(self, other):
        if not isinstance(other, Transaction):
            return False
        return (
            self.sender == other.sender
            and self.fee == other.fee
            and self.first_valid_round == other.first_valid_round
            and self.last_valid_round == other.last_valid_round
            and self.genesis_hash == other.genesis_hash
            and self.genesis_id == other.genesis_id
            and self.note == other.note
            and self.group == other.group
            and self.lease == other.lease
            and self.type == other.type
            and self.rekey_to == other.rekey_to
        )

    @staticmethod
    def required(arg):
        if not arg:
            raise ValueError(f"{arg} supplied as a required argument")
        return arg

    @staticmethod
    def creatable_index(index, required=False):
        """Coerce an index for apps or assets to an integer.

        By using this in all constructors, we allow callers to use
        strings as indexes, check our convenience Txn types to ensure
        index is set, and ensure that 0 is always used internally for
        an unset id, not None, so __eq__ works properly.
        """
        i = int(index or 0)
        if i == 0 and required:
            raise IndexError("Required an index")
        if i < 0:
            raise IndexError(i)
        return i

    def __str__(self):
        return str(self.__dict__)


class PaymentTxn(Transaction):
    """
    Represents a payment transaction.

    Args:
        sender (str): address of the sender
        sp (SuggestedParams): suggested params from algod
        receiver (str): address of the receiver
        amt (int): amount in microAlgos to be sent
        close_remainder_to (str, optional): if nonempty, account will be closed
            and remaining algos will be sent to this address
        note (bytes, optional): arbitrary optional bytes
        lease (byte[32], optional): specifies a lease, and no other transaction
            with the same sender and lease can be confirmed in this
            transaction's valid rounds
        rekey_to (str, optional): additionally rekey the sender to this address

    Attributes:
        sender (str)
        fee (int)
        first_valid_round (int)
        last_valid_round (int)
        note (bytes)
        genesis_id (str)
        genesis_hash (str)
        group (bytes)
        receiver (str)
        amt (int)
        close_remainder_to (str)
        type (str)
        lease (byte[32])
        rekey_to (str)
    """

    def __init__(
        self,
        sender,
        sp,
        receiver,
        amt,
        close_remainder_to=None,
        note=None,
        lease=None,
        rekey_to=None,
    ):
        Transaction.__init__(
            self, sender, sp, note, lease, constants.payment_txn, rekey_to
        )
        if receiver:
            self.receiver = receiver
        else:
            raise error.ZeroAddressError

        self.amt = amt
        if (not isinstance(self.amt, int)) or self.amt < 0:
            raise error.WrongAmountType
        self.close_remainder_to = close_remainder_to
        if not sp.flat_fee:
            self.fee = max(self.estimate_size() * self.fee, constants.min_txn_fee)

    @staticmethod
    def _undictify(d):
        args = {
            "close_remainder_to": encoding.encode_address(d["close"])
            if "close" in d
            else None,
            "amt": d["amt"] if "amt" in d else 0,
            "receiver": encoding.encode_address(d["rcv"])
            if "rcv" in d
            else constants.ZERO_ADDRESS,
        }
        return args

    def __eq__(self, other):
        if not isinstance(other, PaymentTxn):
            return False
        return (
            super(PaymentTxn, self).__eq__(other)
            and self.receiver == other.receiver
            and self.amt == other.amt
            and self.close_remainder_to == other.close_remainder_to
        )


class KeyregTxn(Transaction):
    """
    Represents a key registration transaction.

    Args:
        sender (str): address of sender
        sp (SuggestedParams): suggested params from algod
        votekey (str): participation public key in base64
        selkey (str): VRF public key in base64
        votefst (int): first round to vote
        votelst (int): last round to vote
        votekd (int): vote key dilution
        note (bytes, optional): arbitrary optional bytes
        lease (byte[32], optional): specifies a lease, and no other transaction
            with the same sender and lease can be confirmed in this
            transaction's valid rounds
        rekey_to (str, optional): additionally rekey the sender to this address
        nonpart (bool, optional): mark the account non-participating if true
        StateProofPK: state proof

    Attributes:
        sender (str)
        fee (int)
        first_valid_round (int)
        last_valid_round (int)
        note (bytes)
        genesis_id (str)
        genesis_hash (str)
        group(bytes)
        votepk (str)
        selkey (str)
        votefst (int)
        votelst (int)
        votekd (int)
        type (str)
        lease (byte[32])
        rekey_to (str)
        nonpart (bool)
        sprfkey (str)
    """

    def __init__(
        self,
        sender,
        sp,
        votekey,
        selkey,
        votefst,
        votelst,
        votekd,
        note=None,
        lease=None,
        rekey_to=None,
        nonpart=None,
        sprfkey=None,
    ):
        Transaction.__init__(
            self, sender, sp, note, lease, constants.keyreg_txn, rekey_to
        )
        self.votepk = votekey
        self.selkey = selkey
        self.votefst = votefst
        self.votelst = votelst
        self.votekd = votekd
        self.nonpart = nonpart
        self.sprfkey = sprfkey

        if not sp.flat_fee:
            self.fee = max(self.estimate_size() * self.fee, constants.min_txn_fee)

    def __eq__(self, other):
        if not isinstance(other, KeyregTxn):
            return False
        return (
            super(KeyregTxn, self).__eq__(other)
            and self.votepk == other.votepk
            and self.selkey == other.selkey
            and self.votefst == other.votefst
            and self.votelst == other.votelst
            and self.votekd == other.votekd
            and self.nonpart == other.nonpart
            and self.sprfkey == other.sprfkey
        )


class KeyregOnlineTxn(KeyregTxn):
    """
    Represents an online key registration transaction.
    nonpart is implicitly False for this transaction.

    Args:
        sender (str): address of sender
        sp (SuggestedParams): suggested params from algod
        votekey (str): participation public key in base64
        selkey (str): VRF public key in base64
        votefst (int): first round to vote
        votelst (int): last round to vote
        votekd (int): vote key dilution
        note (bytes, optional): arbitrary optional bytes
        lease (byte[32], optional): specifies a lease, and no other transaction
            with the same sender and lease can be confirmed in this
            transaction's valid rounds
        rekey_to (str, optional): additionally rekey the sender to this address
        sprfkey (str, optional): state proof ID

    Attributes:
        sender (str)
        fee (int)
        first_valid_round (int)
        last_valid_round (int)
        note (bytes)
        genesis_id (str)
        genesis_hash (str)
        group(bytes)
        votepk (str)
        selkey (str)
        votefst (int)
        votelst (int)
        votekd (int)
        type (str)
        lease (byte[32])
        rekey_to (str)
        sprfkey (str)
    """

    def __init__(
        self,
        sender,
        sp,
        votekey,
        selkey,
        votefst,
        votelst,
        votekd,
        note=None,
        lease=None,
        rekey_to=None,
        sprfkey=None,
    ):
        KeyregTxn.__init__(
            self,
            sender,
            sp,
            votekey,
            selkey,
            votefst,
            votelst,
            votekd,
            note,
            lease,
            rekey_to,
            nonpart=False,
            sprfkey=sprfkey,
        )
        self.votepk = votekey
        self.selkey = selkey
        self.votefst = votefst
        self.votelst = votelst
        self.votekd = votekd
        self.sprfkey = sprfkey
        if votekey is None:
            raise error.KeyregOnlineTxnInitError("votekey")
        if selkey is None:
            raise error.KeyregOnlineTxnInitError("selkey")
        if votefst is None:
            raise error.KeyregOnlineTxnInitError("votefst")
        if votelst is None:
            raise error.KeyregOnlineTxnInitError("votelst")
        if votekd is None:
            raise error.KeyregOnlineTxnInitError("votekd")
        if not sp.flat_fee:
            self.fee = max(self.estimate_size() * self.fee, constants.min_txn_fee)

    @staticmethod
    def _undictify(d):
        votekey = b2a_base64(d["votekey"])[:-1].decode()
        selkey = b2a_base64(d["selkey"])[:-1].decode()
        votefst = d["votefst"]
        votelst = d["votelst"]
        votekd = d["votekd"]
        if "sprfkey" in d:
            sprfID = b2a_base64(d["sprfkey"])[:-1].decode()

            args = {
                "votekey": votekey,
                "selkey": selkey,
                "votefst": votefst,
                "votelst": votelst,
                "votekd": votekd,
                "sprfkey": sprfID,
            }
        else:
            args = {
                "votekey": votekey,
                "selkey": selkey,
                "votefst": votefst,
                "votelst": votelst,
                "votekd": votekd,
            }

        return args

    def __eq__(self, other):
        if not isinstance(other, KeyregOnlineTxn):
            return False
        return super(KeyregOnlineTxn, self).__eq__(other)


class KeyregOfflineTxn(KeyregTxn):
    """
    Represents an offline key registration transaction.
    nonpart is implicitly False for this transaction.

    Args:
        sender (str): address of sender
        sp (SuggestedParams): suggested params from algod
        note (bytes, optional): arbitrary optional bytes
        lease (byte[32], optional): specifies a lease, and no other transaction
            with the same sender and lease can be confirmed in this
            transaction's valid rounds
        rekey_to (str, optional): additionally rekey the sender to this address

    Attributes:
        sender (str)
        fee (int)
        first_valid_round (int)
        last_valid_round (int)
        note (bytes)
        genesis_id (str)
        genesis_hash (str)
        group(bytes)
        type (str)
        lease (byte[32])
        rekey_to (str)
    """

    def __init__(self, sender, sp, note=None, lease=None, rekey_to=None):
        KeyregTxn.__init__(
            self,
            sender,
            sp,
            None,
            None,
            None,
            None,
            None,
            note=note,
            lease=lease,
            rekey_to=rekey_to,
            nonpart=False,
            sprfkey=None,
        )
        if not sp.flat_fee:
            self.fee = max(self.estimate_size() * self.fee, constants.min_txn_fee)

    @staticmethod
    def _undictify(d):
        args = {}
        return args

    def __eq__(self, other):
        if not isinstance(other, KeyregOfflineTxn):
            return False
        return super(KeyregOfflineTxn, self).__eq__(other)


class KeyregNonparticipatingTxn(KeyregTxn):
    """
    Represents a nonparticipating key registration transaction.
    nonpart is implicitly True for this transaction.

    Args:
        sender (str): address of sender
        sp (SuggestedParams): suggested params from algod
        note (bytes, optional): arbitrary optional bytes
        lease (byte[32], optional): specifies a lease, and no other transaction
            with the same sender and lease can be confirmed in this
            transaction's valid rounds
        rekey_to (str, optional): additionally rekey the sender to this address

    Attributes:
        sender (str)
        fee (int)
        first_valid_round (int)
        last_valid_round (int)
        note (bytes)
        genesis_id (str)
        genesis_hash (str)
        group(bytes)
        type (str)
        lease (byte[32])
        rekey_to (str)
    """

    def __init__(self, sender, sp, note=None, lease=None, rekey_to=None):
        KeyregTxn.__init__(
            self,
            sender,
            sp,
            None,
            None,
            None,
            None,
            None,
            note=note,
            lease=lease,
            rekey_to=rekey_to,
            nonpart=True,
            sprfkey=None,
        )
        if not sp.flat_fee:
            self.fee = max(self.estimate_size() * self.fee, constants.min_txn_fee)

    @staticmethod
    def _undictify(d):
        args = {}
        return args

    def __eq__(self, other):
        if not isinstance(other, KeyregNonparticipatingTxn):
            return False
        return super(KeyregNonparticipatingTxn, self).__eq__(other)


class SignedTransaction:
    """
    Represents a signed transaction.

    Args:
        transaction (Transaction): transaction that was signed
        signature (str): signature of a single address
        authorizing_address (str, optional): the address authorizing the signed transaction, if different from sender

    Attributes:
        transaction (Transaction)
        signature (str)
        authorizing_address (str)
    """

    def __init__(self, transaction, signature, authorizing_address=None):
        self.signature = signature
        self.transaction = transaction
        self.authorizing_address = authorizing_address

    def get_txid(self):
        """
        Get the transaction's ID.

        Returns:
            str: transaction ID
        """
        return self.transaction.get_txid()

    @staticmethod
    def undictify(d):
        sig = None
        if "sig" in d:
            sig = b2a_base64(d["sig"])[:-1].decode()
        auth = None
        if "sgnr" in d:
            auth = encoding.encode_address(d["sgnr"])
        txn = Transaction.undictify(d["txn"])
        stx = SignedTransaction(txn, sig, auth)
        return stx

    def __eq__(self, other):
        if not isinstance(other, SignedTransaction):
            return False
        return (
            self.transaction == other.transaction
            and self.signature == other.signature
            and self.authorizing_address == other.authorizing_address
        )


class AssetConfigTxn(Transaction):
    """
    Represents a transaction for asset creation, reconfiguration, or
    destruction.

    To create an asset, include the following:
        total, default_frozen, unit_name, asset_name,
        manager, reserve, freeze, clawback, url, metadata,
        decimals

    To destroy an asset, include the following:
        index, strict_empty_address_check (set to False)

    To update asset configuration, include the following:
        index, manager, reserve, freeze, clawback,
        strict_empty_address_check (optional)

    Args:
        sender (str): address of the sender
        sp (SuggestedParams): suggested params from algod
        index (int, optional): index of the asset
        total (int, optional): total number of base units of this asset created
        default_frozen (bool, optional): whether slots for this asset in user
            accounts are frozen by default
        unit_name (str, optional): hint for the name of a unit of this asset
        asset_name (str, optional): hint for the name of the asset
        manager (str, optional): address allowed to change nonzero addresses
            for this asset
        reserve (str, optional): account whose holdings of this asset should
            be reported as "not minted"
        freeze (str, optional): account allowed to change frozen state of
            holdings of this asset
        clawback (str, optional): account allowed take units of this asset
            from any account
        url (str, optional): a URL where more information about the asset
            can be retrieved
        metadata_hash (byte[32], optional): a commitment to some unspecified
            asset metadata (32 byte hash)
        note (bytes, optional): arbitrary optional bytes
        lease (byte[32], optional): specifies a lease, and no other transaction
            with the same sender and lease can be confirmed in this
            transaction's valid rounds
        strict_empty_address_check (bool, optional): set this to False if you
            want to specify empty addresses. Otherwise, if this is left as
            True (the default), having empty addresses will raise an error,
            which will prevent accidentally removing admin access to assets or
            deleting the asset.
        decimals (int, optional): number of digits to use for display after
            decimal. If set to 0, the asset is not divisible. If set to 1, the
            base unit of the asset is in tenths. Must be between 0 and 19,
            inclusive. Defaults to 0.
        rekey_to (str, optional): additionally rekey the sender to this address

    Attributes:
        sender (str)
        fee (int)
        first_valid_round (int)
        last_valid_round (int)
        genesis_hash (str)
        index (int)
        total (int)
        default_frozen (bool)
        unit_name (str)
        asset_name (str)
        manager (str)
        reserve (str)
        freeze (str)
        clawback (str)
        url (str)
        metadata_hash (byte[32])
        note (bytes)
        genesis_id (str)
        type (str)
        lease (byte[32])
        decimals (int)
        rekey (str)
    """

    def __init__(
        self,
        sender,
        sp,
        index=None,
        total=None,
        default_frozen=None,
        unit_name=None,
        asset_name=None,
        manager=None,
        reserve=None,
        freeze=None,
        clawback=None,
        url=None,
        metadata_hash=None,
        note=None,
        lease=None,
        strict_empty_address_check=True,
        decimals=0,
        rekey_to=None,
    ):
        Transaction.__init__(
            self, sender, sp, note, lease, constants.assetconfig_txn, rekey_to
        )
        if strict_empty_address_check:
            if not (manager and reserve and freeze and clawback):
                raise error.EmptyAddressError
        self.index = self.creatable_index(index)
        self.total = int(total) if total else None
        self.default_frozen = bool(default_frozen)
        self.unit_name = unit_name
        self.asset_name = asset_name
        self.manager = manager
        self.reserve = reserve
        self.freeze = freeze
        self.clawback = clawback
        self.url = url
        self.metadata_hash = self.as_metadata(metadata_hash)
        self.decimals = int(decimals)
        if self.decimals < 0 or self.decimals > constants.max_asset_decimals:
            raise error.OutOfRangeDecimalsError
        if not sp.flat_fee:
            self.fee = max(self.estimate_size() * self.fee, constants.min_txn_fee)

    @staticmethod
    def _undictify(d):
        index = None
        total = None
        default_frozen = None
        unit_name = None
        asset_name = None
        manager = None
        reserve = None
        freeze = None
        clawback = None
        url = None
        metadata_hash = None
        decimals = 0

        if "caid" in d:
            index = d["caid"]
        if "apar" in d:
            if "t" in d["apar"]:
                total = d["apar"]["t"]
            if "df" in d["apar"]:
                default_frozen = d["apar"]["df"]
            if "un" in d["apar"]:
                unit_name = d["apar"]["un"]
            if "an" in d["apar"]:
                asset_name = d["apar"]["an"]
            if "m" in d["apar"]:
                manager = encoding.encode_address(d["apar"]["m"])
            if "r" in d["apar"]:
                reserve = encoding.encode_address(d["apar"]["r"])
            if "f" in d["apar"]:
                freeze = encoding.encode_address(d["apar"]["f"])
            if "c" in d["apar"]:
                clawback = encoding.encode_address(d["apar"]["c"])
            if "au" in d["apar"]:
                url = d["apar"]["au"]
            if "am" in d["apar"]:
                metadata_hash = d["apar"]["am"]
            if "dc" in d["apar"]:
                decimals = d["apar"]["dc"]

        args = {
            "index": index,
            "total": total,
            "default_frozen": default_frozen,
            "unit_name": unit_name,
            "asset_name": asset_name,
            "manager": manager,
            "reserve": reserve,
            "freeze": freeze,
            "clawback": clawback,
            "url": url,
            "metadata_hash": metadata_hash,
            "strict_empty_address_check": False,
            "decimals": decimals,
        }

        return args

    def __eq__(self, other):
        if not isinstance(other, AssetConfigTxn):
            return False
        return (
            super(AssetConfigTxn, self).__eq__(other)
            and self.index == other.index
            and self.total == other.total
            and self.default_frozen == other.default_frozen
            and self.unit_name == other.unit_name
            and self.asset_name == other.asset_name
            and self.manager == other.manager
            and self.reserve == other.reserve
            and self.freeze == other.freeze
            and self.clawback == other.clawback
            and self.url == other.url
            and self.metadata_hash == other.metadata_hash
            and self.decimals == other.decimals
        )

    @classmethod
    def as_metadata(cls, md):
        try:
            return cls.as_hash(md)
        except error.WrongHashLengthError:
            raise error.WrongMetadataLengthError


class AssetFreezeTxn(Transaction):

    """
    Represents a transaction for freezing or unfreezing an account's asset
    holdings. Must be issued by the asset's freeze manager.

    Args:
        sender (str): address of the sender, who must be the asset's freeze
            manager
        sp (SuggestedParams): suggested params from algod
        index (int): index of the asset
        target (str): address having its assets frozen or unfrozen
        new_freeze_state (bool): true if the assets should be frozen, false if
            they should be transferrable
        note (bytes, optional): arbitrary optional bytes
        lease (byte[32], optional): specifies a lease, and no other transaction
            with the same sender and lease can be confirmed in this
            transaction's valid rounds
        rekey_to (str, optional): additionally rekey the sender to this address

    Attributes:
        sender (str)
        fee (int)
        first_valid_round (int)
        last_valid_round (int)
        genesis_hash (str)
        index (int)
        target (str)
        new_freeze_state (bool)
        note (bytes)
        genesis_id (str)
        type (str)
        lease (byte[32])
        rekey_to (str)
    """

    def __init__(
        self,
        sender,
        sp,
        index,
        target,
        new_freeze_state,
        note=None,
        lease=None,
        rekey_to=None,
    ):
        Transaction.__init__(
            self, sender, sp, note, lease, constants.assetfreeze_txn, rekey_to
        )
        self.index = self.creatable_index(index, required=True)
        self.target = target
        self.new_freeze_state = new_freeze_state
        if not sp.flat_fee:
            self.fee = max(self.estimate_size() * self.fee, constants.min_txn_fee)

    @staticmethod
    def _undictify(d):
        args = {
            "index": d["faid"],
            "new_freeze_state": d["afrz"] if "afrz" in d else False,
            "target": encoding.encode_address(d["fadd"]),
        }

        return args

    def __eq__(self, other):
        if not isinstance(other, AssetFreezeTxn):
            return False
        return (
            super(AssetFreezeTxn, self).__eq__(other)
            and self.index == other.index
            and self.target == other.target
            and self.new_freeze_state == other.new_freeze_state
        )


class AssetTransferTxn(Transaction):
    """
    Represents a transaction for asset transfer.

    To begin accepting an asset, supply the same address as both sender and
    receiver, and set amount to 0 (or use AssetOptInTxn)

    To revoke an asset, set revocation_target, and issue the transaction from
    the asset's revocation manager account.

    Args:
        sender (str): address of the sender
        sp (SuggestedParams): suggested params from algod
        receiver (str): address of the receiver
        amt (int): amount of asset base units to send
        index (int): index of the asset
        close_assets_to (string, optional): send all of sender's remaining
            assets, after paying `amt` to receiver, to this address
        revocation_target (string, optional): send assets from this address,
            rather than the sender's address (can only be used by an asset's
            revocation manager, also known as clawback)
        note (bytes, optional): arbitrary optional bytes
        lease (byte[32], optional): specifies a lease, and no other transaction
            with the same sender and lease can be confirmed in this
            transaction's valid rounds
        rekey_to (str, optional): additionally rekey the sender to this address

    Attributes:
        sender (str)
        fee (int)
        first_valid_round (int)
        last_valid_round (int)
        genesis_hash (str)
        index (int)
        amount (int)
        receiver (string)
        close_assets_to (string)
        revocation_target (string)
        note (bytes)
        genesis_id (str)
        type (str)
        lease (byte[32])
        rekey_to (str)
    """

    def __init__(
        self,
        sender,
        sp,
        receiver,
        amt,
        index,
        close_assets_to=None,
        revocation_target=None,
        note=None,
        lease=None,
        rekey_to=None,
    ):
        Transaction.__init__(
            self,
            sender,
            sp,
            note,
            lease,
            constants.assettransfer_txn,
            rekey_to,
        )
        if receiver:
            self.receiver = receiver
        else:
            raise error.ZeroAddressError

        self.amount = amt
        if (not isinstance(self.amount, int)) or self.amount < 0:
            raise error.WrongAmountType
        self.index = self.creatable_index(index, required=True)
        self.close_assets_to = close_assets_to
        self.revocation_target = revocation_target
        if not sp.flat_fee:
            self.fee = max(self.estimate_size() * self.fee, constants.min_txn_fee)

    @staticmethod
    def _undictify(d):
        args = {
            "receiver": encoding.encode_address(d["arcv"])
            if "arcv" in d
            else constants.ZERO_ADDRESS,
            "amt": d["aamt"] if "aamt" in d else 0,
            "index": d["xaid"] if "xaid" in d else None,
            "close_assets_to": encoding.encode_address(d["aclose"])
            if "aclose" in d
            else None,
            "revocation_target": encoding.encode_address(d["asnd"])
            if "asnd" in d
            else None,
        }

        return args

    def __eq__(self, other):
        if not isinstance(other, AssetTransferTxn):
            return False
        return (
            super(AssetTransferTxn, self).__eq__(other)
            and self.index == other.index
            and self.amount == other.amount
            and self.receiver == other.receiver
            and self.close_assets_to == other.close_assets_to
            and self.revocation_target == other.revocation_target
        )


class StateSchema:
    """
    Restricts state for an application call.

    Args:
        num_uints(int, optional): number of uints to store
        num_byte_slices(int, optional): number of byte slices to store

    Attributes:
        num_uints (int)
        num_byte_slices (int)
    """

    def __init__(self, num_uints=None, num_byte_slices=None):
        self.num_uints = num_uints
        self.num_byte_slices = num_byte_slices

    def dictify(self):
        d = dict()
        if self.num_uints:
            d["nui"] = self.num_uints
        if self.num_byte_slices:
            d["nbs"] = self.num_byte_slices
        # od = OrderedDict(sorted(d.items()))
        # return od
        return d

    @staticmethod
    def undictify(d):
        args = {
            "num_uints": d["nui"] if "nui" in d else None,
            "num_byte_slices": d["nbs"] if "nbs" in d else None,
        }
        return args

    def __eq__(self, other):
        if not isinstance(other, StateSchema):
            return False
        return (
            self.num_uints == other.num_uints
            and self.num_byte_slices == other.num_byte_slices
        )


class ApplicationCallTxn(Transaction):
    """
    Represents a transaction that interacts with the application system.

    Args:
        sender (str): address of the sender
        sp (SuggestedParams): suggested params from algod
        index (int): index of the application to call; 0 if creating a new application
        on_complete (OnComplete): intEnum representing what app should do on completion
        local_schema (StateSchema, optional): restricts what can be stored by created application;
            must be omitted if not creating an application
        global_schema (StateSchema, optional): restricts what can be stored by created application;
            must be omitted if not creating an application
        approval_program (bytes, optional): the program to run on transaction approval;
            must be omitted if not creating or updating an application
        clear_program (bytes, optional): the program to run when state is being cleared;
            must be omitted if not creating or updating an application
        app_args (list[bytes], optional): list of arguments to the application, each argument itself a buf
        accounts (list[string], optional): list of additional accounts involved in call
        foreign_apps (list[int], optional): list of other applications (identified by index) involved in call
        foreign_assets (list[int], optional): list of assets involved in call
        extra_pages (int, optional): additional program space for supporting larger programs.  A page is 1024 bytes.

    Attributes:
        sender (str)
        fee (int)
        first_valid_round (int)
        last_valid_round (int)
        genesis_hash (str)
        index (int)
        on_complete (int)
        local_schema (StateSchema)
        global_schema (StateSchema)
        approval_program (bytes)
        clear_program (bytes)
        app_args (list[bytes])
        accounts (list[str])
        foreign_apps (list[int])
        foreign_assets (list[int])
        extra_pages (int)
    """

    def __init__(
        self,
        sender,
        sp,
        index,
        on_complete,
        local_schema=None,
        global_schema=None,
        approval_program=None,
        clear_program=None,
        app_args=None,
        accounts=None,
        foreign_apps=None,
        foreign_assets=None,
        note=None,
        lease=None,
        rekey_to=None,
        extra_pages=0,
    ):
        Transaction.__init__(
            self, sender, sp, note, lease, constants.appcall_txn, rekey_to
        )
        self.index = self.creatable_index(index)
        self.on_complete = on_complete
        self.local_schema = self.state_schema(local_schema)
        self.global_schema = self.state_schema(global_schema)
        self.approval_program = self.teal_bytes(approval_program)
        self.clear_program = self.teal_bytes(clear_program)
        self.app_args = self.bytes_list(app_args)
        self.accounts = accounts
        self.foreign_apps = self.int_list(foreign_apps)
        self.foreign_assets = self.int_list(foreign_assets)
        self.extra_pages = extra_pages
        if not sp.flat_fee:
            self.fee = max(self.estimate_size() * self.fee, constants.min_txn_fee)

    @staticmethod
    def state_schema(schema):
        """Confirm the argument is a StateSchema, or false which is coerced to None"""
        if not schema or not schema.dictify():
            return None  # Coerce false/empty values to None, to help __eq__
        assert isinstance(schema, StateSchema), f"{schema} is not a StateSchema"
        return schema

    @staticmethod
    def teal_bytes(teal):
        """Confirm the argument is bytes-like, or false which is coerced to None"""
        if not teal:
            return None  # Coerce false values like "" to None, to help __eq__
        assert isinstance(teal, (bytes, bytearray)), f"Program {teal} is not bytes"
        return teal

    @staticmethod
    def bytes_list(lst):
        """Confirm or coerce list elements to bytes. Return None for empty/false lst."""

        def as_bytes(e):
            if isinstance(e, (bytes, bytearray)):
                return e
            if isinstance(e, str):
                return e.encode()
            if isinstance(e, int):
                # Uses 8 bytes, big endian to match TEAL's btoi
                return e.to_bytes(8, "big")  # raises for negative or too big
            assert False, f"{e} is not bytes, str, or int"

        if not lst:
            return None
        return [as_bytes(elt) for elt in lst]

    @staticmethod
    def int_list(lst):
        """Confirm or coerce list elements to int. Return None for empty/false lst."""
        if not lst:
            return None
        return [int(elt) for elt in lst]

    @staticmethod
    def _undictify(d):
        args = {
            "index": d["apid"] if "apid" in d else None,
            "on_complete": d["apan"] if "apan" in d else None,
            "local_schema": StateSchema(**StateSchema.undictify(d["apls"]))
            if "apls" in d
            else None,
            "global_schema": StateSchema(**StateSchema.undictify(d["apgs"]))
            if "apgs" in d
            else None,
            "approval_program": d["apap"] if "apap" in d else None,
            "clear_program": d["apsu"] if "apsu" in d else None,
            "app_args": d["apaa"] if "apaa" in d else None,
            "accounts": d["apat"] if "apat" in d else None,
            "foreign_apps": d["apfa"] if "apfa" in d else None,
            "foreign_assets": d["apas"] if "apas" in d else None,
            "extra_pages": d["apep"] if "apep" in d else 0,
        }
        if args["accounts"]:
            args["accounts"] = [
                encoding.encode_address(account_bytes)
                for account_bytes in args["accounts"]
            ]
        return args

    def __eq__(self, other):
        if not isinstance(other, ApplicationCallTxn):
            return False
        return (
            super(ApplicationCallTxn, self).__eq__(other)
            and self.index == other.index
            and self.on_complete == other.on_complete
            and self.local_schema == other.local_schema
            and self.global_schema == other.global_schema
            and self.approval_program == other.approval_program
            and self.clear_program == other.clear_program
            and self.app_args == other.app_args
            and self.accounts == other.accounts
            and self.foreign_apps == other.foreign_apps
            and self.foreign_assets == other.foreign_assets
            and self.extra_pages == other.extra_pages
        )


class StateProofTxn(Transaction):
    """
    Represents a state proof transaction

    Arguments:
        sender (str): address of the sender
        state_proof (dict(), optional)
        state_proof_message (dict(), optional)
        state_proof_type (str, optional): state proof type
        sp (SuggestedParams): suggested params from algod


    Attributes:
        sender (str)
        sprf (dict())
        sprfmsg (dict())
        sprf_type (str)
        first_valid_round (int)
        last_valid_round (int)
        genesis_id (str)
        genesis_hash (str)
        type (str)
    """

    def __init__(
        self,
        sender,
        sp,
        state_proof=None,
        state_proof_message=None,
        state_proof_type=None,
    ):
        Transaction.__init__(
            self, sender, sp, None, None, constants.stateproof_txn, None
        )

        self.sprf_type = state_proof_type
        self.sprf = state_proof
        self.sprfmsg = state_proof_message

    @staticmethod
    def _undictify(d):
        args = {}
        if "sptype" in d:
            args["state_proof_type"] = d["sptype"]
        if "sp" in d:
            args["state_proof"] = d["sp"]
        if "spmsg" in d:
            args["state_proof_message"] = d["spmsg"]

        return args

    def __eq__(self, other):
        if not isinstance(other, StateProofTxn):
            return False
        return (
            super(StateProofTxn, self).__eq__(other)
            and self.sprf_type == other.sprf_type
            and self.sprf == other.sprf
            and self.sprfmsg == other.sprfmsg
        )
