## OneKey

 **🔑 OneKey is the smartest way to secure, buy, exchange and grow your crypto assets.** Learn more [here](https://www.onekey.so/).

----

![make something people want](https://github.com/rayston92/graph_bed/blob/edc43bc1cd6ce6a19f3b6c30624a4b58f6870d1f/img/github_home_bg.png?raw=true)


## Repository Structure

* **[`ci`](ci/)**: [Gitlab CI](https://gitlab.com/satoshilabs/trezor/trezor-firmware) configuration files
* **[`common/defs`](common/defs/)**: JSON coin definitions and support tables
* **[`common/protob`](common/protob/)**: Common protobuf definitions for the Trezor protocol
* **[`common/tools`](common/tools/)**: Tools for managing coin definitions and related data
* **[`core`](core/)**: Trezor Core, firmware implementation for Trezor T
* **[`crypto`](crypto/)**: Stand-alone cryptography library used by both Trezor Core and the Trezor One firmware
* **[`docs`](docs/)**: Assorted documentation
* **[`legacy`](legacy/)**: Trezor One firmware implementation
* **[`python`](python/)**: Python [client library](https://pypi.org/project/trezor) and the `trezorctl` command
* **[`storage`](storage/)**: NORCOW storage implementation used by both Trezor Core and the Trezor One firmware
* **[`tests`](tests/)**: Firmware unit test suite
* **[`tools`](tools/)**: Miscellaneous build and helper scripts
* **[`vendor`](vendor/)**: Submodules for external dependencies


## Contribute

See [CONTRIBUTING.md](docs/misc/contributing.md).

Using [Conventional Commits](COMMITS.md) is strongly recommended and might be enforced in future.

Also please have a look at the docs, either in the `docs` folder or at  [docs.trezor.io](https://docs.trezor.io) before contributing. The [misc](docs/misc/index.md) chapter should be read in particular because it contains some useful assorted knowledge.

## Security vulnerability disclosure

Please report suspected security vulnerabilities in private to [dev@onekey.so](mailto:dev@onekey.so), Please do NOT create publicly viewable issues for suspected security vulnerabilities.

## Documentation

See the `docs` folder or visit [docs.trezor.io](https://docs.trezor.io).
