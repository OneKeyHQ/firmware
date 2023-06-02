```shell
# reboot normally
trezorctl device reboot -t 0

# reboot stay in board
trezorctl device reboot -t 1

# reboot stay in boot
trezorctl device reboot -t 2

# get path information
trezorctl device emmc-path-info -p 0:firmware.bin

# read file
trezorctl device emmc-file-read -l ../core/build/firmware/fff.bin -r 0:firmware.bin -f -cs 256KB

# write file
trezorctl device emmc-file-write -l ../core/build/firmware/firmware.bin -r 0:firmware.bin -f -cs 256KB

# list files and folders, always recursive
trezorctl device emmc-dir-list -p "0:"

# create folder[s], always recursive
trezorctl device emmc-dir-make -p "0:level1/l2/3/4/5"

# remove folder and contents , always recursive
trezorctl device emmc-dir-remove -p "0:level1" # this will also remove "/l2/3/4/5"

# update bootloader
trezorctl device emmc-file-write -l ../core/build/bootloader/bootloader.bin -r 0:boot/bootloader.bin
trezorctl device reboot -t 0

# update bluetooth
trezorctl device emmc-file-write -l /home/adam/Downloads/touch_ble_signed-2022-1102_2.1.0.bin -r 0:bluetooth.bin
trezorctl device firmware-update-emmc -p 0:bluetooth.bin

# update main firmware
trezorctl device emmc-file-write -l ../core/build/firmware/firmware.bin -r 0:firmware.bin -f
trezorctl device firmware-update-emmc -p 0:firmware.bin
```