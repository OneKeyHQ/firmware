/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include "bootloader.h"
#include "ecdsa.h"
#include "memory.h"
#include "memzero.h"
#include "secp256k1.h"
#include "sha2.h"
#include "signatures.h"

#if ONEKEY_MINI
#include "w25qxx.h"
#endif

const uint32_t FIRMWARE_MAGIC_OLD = 0x525a5254;  // TRZR
#if ONEKEY_MINI
const uint32_t FIRMWARE_MAGIC_NEW = 0x494e494d;  // MINI
#else
const uint32_t FIRMWARE_MAGIC_NEW = 0x465a5254;  // TRZF
#endif
const uint32_t FIRMWARE_MAGIC_BLE = 0x33383235;  // 5283

#if ONEKEY_MINI
#define PUBKEYS 7

static const uint8_t * const pubkey_Bixin[PUBKEYS] = {
	(const uint8_t *)"\x04\xd6\x5c\x46\x99\x40\x9d\x08\x63\x27\xfd\xd0\xb3\xab\x1d\x5a\xc3\xb1\x8f\x80\xf2\x4b\x76\xf4\xba\x6b\xc7\x1a\x8b\xa9\x4d\x18\x23\x5d\x87\xb9\xbf\xd8\xc9\x2b\xca\x82\x61\x77\x2f\x67\xda\x84\x8a\x79\x42\x13\x55\x01\xfd\x8e\x57\x35\x1e\x8e\xa9\x29\xab\x42\x9b",
	(const uint8_t *)"\x04\x75\x7f\x1f\x97\x34\xfc\xda\xe4\x01\x1c\x93\x40\x02\x66\xb0\x26\xdd\x09\x9a\x6f\x10\x0e\x9d\xe9\x0b\x16\xae\x16\x26\x29\x17\xe8\x80\x5d\xb5\xec\xae\x3e\x27\xd4\x55\xc9\x73\xe4\xab\xf7\x7c\x56\x3b\xfa\x73\x05\x6d\x42\xb6\x18\x7f\x5a\x36\x84\xfe\x06\x44\x71",
	(const uint8_t *)"\x04\x6d\xda\x87\xed\xc2\x42\x89\xae\xa9\x29\x29\x9a\xbb\x5a\x27\x9e\x91\x37\x34\xb6\x06\x42\xec\x7b\x73\x48\x91\xc4\x6c\xf0\x96\xfb\xf5\x2a\xf0\xf1\x98\x9a\x73\xd8\x6a\x2f\xe4\x35\x3e\x05\x52\x67\x3b\x1b\xd3\x02\x27\x58\x5e\x64\x52\x55\x44\x8d\x72\xe3\x7f\x2a",
	(const uint8_t *)"\x04\x91\x20\x57\x1a\xc8\xcc\x9d\x55\xa2\x91\xba\x52\xa6\xff\x23\x6b\x36\x5c\x83\x0d\xee\x9e\x73\x9c\x33\x7d\x33\xe2\x06\x44\x40\x66\x21\x01\x04\x33\xc4\xee\x64\x49\x87\xe5\xb5\x75\xb3\xba\x4b\xf1\x3f\xa0\x4c\xbe\xea\x4b\x25\xa0\x4d\xbf\xb1\x0e\xda\xd2\x97\x25",
	(const uint8_t *)"\x04\xa5\x46\xfb\x3a\xf2\x22\xe8\xfd\x2e\x18\x35\xd2\xe3\xf9\xe1\xfa\x43\x79\x99\x1c\x9a\x43\x10\x8d\x4c\x20\x92\x57\xf3\x11\xe4\xfa\xc4\xd0\x0a\x7c\x5f\xe3\xd3\xf2\x00\x0e\xdc\x25\xa5\x8d\xd1\x53\x02\x8e\xd9\xcd\xb2\x6f\xfb\x19\x51\x2a\x39\x81\xd7\x4a\xee\xd0",
	(const uint8_t *)"\x04\x20\x47\x66\xd9\x69\xb7\x3a\x0c\xfd\xeb\x1c\x4f\x86\x5a\x19\x33\x55\xba\x71\x42\x5f\x8b\x7f\x81\x5b\xe3\xd1\x5b\x5b\x37\x3d\xb0\x54\x95\x5a\x16\x37\xeb\x58\x26\x7d\xba\xab\x09\x1e\x9d\xb5\x89\x80\x51\x2c\x06\x7c\xc7\xc1\x3b\x93\xad\xa8\x7f\xad\x49\x16\x2d",
	(const uint8_t *)"\x04\xfe\xa4\x86\xff\xa8\xdf\x90\xb9\x3c\x71\x23\x13\x30\x2e\x52\xd9\x48\xdf\xc2\x49\x41\xbc\x9e\xa7\x74\x0b\xc2\x0b\x9a\x49\xeb\xe5\xca\x65\x49\xc2\xc6\xd2\xf0\x25\x81\xb6\xce\x79\xe4\x42\xab\x3c\xb1\xab\xc6\x1a\x00\x5a\xf4\xb3\x4e\x09\x45\x69\x7b\x1d\x62\x43",
};

#define SIGNATURES 4
#else
#define PUBKEYS 5

static const uint8_t * const pubkey_Bixin[PUBKEYS] = {
	(const uint8_t *)"\x04\xad\x90\x35\xd6\x7a\xc4\x79\x5c\x91\x3c\x45\x2d\x25\x15\x6f\x0b\x09\x4c\x34\xf6\x56\xa2\x49\xb9\x4d\x8d\x66\x19\xab\x0d\x92\xb1\xe8\xbc\xc3\x28\xbd\xc8\x33\xb9\xb5\x1c\xa3\x1b\xfd\x01\x36\x61\x51\x53\xf9\x3a\xba\x46\xd0\x2a\xb5\xd9\x25\xf4\xf3\x64\xc6\x66",
	(const uint8_t *)"\x04\x95\xb8\x3d\xa4\x42\xc6\x89\xbd\xa8\x2e\x9f\x95\x43\x81\x1f\xec\x2f\x58\x33\x4f\x5c\x76\x36\x1f\x5a\x49\xfb\xb6\x63\x4a\x81\x15\x2c\x6d\xa7\xb8\xa1\x78\x2c\xca\xa9\x28\x7e\xc7\xa8\xef\xe9\xd8\xbb\xa7\xd9\x01\x80\xf7\xb7\x19\xc0\x17\xd8\x04\xd6\x1c\x3d\x5d",
	(const uint8_t *)"\x04\xe0\xd5\xef\x94\xcf\x95\x0b\x9f\x85\x5f\xb5\x52\x67\x64\xdc\x28\xd2\xd9\x65\x82\xc1\xca\x1a\xd3\x9c\xab\xab\x65\x3e\x61\x98\xf8\x0e\x64\x95\xe5\x36\xb8\xbc\xe3\x78\x57\xda\xfc\x5a\x51\x95\x26\x24\xab\x08\x3c\x33\x16\x8e\xe3\xed\x83\xe0\x36\xde\xfa\xb8\x5c",
	(const uint8_t *)"\x04\xc9\xec\x74\x0c\xb3\x32\x81\x65\xdc\x3f\xdb\x93\xa5\x4e\x70\x75\x56\x9b\x7a\x54\xcd\xee\xc0\x21\x1f\xa9\xd9\x52\xc6\x64\x79\x60\xbd\x95\xf9\x94\xc8\x17\x45\x88\x92\xd5\xdb\x6e\xb3\x4d\xa7\x6c\xe0\x3c\x9c\x04\xa4\x32\x5c\x27\x52\x64\x75\x90\xb1\xa4\xf3\x65",
	(const uint8_t *)"\x04\xd2\x08\xab\xa7\x9b\x6f\x60\xf1\x78\x60\x81\x67\xdd\xb7\x77\x86\x0a\x81\x55\x02\x2e\x28\xf1\x20\xa4\x16\x30\x86\x9a\x4e\x0f\x0d\x16\x98\xa4\x0e\xed\x2a\xed\xf7\x48\x46\xe1\xe4\x01\xce\xe6\xfd\xb1\xe8\x11\x16\xd1\x4c\xd2\x97\x12\x70\xf2\xcb\x0c\x56\x2c\x2e",
};

#define SIGNATURES 3

#endif

#define FLASH_META_START 0x08008000
#define FLASH_META_CODELEN (FLASH_META_START + 0x0004)
#define FLASH_META_SIGINDEX1 (FLASH_META_START + 0x0008)
#define FLASH_META_SIGINDEX2 (FLASH_META_START + 0x0009)
#define FLASH_META_SIGINDEX3 (FLASH_META_START + 0x000A)
#define FLASH_OLD_APP_START 0x08010000
#define FLASH_META_SIG1 (FLASH_META_START + 0x0040)
#define FLASH_META_SIG2 (FLASH_META_START + 0x0080)
#define FLASH_META_SIG3 (FLASH_META_START + 0x00C0)

bool firmware_present_old(void) {
  if (memcmp(FLASH_PTR(FLASH_META_START), &FIRMWARE_MAGIC_OLD,
             4)) {  // magic does not match
    return false;
  }
  if (*((const uint32_t *)FLASH_PTR(FLASH_META_CODELEN)) <
      8192) {  // firmware reports smaller size than 8192
    return false;
  }
  if (*((const uint32_t *)FLASH_PTR(FLASH_META_CODELEN)) >
      FLASH_APP_LEN) {  // firmware reports bigger size than flash size
    return false;
  }

  return true;
}

int signatures_old_ok(void) {
  const uint32_t codelen = *((const uint32_t *)FLASH_META_CODELEN);
  const uint8_t sigindex1 = *((const uint8_t *)FLASH_META_SIGINDEX1);
  const uint8_t sigindex2 = *((const uint8_t *)FLASH_META_SIGINDEX2);
  const uint8_t sigindex3 = *((const uint8_t *)FLASH_META_SIGINDEX3);

  if (codelen > FLASH_APP_LEN) {
    return false;
  }

  uint8_t hash[32] = {0};
  sha256_Raw(FLASH_PTR(FLASH_OLD_APP_START), codelen, hash);

  if (sigindex1 < 1 || sigindex1 > PUBKEYS) return SIG_FAIL;  // invalid index
  if (sigindex2 < 1 || sigindex2 > PUBKEYS) return SIG_FAIL;  // invalid index
  if (sigindex3 < 1 || sigindex3 > PUBKEYS) return SIG_FAIL;  // invalid index

  if (sigindex1 == sigindex2) return SIG_FAIL;  // duplicate use
  if (sigindex1 == sigindex3) return SIG_FAIL;  // duplicate use
  if (sigindex2 == sigindex3) return SIG_FAIL;  // duplicate use

  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_Bixin[sigindex1 - 1],
                               (const uint8_t *)FLASH_META_SIG1,
                               hash)) {  // failure
    return SIG_FAIL;
  }
  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_Bixin[sigindex2 - 1],
                               (const uint8_t *)FLASH_META_SIG2,
                               hash)) {  // failure
    return SIG_FAIL;
  }
  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_Bixin[sigindex3 - 1],
                               (const uint8_t *)FLASH_META_SIG3,
                               hash)) {  // failture
    return SIG_FAIL;
  }

  return SIG_OK;
}

void compute_firmware_fingerprint(const image_header *hdr, uint8_t hash[32]) {
  image_header copy = {0};
  memcpy(&copy, hdr, sizeof(image_header));
  memzero(copy.sig1, sizeof(copy.sig1));
  memzero(copy.sig2, sizeof(copy.sig2));
  memzero(copy.sig3, sizeof(copy.sig3));
  copy.sigindex1 = 0;
  copy.sigindex2 = 0;
  copy.sigindex3 = 0;
#if ONEKEY_MINI
  memzero(copy.sig4, sizeof(copy.sig4));
  copy.sigindex4 = 0;
#endif
  sha256_Raw((const uint8_t *)&copy, sizeof(image_header), hash);
}

bool firmware_present_new(void) {
  const image_header *hdr =
      (const image_header *)FLASH_PTR(FLASH_FWHEADER_START);
  if (hdr->magic != FIRMWARE_MAGIC_NEW) return false;
  // we need to ignore hdrlen for now
  // because we keep reset_handler ptr there
  // for compatibility with older bootloaders
  // after this is no longer necessary, let's uncomment the line below:
  // if (hdr->hdrlen != FLASH_FWHEADER_LEN) return false;
  if (hdr->codelen > FLASH_APP_LEN) return false;
  if (hdr->codelen < 4096) return false;

  return true;
}

int signatures_new_ok(const image_header *hdr, uint8_t store_fingerprint[32]) {
  uint8_t hash[32] = {0};
  compute_firmware_fingerprint(hdr, hash);

  if (store_fingerprint) {
    memcpy(store_fingerprint, hash, 32);
  }

  if (hdr->sigindex1 < 1 || hdr->sigindex1 > PUBKEYS)
    return SIG_FAIL;  // invalid index
  if (hdr->sigindex2 < 1 || hdr->sigindex2 > PUBKEYS)
    return SIG_FAIL;  // invalid index
  if (hdr->sigindex3 < 1 || hdr->sigindex3 > PUBKEYS)
    return SIG_FAIL;  // invalid index

  if (hdr->sigindex1 == hdr->sigindex2) return SIG_FAIL;  // duplicate use
  if (hdr->sigindex1 == hdr->sigindex3) return SIG_FAIL;  // duplicate use
  if (hdr->sigindex2 == hdr->sigindex3) return SIG_FAIL;  // duplicate use

#if ONEKEY_MINI
  if (hdr->sigindex4 < 1 || hdr->sigindex4 > PUBKEYS)
    return SIG_FAIL;                                      // invalid index
  if (hdr->sigindex4 == hdr->sigindex3) return SIG_FAIL;  // duplicate use
#endif

  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_Bixin[hdr->sigindex1 - 1],
                               hdr->sig1, hash)) {  // failure
    return SIG_FAIL;
  }
  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_Bixin[hdr->sigindex1 - 1],
                               hdr->sig1, hash)) {  // failure
    return SIG_FAIL;
  }
  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_Bixin[hdr->sigindex1 - 1],
                               hdr->sig1, hash)) {  // failure
    return SIG_FAIL;
  }

#if ONEKEY_MINI
  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_Bixin[hdr->sigindex4 - 1],
                               hdr->sig4, hash)) {  // failure
    return SIG_FAIL;
  }
#endif

  return SIG_OK;
}

int mem_is_empty(const uint8_t *src, uint32_t len) {
  for (uint32_t i = 0; i < len; i++) {
    if (src[i]) return 0;
  }
  return 1;
}

int check_firmware_hashes(const image_header *hdr) {
  uint8_t hash[32] = {0};
  // check hash of the first code chunk
  sha256_Raw(FLASH_PTR(FLASH_APP_START), (64 - 1) * 1024, hash);
  if (0 != memcmp(hash, hdr->hashes, 32)) return SIG_FAIL;
  // check remaining used chunks
  uint32_t total_len = FLASH_FWHEADER_LEN + hdr->codelen;
  int used_chunks = total_len / FW_CHUNK_SIZE;
  if (total_len % FW_CHUNK_SIZE > 0) {
    used_chunks++;
  }
  for (int i = 1; i < used_chunks; i++) {
    sha256_Raw(FLASH_PTR(FLASH_FWHEADER_START + (64 * i) * 1024), 64 * 1024,
               hash);
    if (0 != memcmp(hdr->hashes + 32 * i, hash, 32)) return SIG_FAIL;
  }
  // check unused chunks
  for (int i = used_chunks; i < 16; i++) {
    if (!mem_is_empty(hdr->hashes + 32 * i, 32)) return SIG_FAIL;
  }
  // all OK
  return SIG_OK;
}

#if ONEKEY_MINI
extern uint32_t FW_CHUNK[];
int check_firmware_hashes_ex(const image_header *hdr) {
  uint8_t hash[32] = {0};
  // check hash of the first code chunk
  w25qxx_read_bytes((uint8_t *)FW_CHUNK,
                    SPI_FLASH_FIRMWARE_ADDR_START + FLASH_FWHEADER_LEN,
                    (64 - 1) * 1024);
  sha256_Raw((uint8_t *)FW_CHUNK, (64 - 1) * 1024, hash);
  if (0 != memcmp(hash, hdr->hashes, 32)) return SIG_FAIL;
  // check remaining used chunks
  uint32_t total_len = FLASH_FWHEADER_LEN + hdr->codelen;
  int used_chunks = total_len / FW_CHUNK_SIZE;
  if (total_len % FW_CHUNK_SIZE > 0) {
    used_chunks++;
  }
  for (int i = 1; i < used_chunks; i++) {
    w25qxx_read_bytes((uint8_t *)FW_CHUNK,
                      SPI_FLASH_FIRMWARE_ADDR_START + (64 * i) * 1024,
                      64 * 1024);
    sha256_Raw((uint8_t *)FW_CHUNK, 64 * 1024, hash);
    if (0 != memcmp(hdr->hashes + 32 * i, hash, 32)) return SIG_FAIL;
  }
  // check unused chunks
  for (int i = used_chunks; i < 16; i++) {
    if (!mem_is_empty(hdr->hashes + 32 * i, 32)) return SIG_FAIL;
  }
  // all OK
  return SIG_OK;
}
#endif
