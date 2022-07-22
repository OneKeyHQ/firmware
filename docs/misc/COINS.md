## Coins Definitions

We currently recognize six categories of coins: bitcoin, eth, erc20, nem, trx, misc. See [defs](https://github.com/OneKeyHQ/firmware/tree/master/common/defs)

## Adding a new coin

You can add new coin/token/network to the official Onekey firmware.

#### Forks and derivatives

If the coin you are adding is a fork of Bitcoin or other cryptocurrency we already support (in other words, new app is not needed) you can modify the definitions in the onekey-firmware repository and file a PR. In such case the coin does not have to be in TOP30 (see below), but we still reserve the right not to include the coin. The location depends on the type of the asset to be added:

- Bitcoin clones should be added to the [common/defs/bitcoin](https://github.com/OneKeyHQ/firmware/tree/master/common/defs/bitcoin) subdirectory as separate .json files
- Ethereum networks/chains should be added to the [ethereum-lists/chains](https://github.com/ethereum-lists/chains)
- Ethereum tokens should be added to the [ethereum-lists/tokens](https://github.com/ethereum-lists/tokens) repository
- NEM mosaics should be added to the [common/defs/nem/nem_mosaics.json](https://github.com/OneKeyHQ/firmware/tree/master/common/defs/nem/nem_mosaics.json) file
- TRON token should be added to the [common/defs/tron/tron_tokens.json](https://github.com/OneKeyHQ/firmware/tree/master/common/defs/tron/tron_tokens.json) file
- Other coins should be added to the [common/defs/misc/misc.json](https://github.com/OneKeyHQ/firmware/blob/master/common/defs/misc/misc.json) file. 

#### Misc

You can add a new coin to misc.json by following the steps below
1. [Protobuf Definitions](https://github.com/OneKeyHQ/firmware/blob/master/common/protob)
2. Implementation of user-facing features [app](https://github.com/OneKeyHQ/firmware/blob/master/core/src/apps)
3. [Command line client](https://github.com/OneKeyHQ/firmware/blob/master/python/src/trezorlib)
4. [Unit tests](https://github.com/OneKeyHQ/firmware/blob/master/core/tests)
5. [Device tests](https://github.com/OneKeyHQ/firmware/blob/master/tests/device_tests)

#### New category

In addition to the steps to add a coin to misc, [coin_info.py](https://github.com/OneKeyHQ/firmware/blob/master/common/tools/coin_info.py) needs to be modified and you can refer to [here](https://github.com/OneKeyHQ/firmware/pull/92)


## Pull request requirements

See [CONTRIBUTING.md](https://github.com/OneKeyHQ/firmware/blob/master/docs/misc/contributing.md)
