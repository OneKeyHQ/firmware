<p align="center">
<img width="200" src="https://github.com/rayston92/graph_bed/blob/e3b2c938fc5b17d68531f69178908afb16266e6a/img/onekey_logo_badge_border.png?raw=trueg"/>
</p>

---

[![Github Stars](https://img.shields.io/github/stars/OneKeyHQ/firmware?t&logo=github&style=for-the-badge&labelColor=000)](https://github.com/OneKeyHQ/firmware/stargazers)
[![Version](https://img.shields.io/github/release/OneKeyHQ/firmware.svg?style=for-the-badge&labelColor=000)](https://github.com/OneKeyHQ/firmware/releases)
[![](https://img.shields.io/github/contributors-anon/OneKeyHQ/firmware?style=for-the-badge&labelColor=000)](https://github.com/OneKeyHQ/firmware/graphs/contributors)
[![Last commit](https://img.shields.io/github/last-commit/OneKeyHQ/firmware.svg?style=for-the-badge&labelColor=000)](https://github.com/OneKeyHQ/firmware/commits/onekey)
[![Issues](https://img.shields.io/github/issues-raw/OneKeyHQ/firmware.svg?style=for-the-badge&labelColor=000)](https://github.com/OneKeyHQ/firmware/issues?q=is%3Aissue+is%3Aopen)
[![Pull Requests](https://img.shields.io/github/issues-pr-raw/OneKeyHQ/firmware.svg?style=for-the-badge&labelColor=000)](https://github.com/OneKeyHQ/firmware/pulls?q=is%3Apr+is%3Aopen)
[![Discord](https://img.shields.io/discord/868309113942196295?style=for-the-badge&labelColor=000)](https://discord.gg/onekey)
[![Twitter Follow](https://img.shields.io/twitter/follow/OneKeyHQ?style=for-the-badge&labelColor=000)](https://twitter.com/OneKeyHQ)


## Community & Support

- [Community Forum](https://github.com/orgs/OneKeyHQ/discussions). Best for: help with building, discussion about best practices.
- [GitHub Issues](https://github.com/OneKeyHQ/firmware/issues). Best for: bugs and errors you encounter using OneKey.
- [Discord](https://discord.gg/onekey). Best for: sharing your ideas and hanging out with the community.


## 🚀 Getting Onboard

1. Install [nix](https://nixos.org/download.html)
2. Pulling the latest code via the git command line tool,  setting up the development environment

```
  git clone -b mini https://github.com/OneKeyHQ/onekey-firmware.git onekey-firmware
  cd onekey-firmware
  nix-shell
  poetry install
```

3. Run the build with:

```
  cd legacy
  export ONEKEY_MINI=1 EMULATOR=1 DEBUG_LINK=1 DEBUG_LOG=1
  poetry run make vendor
  poetry run ./script/setup
  poetry run ./script/cibuild
```

4. Now you can start the emulator

```
   ./firmware/mini*Stable*.bin
```

5. You can now install the command line client utility to interact with the emulator

```
   cd python && poetry run python3 -m pip install .
```
## ✏ Contribute

- Adding a small feature or a fix

  If your change is somewhat subtle, feel free to file a PR in one of the appropriate repositories directly. See the PR requirements noted at [CONTRIBUTING.md](https://github.com/OneKeyHQ/firmware/blob/touch/docs/misc/contributing.md)

- Add new coin/token/network to the official onekey firmware

  See [COINS.md](https://github.com/OneKeyHQ/firmware/blob/touch/docs/misc/COINS.md)

Also please have a look at the [docs](https://github.com/OneKeyHQ/firmware/blob/touch/docs/SUMMARY.md) before contributing. The misc chapter should be read in particular because it contains some useful assorted knowledge.

## 🔒 Security

- Please read [Bug Bunty Rules](https://github.com/OneKeyHQ/app-monorepo/blob/onekey/docs/BUG_RULES.md), we have detailed the exact plan in this article.
- Please report suspected security vulnerabilities in private to dev@onekey.so
- Please do NOT create publicly viewable issues for suspected security vulnerabilities.
- As an open source project, although we are not yet profitable, we try to give some rewards to white hat hackers who disclose vulnerabilities to us in a timely manner.

## ✨ Salute!

[![](https://img.shields.io/github/contributors-anon/OneKeyHQ/firmware?style=for-the-badge&labelColor=000)](https://github.com/OneKeyHQ/firmware/graphs/contributors)

<a href="https://github.com/onekeyhq/firmware/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=onekeyhq/firmware&max=240&columns=24"/>
</a>