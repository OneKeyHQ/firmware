/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FSM_H__
#define __FSM_H__

#include "messages-aptos.pb.h"
#include "messages-bitcoin.pb.h"
#include "messages-conflux.pb.h"
#include "messages-crypto.pb.h"
#include "messages-debug.pb.h"
#include "messages-ethereum.pb.h"
#include "messages-management.pb.h"
#include "messages-near.pb.h"
#include "messages-nem.pb.h"
#include "messages-solana.pb.h"
#include "messages-starcoin.pb.h"
#include "messages-stellar.pb.h"
#include "messages-tron.pb.h"

// message functions

void fsm_sendSuccess(const char *text);

#if DEBUG_LINK
void fsm_sendFailureDebug(FailureType code, const char *text,
                          const char *source);

#define fsm_sendFailure(code, text) \
  fsm_sendFailureDebug((code), (text), __FILE__ ":" VERSTR(__LINE__) ":")
#else
void fsm_sendFailure(FailureType code, const char *text);
#endif

// void fsm_msgPinMatrixAck(const PinMatrixAck *msg);   // tiny
// void fsm_msgButtonAck(const ButtonAck *msg);         // tiny
// void fsm_msgPassphraseAck(const PassphraseAck *msg); // tiny

// common
void fsm_msgInitialize(const Initialize *msg);
void fsm_msgGetFeatures(const GetFeatures *msg);
void fsm_msgPing(const Ping *msg);
void fsm_msgChangePin(const ChangePin *msg);
void fsm_msgChangeWipeCode(const ChangeWipeCode *msg);
void fsm_msgWipeDevice(const WipeDevice *msg);
void fsm_msgGetEntropy(const GetEntropy *msg);
#if DEBUG_LINK
void fsm_msgLoadDevice(const LoadDevice *msg);
#endif
void fsm_msgResetDevice(const ResetDevice *msg);
void fsm_msgEntropyAck(const EntropyAck *msg);
void fsm_msgBackupDevice(const BackupDevice *msg);
void fsm_msgCancel(const Cancel *msg);
void fsm_msgLockDevice(const LockDevice *msg);
void fsm_msgEndSession(const EndSession *msg);
void fsm_msgApplySettings(const ApplySettings *msg);
void fsm_msgApplyFlags(const ApplyFlags *msg);
void fsm_msgRecoveryDevice(const RecoveryDevice *msg);
void fsm_msgWordAck(const WordAck *msg);
void fsm_msgSetU2FCounter(const SetU2FCounter *msg);
void fsm_msgGetNextU2FCounter(void);

// coin
void fsm_msgGetPublicKey(const GetPublicKey *msg);
void fsm_msgSignTx(const SignTx *msg);
void fsm_msgTxAck(
    TxAck *msg);  // not const because we mutate input/output scripts
void fsm_msgGetAddress(const GetAddress *msg);
void fsm_msgSignMessage(const SignMessage *msg);
void fsm_msgVerifyMessage(const VerifyMessage *msg);

// crypto
void fsm_msgCipherKeyValue(const CipherKeyValue *msg);
void fsm_msgSignIdentity(const SignIdentity *msg);
void fsm_msgGetECDHSessionKey(const GetECDHSessionKey *msg);
void fsm_msgCosiCommit(const CosiCommit *msg);
void fsm_msgCosiSign(const CosiSign *msg);
void fsm_msgBatchGetPublickeys(const BatchGetPublickeys *msg);

// debug
#if DEBUG_LINK
// void fsm_msgDebugLinkDecision(const DebugLinkDecision *msg); // tiny
void fsm_msgDebugLinkGetState(const DebugLinkGetState *msg);
void fsm_msgDebugLinkStop(const DebugLinkStop *msg);
void fsm_msgDebugLinkMemoryWrite(const DebugLinkMemoryWrite *msg);
void fsm_msgDebugLinkMemoryRead(const DebugLinkMemoryRead *msg);
void fsm_msgDebugLinkFlashErase(const DebugLinkFlashErase *msg);
#endif

// ethereum
void fsm_msgEthereumGetAddress(const EthereumGetAddress *msg);
void fsm_msgEthereumGetPublicKey(const EthereumGetPublicKey *msg);
void fsm_msgEthereumSignTx(const EthereumSignTx *msg);
void fsm_msgEthereumSignTxEIP1559(const EthereumSignTxEIP1559 *msg);
void fsm_msgEthereumTxAck(const EthereumTxAck *msg);
void fsm_msgEthereumSignMessage(const EthereumSignMessage *msg);
void fsm_msgEthereumVerifyMessage(const EthereumVerifyMessage *msg);
void fsm_msgEthereumSignMessageEIP712(const EthereumSignMessageEIP712 *msg);
void fsm_msgEthereumSignTypedHash(const EthereumSignTypedHash *msg);

// nem
void fsm_msgNEMGetAddress(
    NEMGetAddress *msg);  // not const because we mutate msg->network
void fsm_msgNEMSignTx(
    NEMSignTx *msg);  // not const because we mutate msg->network
void fsm_msgNEMDecryptMessage(
    NEMDecryptMessage *msg);  // not const because we mutate msg->payload

// solana
void fsm_msgSolanaGetAddress(const SolanaGetAddress *msg);
void fsm_msgSolanaSignTx(const SolanaSignTx *msg);

// starcoin
void fsm_msgStarcoinGetAddress(const StarcoinGetAddress *msg);
void fsm_msgStarcoinGetPublicKey(const StarcoinGetPublicKey *msg);
void fsm_msgStarcoinSignTx(const StarcoinSignTx *msg);
void fsm_msgStarcoinSignMessage(const StarcoinSignMessage *msg);
void fsm_msgStarcoinVerifyMessage(const StarcoinVerifyMessage *msg);

// stellar
void fsm_msgStellarGetAddress(const StellarGetAddress *msg);
void fsm_msgStellarSignTx(const StellarSignTx *msg);
void fsm_msgStellarPaymentOp(const StellarPaymentOp *msg);
void fsm_msgStellarCreateAccountOp(const StellarCreateAccountOp *msg);
void fsm_msgStellarPathPaymentStrictReceiveOp(
    const StellarPathPaymentStrictReceiveOp *msg);
void fsm_msgStellarPathPaymentStrictSendOp(
    const StellarPathPaymentStrictSendOp *msg);
void fsm_msgStellarManageBuyOfferOp(const StellarManageBuyOfferOp *msg);
void fsm_msgStellarManageSellOfferOp(const StellarManageSellOfferOp *msg);
void fsm_msgStellarCreatePassiveSellOfferOp(
    const StellarCreatePassiveSellOfferOp *msg);
void fsm_msgStellarSetOptionsOp(const StellarSetOptionsOp *msg);
void fsm_msgStellarChangeTrustOp(const StellarChangeTrustOp *msg);
void fsm_msgStellarAllowTrustOp(const StellarAllowTrustOp *msg);
void fsm_msgStellarAccountMergeOp(const StellarAccountMergeOp *msg);
void fsm_msgStellarManageDataOp(const StellarManageDataOp *msg);
void fsm_msgStellarBumpSequenceOp(const StellarBumpSequenceOp *msg);

void fsm_msgRebootToBootloader(void);

bool fsm_layoutSignMessage(const uint8_t *msg, uint32_t len);
bool fsm_layoutSignMessage_ex(const char *description, const uint8_t *msg,
                              uint32_t len);
bool fsm_layoutVerifyMessage(const uint8_t *msg, uint32_t len);

void fsm_msgBixinReboot(const BixinReboot *msg);
void fsm_msgBixinMessageSE(const BixinMessageSE *msg);
void fsm_msgBixinVerifyDeviceRequest(const BixinVerifyDeviceRequest *msg);
void fsm_msgBixinLoadDevice(const BixinLoadDevice *msg);
void fsm_msgBixinBackupDevice(void);

void fsm_msgDeviceInfoSettings(const DeviceInfoSettings *msg);
void fsm_msgGetDeviceInfo(const GetDeviceInfo *msg);
void fsm_msgReadSEPublicKey(const ReadSEPublicKey *msg);
void fsm_msgWriteSEPublicCert(const WriteSEPublicCert *msg);
void fsm_msgReadSEPublicCert(const ReadSEPublicCert *msg);
void fsm_msgSpiFlashWrite(const SpiFlashWrite *msg);
void fsm_msgSpiFlashRead(const SpiFlashRead *msg);
void fsm_msgSESignMessage(const SESignMessage *msg);
void fsm_msgNFTWriteInfo(const NFTWriteInfo *msg);
void fsm_msgNFTWriteData(const NFTWriteData *msg);

void fsm_msgGetPublicKeyMultiple(const GetPublicKeyMultiple *msg);

// tron
void fsm_msgTronSignMessage(TronSignMessage *msg);
void fsm_msgTronGetAddress(TronGetAddress *msg);
void fsm_msgTronSignTx(TronSignTx *msg);

// aptos
void fsm_msgAptosGetAddress(const AptosGetAddress *msg);
void fsm_msgAptosSignTx(const AptosSignTx *msg);

// near
void fsm_msgNearGetAddress(NearGetAddress *msg);
void fsm_msgNearSignTx(const NearSignTx *msg);

// conflux
void fsm_msgConfluxGetAddress(const ConfluxGetAddress *msg);
void fsm_msgConfluxSignTx(ConfluxSignTx *msg);  // not const because we mutate
                                                // transaction during validation
void fsm_msgConfluxTxAck(const ConfluxTxAck *msg);
void fsm_msgConfluxSignMessage(const ConfluxSignMessage *msg);

#endif
