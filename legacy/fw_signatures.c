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

#include "ecdsa.h"
#include "fw_signatures.h"
#include "memory.h"
#include "memzero.h"
#include "secbool.h"
#include "secp256k1.h"
#include "sha2.h"

const uint32_t FIRMWARE_MAGIC_NEW = 0x465a5254;  // TRZF
const uint32_t FIRMWARE_MAGIC_BLE = 0x33383235;  // 5283

/*
 * There are 3 schemes in history of T1, for clarity naming:
 *
 * - v1 - previously called "old" with TRZR magic header (no longer here)
 * - v2 - previously called "new" with TRZF magic header
 * - v3 - the latest scheme using Trezor's SignMessage and VerifyMessage
 *   style signatures
 *
 * See `debug_signing/README.md` and the scripts there for signatures debug.
 *
 * Latest scheme v3 ref: https://github.com/trezor/trezor-firmware/issues/2513
 */
#define PUBKEYS_V3 7
#define PUBKEYS_V2 5

#if DEBUG_T1_SIGNATURES || BOOTLOADER_QA

// Make build explode if combining debug sigs with production
#if PRODUCTION
#error "Can't have production device with debug keys! Build aborted"
#endif

// These are **only** for debugging signatures with SignMessage
// Use this mnemonic for testing signing:
// "table table table table table table table table table table table advance"
// the "SignMessage"-style public keys, third signing scheme
// See legacy/debug_signing/README.md
static const uint8_t * const pubkey_v3[PUBKEYS_V3] = {
    (const uint8_t *)"\x03\xc0\xfd\x4e\x0b\x9f\x9d\xaa\x68\x44\xd7\x48\xa6\x46\x6c\xa9\xd9\x8b\xf8\xb5\x5c\xa4\xf8\x62\xc8\x8a\xbb\x54\x78\x91\xba\x85\x24",
    (const uint8_t *)"\x03\x5b\xbf\x20\x1b\xb0\x77\x9a\x3b\x9c\xdf\xe6\x95\xc0\xdc\x93\xa5\x05\x7a\x05\x0e\x78\x8b\xda\xd0\x81\xa2\x10\x3f\xe4\xd1\x1c\xdc",
    (const uint8_t *)"\x03\x61\x9d\x92\x74\x22\x75\x4d\xa4\xf2\x48\x82\x4e\x39\x60\xd0\xe1\xea\x4a\x6e\x81\x41\x77\x91\xc3\x50\xf4\x37\x82\x31\xce\xcc\x9b",
    (const uint8_t *)"\x02\x3e\x0a\x84\x69\xfb\x3b\x5a\x9e\xfe\xa8\x47\x20\x90\x93\x1b\x01\xc1\xf1\xbb\x96\xde\xe2\xec\x54\x84\x86\xba\xed\xcf\x89\x87\xb2",
    (const uint8_t *)"\x02\x25\x48\x70\xd2\xcc\x6e\x67\xe3\x73\x7f\x7f\x7c\x0c\x47\xe6\xf1\x14\xca\x35\x30\xe5\x3e\xa3\xc7\xe0\x04\x26\x45\xe6\xa6\xc3\x8f",
    (const uint8_t *)"\x02\x02\xd2\x76\x43\xed\x23\xf2\xdb\x53\x11\xd5\xea\x73\x05\xa0\x96\x2d\x0a\xa6\xec\x76\x2e\x59\x4e\x5d\x05\x60\xbb\x7a\x25\x21\x08",
    (const uint8_t *)"\x02\x15\x67\x65\xb8\xf3\x38\x91\x17\xae\x8c\x59\x09\x41\x87\x78\x4d\x26\xa6\x10\xfd\xb7\x27\xc5\x57\x6c\x70\x6b\x45\xc6\x86\x26\x86"
};

// the "new", or second signing scheme keys

/*
 Debug private keys for v2 (previously called "new") scheme
 corresponding to pubkeys below as python hexstring array:

 ['4444444444444444444444444444444444444444444444444444444444444444',
  '4545454545454545454545454545454545454545454545454545454545454545',
  'bfc4bca9c9c228a16639d3503d999a733a439210b64cebe757a4fd03ca46a5c8',
  '5518381d95e93e8eb68a294354989906e3828f36b4556a2ad85d8333294eb1b7',
  '1d1d34168760dec092c9ff89377d8659076d2dfd95e0281719c15f90d067e211']
 */

static const uint8_t * const pubkey_v2[PUBKEYS_V2] = {
    (const uint8_t *)"\x02\xad\x90\x35\xd6\x7a\xc4\x79\x5c\x91\x3c\x45\x2d\x25\x15\x6f\x0b\x09\x4c\x34\xf6\x56\xa2\x49\xb9\x4d\x8d\x66\x19\xab\x0d\x92\xb1",
    (const uint8_t *)"\x03\x95\xb8\x3d\xa4\x42\xc6\x89\xbd\xa8\x2e\x9f\x95\x43\x81\x1f\xec\x2f\x58\x33\x4f\x5c\x76\x36\x1f\x5a\x49\xfb\xb6\x63\x4a\x81\x15",
    (const uint8_t *)"\x02\xe0\xd5\xef\x94\xcf\x95\x0b\x9f\x85\x5f\xb5\x52\x67\x64\xdc\x28\xd2\xd9\x65\x82\xc1\xca\x1a\xd3\x9c\xab\xab\x65\x3e\x61\x98\xf8",
    (const uint8_t *)"\x03\xc9\xec\x74\x0c\xb3\x32\x81\x65\xdc\x3f\xdb\x93\xa5\x4e\x70\x75\x56\x9b\x7a\x54\xcd\xee\xc0\x21\x1f\xa9\xd9\x52\xc6\x64\x79\x60",
    (const uint8_t *)"\x02\xd2\x08\xab\xa7\x9b\x6f\x60\xf1\x78\x60\x81\x67\xdd\xb7\x77\x86\x0a\x81\x55\x02\x2e\x28\xf1\x20\xa4\x16\x30\x86\x9a\x4e\x0f\x0d"
};

#else  // DEBUG_T1_SIGNATURES is now 0
// These public keys are production keys
// - used in production devices

// the "SignMessage"-style public keys, third signing scheme
static const uint8_t * const pubkey_v3[PUBKEYS_V3] = {
    (const uint8_t *)"\x03\x91\x20\x57\x1a\xc8\xcc\x9d\x55\xa2\x91\xba\x52\xa6\xff\x23\x6b\x36\x5c\x83\x0d\xee\x9e\x73\x9c\x33\x7d\x33\xe2\x06\x44\x40\x66",
    (const uint8_t *)"\x03\x5c\x4e\xe8\xd3\x60\xb8\xed\x17\x25\xa3\xf8\x53\x68\xa1\x4b\xb0\x6d\xdc\x1e\xc5\x4c\x5c\xdb\xbd\xfd\x97\x17\x0f\x11\xef\x32\xd6",
    (const uint8_t *)"\x02\x6d\xda\x87\xed\xc2\x42\x89\xae\xa9\x29\x29\x9a\xbb\x5a\x27\x9e\x91\x37\x34\xb6\x06\x42\xec\x7b\x73\x48\x91\xc4\x6c\xf0\x96\xfb",
    (const uint8_t *)"\x02\xa5\x46\xfb\x3a\xf2\x22\xe8\xfd\x2e\x18\x35\xd2\xe3\xf9\xe1\xfa\x43\x79\x99\x1c\x9a\x43\x10\x8d\x4c\x20\x92\x57\xf3\x11\xe4\xfa",
    (const uint8_t *)"\x03\xfe\xa4\x86\xff\xa8\xdf\x90\xb9\x3c\x71\x23\x13\x30\x2e\x52\xd9\x48\xdf\xc2\x49\x41\xbc\x9e\xa7\x74\x0b\xc2\x0b\x9a\x49\xeb\xe5",
    (const uint8_t *)"\x03\x20\x47\x66\xd9\x69\xb7\x3a\x0c\xfd\xeb\x1c\x4f\x86\x5a\x19\x33\x55\xba\x71\x42\x5f\x8b\x7f\x81\x5b\xe3\xd1\x5b\x5b\x37\x3d\xb0",
    (const uint8_t *)"\x02\xe4\x2f\xc6\x6a\xb1\x1b\x68\x3b\x46\x7d\x8d\x7f\xb1\x3f\x35\x62\x60\xee\xc3\x67\x6a\xdb\x64\xec\x20\x1f\xfa\xa1\x3d\x2a\x5f\x1e"
};

// the "new", or second signing scheme keys
static const uint8_t * const pubkey_v2[PUBKEYS_V2] = {
    (const uint8_t *)"\x02\xad\x90\x35\xd6\x7a\xc4\x79\x5c\x91\x3c\x45\x2d\x25\x15\x6f\x0b\x09\x4c\x34\xf6\x56\xa2\x49\xb9\x4d\x8d\x66\x19\xab\x0d\x92\xb1",
    (const uint8_t *)"\x03\x95\xb8\x3d\xa4\x42\xc6\x89\xbd\xa8\x2e\x9f\x95\x43\x81\x1f\xec\x2f\x58\x33\x4f\x5c\x76\x36\x1f\x5a\x49\xfb\xb6\x63\x4a\x81\x15",
    (const uint8_t *)"\x02\xe0\xd5\xef\x94\xcf\x95\x0b\x9f\x85\x5f\xb5\x52\x67\x64\xdc\x28\xd2\xd9\x65\x82\xc1\xca\x1a\xd3\x9c\xab\xab\x65\x3e\x61\x98\xf8",
    (const uint8_t *)"\x03\xc9\xec\x74\x0c\xb3\x32\x81\x65\xdc\x3f\xdb\x93\xa5\x4e\x70\x75\x56\x9b\x7a\x54\xcd\xee\xc0\x21\x1f\xa9\xd9\x52\xc6\x64\x79\x60",
    (const uint8_t *)"\x02\xd2\x08\xab\xa7\x9b\x6f\x60\xf1\x78\x60\x81\x67\xdd\xb7\x77\x86\x0a\x81\x55\x02\x2e\x28\xf1\x20\xa4\x16\x30\x86\x9a\x4e\x0f\x0d"
};
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

/*
 * 0x18 in message prefix is coin info, 0x20 is the length of hash
 * that follows.
 * See `core/src/apps/bitcoin/sign_message.py`.
 */
#define VERIFYMESSAGE_PREFIX \
  ("\x18"                    \
   "Bitcoin Signed Message:\n\x20")
#define PREFIX_LENGTH (sizeof(VERIFYMESSAGE_PREFIX) - 1)
#define SIGNED_LENGTH (PREFIX_LENGTH + 32)

void compute_firmware_fingerprint(const image_header *hdr, uint8_t hash[32]) {
  image_header copy = {0};
  memcpy(&copy, hdr, sizeof(image_header));
  memzero(copy.sig1, sizeof(copy.sig1));
  memzero(copy.sig2, sizeof(copy.sig2));
  memzero(copy.sig3, sizeof(copy.sig3));
  memzero(copy.signatures_4.sig4, sizeof(copy.signatures_4.sig4));
  copy.signatures_4.sigindex1 = 0;
  copy.signatures_4.sigindex2 = 0;
  copy.signatures_4.sigindex3 = 0;
  copy.signatures_4.sigindex4 = 0;
  sha256_Raw((const uint8_t *)&copy, sizeof(image_header), hash);
}

void compute_firmware_fingerprint_for_verifymessage(const image_header *hdr,
                                                    uint8_t hash[32]) {
  uint8_t prefixed_header[SIGNED_LENGTH] = VERIFYMESSAGE_PREFIX;
  uint8_t header_hash[32];
  uint8_t hash_before_double_hashing[32];
  compute_firmware_fingerprint(hdr, header_hash);
  memcpy(prefixed_header + PREFIX_LENGTH, header_hash, sizeof(header_hash));
  sha256_Raw(prefixed_header, sizeof(prefixed_header),
             hash_before_double_hashing);
  // We need to do hash the previous result again because SignMessage
  // computes it this way, see `core/src/apps/bitcoin/sign_message.py`
  sha256_Raw(hash_before_double_hashing, sizeof(hash_before_double_hashing),
             hash);
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

int signatures_ok(const image_header *hdr, uint8_t store_fingerprint[32],
                  secbool use_verifymessage) {
  uint8_t hash[32] = {0};
  // which set of public keys depend on scheme
  const uint8_t *const *pubkey_ptr = NULL;
  uint8_t pubkeys = 0;
  uint8_t sigindex1, sigindex2, sigindex3, sigindex4;
  if (hdr->signatures_4.sigindex1 >= 1 &&
      hdr->signatures_4.sigindex1 <= PUBKEYS_V3) {  // 4/7 signatures v3
    if (use_verifymessage != sectrue) {
      return SIG_FAIL;
    }
    pubkey_ptr = pubkey_v3;
    compute_firmware_fingerprint_for_verifymessage(hdr, hash);
    pubkeys = PUBKEYS_V3;
    sigindex1 = hdr->signatures_4.sigindex1;
    sigindex2 = hdr->signatures_4.sigindex2;
    sigindex3 = hdr->signatures_4.sigindex3;
    sigindex4 = hdr->signatures_4.sigindex4;
  } else {  // 3/5 signatures v2
    if (use_verifymessage != secfalse) {
      return SIG_FAIL;
    }
    pubkey_ptr = pubkey_v2;
    compute_firmware_fingerprint(hdr, hash);
    pubkeys = PUBKEYS_V2;
    sigindex1 = hdr->signatures_3.sigindex1;
    sigindex2 = hdr->signatures_3.sigindex2;
    sigindex3 = hdr->signatures_3.sigindex3;
    sigindex4 = 0;
  }

  if (store_fingerprint) {
    memcpy(store_fingerprint, hash, 32);
  }

  if (sigindex1 < 1 || sigindex1 > pubkeys) return SIG_FAIL;  // invalid index
  if (sigindex2 < 1 || sigindex2 > pubkeys) return SIG_FAIL;  // invalid index
  if (sigindex3 < 1 || sigindex3 > pubkeys) {
    return SIG_FAIL;  // invalid index
  }
  if (use_verifymessage == sectrue) {
    if (sigindex4 < 1 || sigindex4 > pubkeys) {
      return SIG_FAIL;  // invalid index
    }
  }

  if (sigindex1 == sigindex2) return SIG_FAIL;  // duplicate use
  if (sigindex1 == sigindex3) return SIG_FAIL;  // duplicate use
  if (sigindex2 == sigindex3) return SIG_FAIL;  // duplicate use

  if (use_verifymessage == sectrue) {
    if (sigindex4 == sigindex1) return SIG_FAIL;  // duplicate use
    if (sigindex4 == sigindex2) return SIG_FAIL;  // duplicate use
    if (sigindex4 == sigindex3) return SIG_FAIL;  // duplicate use
  }

  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_ptr[sigindex1 - 1], hdr->sig1,
                               hash)) {  // failure
    return SIG_FAIL;
  }

  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_ptr[sigindex2 - 1], hdr->sig2,
                               hash)) {  // failure
    return SIG_FAIL;
  }

  if (0 != ecdsa_verify_digest(&secp256k1, pubkey_ptr[sigindex3 - 1], hdr->sig3,
                               hash)) {  // failure
    return SIG_FAIL;
  }

  if (use_verifymessage == sectrue) {
    if (0 != ecdsa_verify_digest(&secp256k1, pubkey_ptr[sigindex4 - 1],
                                 hdr->signatures_4.sig4, hash)) {  // failure
      return SIG_FAIL;
    }
  }

  return SIG_OK;
}

int signatures_match(const image_header *hdr, uint8_t store_fingerprint[32]) {
  int result = 0;
  // Return success if v3 ("verify message") or the v2 ("new") style matches.
  // Use XOR to always force computing both signatures to avoid potential
  // timing side channels.
  // Return only the hash for the v2 computation so that it is
  // the same shown in previous bootloader.
  result ^= signatures_ok(hdr, store_fingerprint, secfalse);
  result ^= signatures_ok(hdr, NULL, sectrue);
  if (result != SIG_OK) {
    return SIG_FAIL;
  }
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
