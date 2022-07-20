# Communication

_Note: In this section we describe the internal functioning of the communication protocol. If you wish to implement Onekey support you should use [Connect](https://github.com/OneKeyHQ/connect/) or [python library](https://github.com/OneKeyHQ/firmware/tree/master/python/), which will do all this hard work for you._

We use [Protobuf v2](https://developers.google.com/protocol-buffers/) for host-device communication. The communication cycle is very simple, Onekey receives a message (request), acts on it and responds with another one (response). Onekey on its own is incapable of initiating the communication.

## Definitions

Protobuf messages are defined in the [Common](https://github.com/OneKeyHQ/firmware/tree/master/common) project, which is part of this monorepo. This repository is also exported to [onekey/trezor-common](https://github.com/OneKeyHQ/trezor-common) to be used by third parties, which prefer not to include the whole monorepo. That copy is read-only mirror and all changes are happening in this monorepo.

## Notable topics

- [Sessions](sessions.md)
- [Passphrase](passphrase.md)
- [Bitcoin transaction signing](bitcoin-signing.md)

