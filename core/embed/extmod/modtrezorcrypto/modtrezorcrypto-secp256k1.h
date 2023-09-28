/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "py/objstr.h"

#include "vendor/trezor-crypto/ecdsa.h"
#include "vendor/trezor-crypto/secp256k1.h"

#ifdef USE_SECP256K1_ZKP_ECDSA
#include "zkp_ecdsa.h"
#endif

#if USE_THD89
#include "se_thd89.h"
#endif

/// package: trezorcrypto.secp256k1

/// def generate_secret() -> bytes:
///     """
///     Generate secret key.
///     """
STATIC mp_obj_t mod_trezorcrypto_secp256k1_generate_secret() {
  vstr_t sk = {0};
  vstr_init_len(&sk, 32);
  for (;;) {
    random_buffer((uint8_t *)sk.buf, sk.len);
    // check whether secret > 0 && secret < curve_order
    if (0 ==
        memcmp(
            sk.buf,
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
            32))
      continue;
    if (0 <=
        memcmp(
            sk.buf,
            "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFE"
            "\xBA\xAE\xDC\xE6\xAF\x48\xA0\x3B\xBF\xD2\x5E\x8C\xD0\x36\x41\x41",
            32))
      continue;
    break;
  }
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &sk);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorcrypto_secp256k1_generate_secret_obj,
                                 mod_trezorcrypto_secp256k1_generate_secret);

/// def publickey(secret_key: bytes, compressed: bool = True) -> bytes:
///     """
///     Computes public key from secret key.
///     """
STATIC mp_obj_t mod_trezorcrypto_secp256k1_publickey(size_t n_args,
                                                     const mp_obj_t *args) {
  mp_buffer_info_t sk = {0};
  mp_get_buffer_raise(args[0], &sk, MP_BUFFER_READ);
  if (sk.len != 32) {
    mp_raise_ValueError("Invalid length of secret key");
  }
  vstr_t pk = {0};
  int ret = 0;
  bool compressed = n_args < 2 || args[1] == mp_const_true;
  if (compressed) {
    vstr_init_len(&pk, 33);
#ifdef USE_SECP256K1_ZKP_ECDSA
    ret = zkp_ecdsa_get_public_key33(&secp256k1, (const uint8_t *)sk.buf,
                                     (uint8_t *)pk.buf);
#else
    ret = ecdsa_get_public_key33(&secp256k1, (const uint8_t *)sk.buf,
                                 (uint8_t *)pk.buf);
#endif

  } else {
    vstr_init_len(&pk, 65);
#ifdef USE_SECP256K1_ZKP_ECDSA
    ret = zkp_ecdsa_get_public_key65(&secp256k1, (const uint8_t *)sk.buf,
                                     (uint8_t *)pk.buf);
#else
    ret = ecdsa_get_public_key65(&secp256k1, (const uint8_t *)sk.buf,
                                 (uint8_t *)pk.buf);
#endif
  }
  if (0 != ret) {
    vstr_clear(&pk);
    mp_raise_ValueError("Invalid secret key");
  }
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &pk);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(
    mod_trezorcrypto_secp256k1_publickey_obj, 1, 2,
    mod_trezorcrypto_secp256k1_publickey);

#if !BITCOIN_ONLY

static int ethereum_is_canonical(uint8_t v, uint8_t signature[64]) {
  (void)signature;
  return (v & 2) == 0;
}

static int eos_is_canonical(uint8_t v, uint8_t signature[64]) {
  (void)v;
  return !(signature[0] & 0x80) &&
         !(signature[0] == 0 && !(signature[1] & 0x80)) &&
         !(signature[32] & 0x80) &&
         !(signature[32] == 0 && !(signature[33] & 0x80));
}

/// mock:global

/// CANONICAL_SIG_ETHEREUM: int = 1
/// CANONICAL_SIG_EOS: int = 2
enum {
  CANONICAL_SIG_ETHEREUM = 1,
  CANONICAL_SIG_EOS = 2,
};

#endif

/// def sign(
///     secret_key: bytes,
///     digest: bytes,
///     compressed: bool = True,
///     canonical: int | None = None,
/// ) -> bytes:
///     """
///     Uses secret key to produce the signature of the digest.
///     """
STATIC mp_obj_t mod_trezorcrypto_secp256k1_sign(size_t n_args,
                                                const mp_obj_t *args) {
  mp_buffer_info_t sk = {0};
  mp_buffer_info_t dig = {0};
  mp_get_buffer_raise(args[0], &sk, MP_BUFFER_READ);
  mp_get_buffer_raise(args[1], &dig, MP_BUFFER_READ);
  bool compressed = (n_args < 3) || (args[2] == mp_const_true);
  int (*is_canonical)(uint8_t by, uint8_t sig[64]) = NULL;
  uint8_t canonical_type = 0;
#if !BITCOIN_ONLY
  mp_int_t canonical = (n_args > 3) ? mp_obj_get_int(args[3]) : 0;
  switch (canonical) {
    case CANONICAL_SIG_ETHEREUM:
      is_canonical = ethereum_is_canonical;
      canonical_type = 1;
      break;
    case CANONICAL_SIG_EOS:
      is_canonical = eos_is_canonical;
      canonical_type = 2;
      break;
  }
#endif
  if (sk.len != 32) {
    mp_raise_ValueError("Invalid length of secret key");
  }
  if (dig.len != 32) {
    mp_raise_ValueError("Invalid length of digest");
  }
  vstr_t sig = {0};
  vstr_init_len(&sig, 65);
  uint8_t pby = 0;
  int ret = 0;

#if USE_THD89
  (void)is_canonical;
  ret = se_secp256k1_sign_digest(canonical_type, (const uint8_t *)dig.buf,
                                 (uint8_t *)sig.buf + 1, &pby);
#else
  (void)canonical_type;
#ifdef USE_SECP256K1_ZKP_ECDSA
  if (!is_canonical) {
    ret = zkp_ecdsa_sign_digest(&secp256k1, (const uint8_t *)sk.buf,
                                (const uint8_t *)dig.buf,
                                (uint8_t *)sig.buf + 1, &pby, is_canonical);
  } else
#endif
  {
    ret = ecdsa_sign_digest(&secp256k1, (const uint8_t *)sk.buf,
                            (const uint8_t *)dig.buf, (uint8_t *)sig.buf + 1,
                            &pby, is_canonical);
  }
#endif
  if (0 != ret) {
    vstr_clear(&sig);
    mp_raise_ValueError("Signing failed");
  }

  sig.buf[0] = 27 + pby + compressed * 4;
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &sig);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorcrypto_secp256k1_sign_obj,
                                           2, 4,
                                           mod_trezorcrypto_secp256k1_sign);

/// def verify(public_key: bytes, signature: bytes, digest: bytes) -> bool:
///     """
///     Uses public key to verify the signature of the digest.
///     Returns True on success.
///     """
STATIC mp_obj_t mod_trezorcrypto_secp256k1_verify(mp_obj_t public_key,
                                                  mp_obj_t signature,
                                                  mp_obj_t digest) {
  mp_buffer_info_t pk = {0}, sig = {0}, dig = {0};
  mp_get_buffer_raise(public_key, &pk, MP_BUFFER_READ);
  mp_get_buffer_raise(signature, &sig, MP_BUFFER_READ);
  mp_get_buffer_raise(digest, &dig, MP_BUFFER_READ);
  if (pk.len != 33 && pk.len != 65) {
    return mp_const_false;
  }
  if (sig.len != 64 && sig.len != 65) {
    return mp_const_false;
  }
  int offset = sig.len - 64;
  if (dig.len != 32) {
    return mp_const_false;
  }
  int ret = 0;
#ifdef USE_SECP256K1_ZKP_ECDSA
  ret = zkp_ecdsa_verify_digest(&secp256k1, (const uint8_t *)pk.buf,
                                (const uint8_t *)sig.buf + offset,
                                (const uint8_t *)dig.buf);
#else
  ret = ecdsa_verify_digest(&secp256k1, (const uint8_t *)pk.buf,
                            (const uint8_t *)sig.buf + offset,
                            (const uint8_t *)dig.buf);
#endif
  return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_trezorcrypto_secp256k1_verify_obj,
                                 mod_trezorcrypto_secp256k1_verify);

/// def verify_recover(signature: bytes, digest: bytes) -> bytes:
///     """
///     Uses signature of the digest to verify the digest and recover the public
///     key. Returns public key on success, None if the signature is invalid.
///     """
STATIC mp_obj_t mod_trezorcrypto_secp256k1_verify_recover(mp_obj_t signature,
                                                          mp_obj_t digest) {
  mp_buffer_info_t sig = {0}, dig = {0};
  mp_get_buffer_raise(signature, &sig, MP_BUFFER_READ);
  mp_get_buffer_raise(digest, &dig, MP_BUFFER_READ);
  if (sig.len != 65) {
    return mp_const_none;
  }
  if (dig.len != 32) {
    return mp_const_none;
  }
  uint8_t recid = ((const uint8_t *)sig.buf)[0] - 27;
  if (recid >= 8) {
    return mp_const_none;
  }
  bool compressed = (recid >= 4);
  recid &= 3;
  vstr_t pk = {0};
  vstr_init_len(&pk, 65);
  int ret = 0;
#ifdef USE_SECP256K1_ZKP_ECDSA
  ret = zkp_ecdsa_recover_pub_from_sig(&secp256k1, (uint8_t *)pk.buf,
                                       (const uint8_t *)sig.buf + 1,
                                       (const uint8_t *)dig.buf, recid);
#else
  ret = ecdsa_recover_pub_from_sig(&secp256k1, (uint8_t *)pk.buf,
                                   (const uint8_t *)sig.buf + 1,
                                   (const uint8_t *)dig.buf, recid);
#endif
  if (ret == 0) {
    if (compressed) {
      pk.buf[0] = 0x02 | (pk.buf[64] & 1);
      pk.len = 33;
    }
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &pk);
  } else {
    return mp_const_none;
  }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorcrypto_secp256k1_verify_recover_obj,
                                 mod_trezorcrypto_secp256k1_verify_recover);

/// def multiply(secret_key: bytes, public_key: bytes) -> bytes:
///     """
///     Multiplies point defined by public_key with scalar defined by
///     secret_key. Useful for ECDH.
///     """
STATIC mp_obj_t mod_trezorcrypto_secp256k1_multiply(mp_obj_t secret_key,
                                                    mp_obj_t public_key) {
  mp_buffer_info_t sk = {0}, pk = {0};
  mp_get_buffer_raise(secret_key, &sk, MP_BUFFER_READ);
  mp_get_buffer_raise(public_key, &pk, MP_BUFFER_READ);
  if (sk.len != 32) {
    mp_raise_ValueError("Invalid length of secret key");
  }
  if (pk.len != 33 && pk.len != 65) {
    mp_raise_ValueError("Invalid length of public key");
  }
  vstr_t out = {0};
  vstr_init_len(&out, 65);
#if USE_THD89
  uint8_t pubkey[65] = {0};
  if (pk.len == 33) {
    if (!ecdsa_uncompress_pubkey(&secp256k1, pk.buf, pubkey)) {
      mp_raise_ValueError("Invalid public key");
    }
  } else {
    memcpy(pubkey, pk.buf, 65);
  }
  if (0 != se_get_shared_key(SECP256K1_NAME, (const uint8_t *)pubkey,
                             (uint8_t *)out.buf)) {
    vstr_clear(&out);
    mp_raise_ValueError("Multiply failed");
  }
#else
  if (0 != ecdh_multiply(&secp256k1, (const uint8_t *)sk.buf,
                         (const uint8_t *)pk.buf, (uint8_t *)out.buf)) {
    vstr_clear(&out);
    mp_raise_ValueError("Multiply failed");
  }
#endif
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &out);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorcrypto_secp256k1_multiply_obj,
                                 mod_trezorcrypto_secp256k1_multiply);

STATIC const mp_rom_map_elem_t mod_trezorcrypto_secp256k1_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_secp256k1)},
    {MP_ROM_QSTR(MP_QSTR_generate_secret),
     MP_ROM_PTR(&mod_trezorcrypto_secp256k1_generate_secret_obj)},
    {MP_ROM_QSTR(MP_QSTR_publickey),
     MP_ROM_PTR(&mod_trezorcrypto_secp256k1_publickey_obj)},
    {MP_ROM_QSTR(MP_QSTR_sign),
     MP_ROM_PTR(&mod_trezorcrypto_secp256k1_sign_obj)},
    {MP_ROM_QSTR(MP_QSTR_verify),
     MP_ROM_PTR(&mod_trezorcrypto_secp256k1_verify_obj)},
    {MP_ROM_QSTR(MP_QSTR_verify_recover),
     MP_ROM_PTR(&mod_trezorcrypto_secp256k1_verify_recover_obj)},
    {MP_ROM_QSTR(MP_QSTR_multiply),
     MP_ROM_PTR(&mod_trezorcrypto_secp256k1_multiply_obj)},
#if !BITCOIN_ONLY
    {MP_ROM_QSTR(MP_QSTR_CANONICAL_SIG_ETHEREUM),
     MP_ROM_INT(CANONICAL_SIG_ETHEREUM)},
    {MP_ROM_QSTR(MP_QSTR_CANONICAL_SIG_EOS), MP_ROM_INT(CANONICAL_SIG_EOS)},
#endif
};
STATIC MP_DEFINE_CONST_DICT(mod_trezorcrypto_secp256k1_globals,
                            mod_trezorcrypto_secp256k1_globals_table);

STATIC const mp_obj_module_t mod_trezorcrypto_secp256k1_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mod_trezorcrypto_secp256k1_globals,
};
