FROM ubuntu:focal

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install --no-install-recommends -y git wget make autoconf automake libtool curl make g++ unzip scons llvm-dev libclang-dev clang libsdl2-dev libsdl2-image-dev  xz-utils vim  ca-certificates && apt-get clean && rm -rf /var/lib/apt/lists/* \
      && mkdir -m 0755 /nix && groupadd -r nixbld && chown root /nix \
      && for n in $(seq 1 10); do useradd -c "Nix build user $n" -d /var/empty -g nixbld -G nixbld -M -N -r -s "$(command -v nologin)" "nixbld$n"; done
SHELL ["/bin/bash", "-o", "pipefail", "-c"]
RUN set -o pipefail && curl -L https://nixos.org/nix/install | bash

ENV USER=root
RUN . "$HOME/.nix-profile/etc/profile.d/nix.sh"
RUN echo "source $HOME/.nix-profile/etc/profile.d/nix.sh" >> "$HOME/.bashrc" 

