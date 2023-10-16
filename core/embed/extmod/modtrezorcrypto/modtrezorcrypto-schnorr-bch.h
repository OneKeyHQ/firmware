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

#include "schnorr_bch.h"
#include "vendor/trezor-crypto/secp256k1.h"

/// package: trezorcrypto.schnorr_bch

/// def sign(
///     secret_key: bytes,
///     digest: bytes,
/// ) -> bytes:
///     """
///     Uses secret key to produce the signature of the digest.
///     """
STATIC mp_obj_t mod_trezorcrypto_schnorr_bch_sign(mp_obj_t secret_key,
                                                  mp_obj_t digest) {
  mp_buffer_info_t sk = {0}, dig = {0};
  mp_get_buffer_raise(secret_key, &sk, MP_BUFFER_READ);
  mp_get_buffer_raise(digest, &dig, MP_BUFFER_READ);
  if (sk.len != 32) {
    mp_raise_ValueError("Invalid length of secret key");
  }
  if (dig.len != 32) {
    mp_raise_ValueError("Invalid length of digest");
  }

  vstr_t sig = {0};
  vstr_init_len(&sig, 64);
  int ret = schnorr_sign_digest(&secp256k1, (const uint8_t *)sk.buf,
                                (const uint8_t *)dig.buf, (uint8_t *)sig.buf);
  if (0 != ret) {
    vstr_clear(&sig);
    mp_raise_ValueError("Signing failed");
  }
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &sig);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorcrypto_schnorr_bch_sign_obj,
                                 mod_trezorcrypto_schnorr_bch_sign);

/// def verify(public_key: bytes, signature: bytes, digest: bytes) -> bool:
///     """
///     Uses public key to verify the signature of the digest.
///     Returns True on success.
///     """
STATIC mp_obj_t mod_trezorcrypto_schnorr_bch_verify(mp_obj_t public_key,
                                                    mp_obj_t signature,
                                                    mp_obj_t digest) {
  mp_buffer_info_t pk = {0}, sig = {0}, dig = {0};
  mp_get_buffer_raise(public_key, &pk, MP_BUFFER_READ);
  mp_get_buffer_raise(signature, &sig, MP_BUFFER_READ);
  mp_get_buffer_raise(digest, &dig, MP_BUFFER_READ);
  if (pk.len != 33) {
    return mp_const_false;
  }
  if (sig.len != 64) {
    return mp_const_false;
  }
  if (dig.len != 32) {
    return mp_const_false;
  }
  int ret =
      schnorr_verify_digest(&secp256k1, (const uint8_t *)pk.buf,
                            (const uint8_t *)sig.buf, (const uint8_t *)dig.buf);
  return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_trezorcrypto_schnorr_bch_verify_obj,
                                 mod_trezorcrypto_schnorr_bch_verify);

STATIC const mp_rom_map_elem_t mod_trezorcrypto_schnorr_bch_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_schnorr_bch)},
    {MP_ROM_QSTR(MP_QSTR_sign),
     MP_ROM_PTR(&mod_trezorcrypto_schnorr_bch_sign_obj)},
    {MP_ROM_QSTR(MP_QSTR_verify),
     MP_ROM_PTR(&mod_trezorcrypto_schnorr_bch_verify_obj)}};
STATIC MP_DEFINE_CONST_DICT(mod_trezorcrypto_schnorr_bch_globals,
                            mod_trezorcrypto_schnorr_bch_globals_table);

STATIC const mp_obj_module_t mod_trezorcrypto_schnorr_bch_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mod_trezorcrypto_schnorr_bch_globals,
};
