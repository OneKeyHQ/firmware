We want to invite the wider community to participate in the verification of the firmware built by Onekey. 

We do daily builds via github action, onekey to multi-sign the firmware, and release official firmware.

The fingerprints of the CI builds are compared with the fingerprints of the official firmware for verification.

## Onekey Mini firmware 

1. Download the official firmware from [here](https://github.com/OneKeyHQ/firmware/releases)

```
   wget https://github.com/OneKeyHQ/firmware/releases/download/mini%2Fv2.3.0/mini.2.3.0-Stable-0810-2bcc073.bin --no-check-certificate
```

2. Download the [artifacts](https://github.com/OneKeyHQ/firmware/actions/runs/2829420581) for the corresponding ci build and unzip it

```
  unzip firmware-e132e608d09fff246ba36ef7737a5ca403a2af09.zip -d ci
```

3. Verify firmware, Those two hashes should be equal
```
  tail -c +1024 mini.2.3.0-Stable-0810-2bcc073.bin | shasum -a 256
  tail -c +1024 ci/mini.2.3.0-Stable-0810-2bcc073.bin | shasum -a 256
```

## Onekey Classic firmware 

See Onekey Mini firmware

## Onekey Touch firmware 

See Onekey Mini firmware
