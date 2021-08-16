NAME  = bootloader

OBJS += bootloader.o
OBJS += signatures.o
OBJS += usb.o

ifneq ($(ONEKEY_MINI),1)
OBJS += swd.o
OBJS += updateble.o
else
OBJS += messages.o
OBJS += ../vendor/nanopb/pb_common.o
OBJS += ../vendor/nanopb/pb_decode.o
OBJS += ../vendor/nanopb/pb_encode.o
OBJS += protob/messages-management.pb.o
CFLAGS += -I../vendor/nanopb -Iprotob -DPB_FIELD_16BIT=1 -DPB_ENCODE_ARRAYS_UNPACKED=1 -DPB_VALIDATE_UTF8=1
endif

OBJS += layout_boot.o

# overrides from libtrezor
ifneq ($(ONEKEY_MINI),1)
CFLAGS += -DFONT_SKIP_FIXED=1
endif
OBJS += ../gen/fonts.small.o

# overrides from trezor-crypto
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

align: $(NAME).bin
	./firmware_align.py $(NAME).bin
