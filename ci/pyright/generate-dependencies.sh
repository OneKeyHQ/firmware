#!/usr/bin/env nix-shell
#! nix-shell -i bash -p nodePackages.node2nix

node2nix \
  --nodejs-14 \
  --development \
  --input package.json \
  --output node-packages.nix \
  --composition node-composition.nix
