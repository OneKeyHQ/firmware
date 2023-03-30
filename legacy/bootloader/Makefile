BOOT_BUILD_VERSION=$(shell ../../tools/version.sh version.h)
BOOT_BUILD_DATE=$(shell date "+%m%d")
BUILD_COMMIT=$(shell git rev-parse HEAD | cut -c1-7)
NAME  = bootloader.$(BOOT_BUILD_VERSION)-Stable-$(BOOT_BUILD_DATE)-$(BUILD_COMMIT)

OBJS += startup.o
OBJS += bootloader.o
OBJS += usb.o
OBJS += swd.o
OBJS += updateble.o
OBJS += layout_boot.o

# Overrides from libtrezor.
# Objects ending with 'small.o' are built with -Os to make the bootloader smaller.
CFLAGS += -DFONT_SKIP_FIXED=1
OBJS += ../gen/fonts.small.o

# Overrides from trezor-crypto
CFLAGS += -DUSE_PRECOMPUTED_CP=0
CFLAGS += -DUSE_PRECOMPUTED_IV=0
CFLAGS += -DUSE_PRECOMPUTED_CP=0
OBJS += ../vendor/trezor-crypto/bignum.small.o
OBJS += ../vendor/trezor-crypto/ecdsa.small.o
OBJS += ../vendor/trezor-crypto/secp256k1.small.o
OBJS += ../vendor/trezor-crypto/sha2.small.o
OBJS += ../vendor/trezor-crypto/memzero.small.o
OBJS += ../vendor/trezor-crypto/hmac_drbg.small.o

OPTFLAGS ?= -Os

include ../Makefile.include

bootloader.bin: $(NAME).bin
	./firmware_align.py $< $@

all: bootloader.bin
