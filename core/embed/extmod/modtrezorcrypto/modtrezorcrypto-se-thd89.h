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
#include "py/runtime.h"

#include "se_thd89.h"

/// package: trezorcrypto.se_thd89

/// def check(mnemonic: str) -> bool:
///     """
///     Check whether given mnemonic is valid.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_check(mp_obj_t mnemonic) {
  mp_buffer_info_t text = {0};
  mp_get_buffer_raise(mnemonic, &text, MP_BUFFER_READ);
  return (text.len > 0 && se_containsMnemonic(text.buf)) ? mp_const_true
                                                         : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorcrypto_se_thd89_check_obj,
                                 mod_trezorcrypto_se_thd89_check);

/// def seed(
///     passphrase: str,
///     callback: Callable[[int, int], None] | None = None,
/// ) -> bool:
///     """
///     Generate seed from mnemonic and passphrase.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_seed(size_t n_args,
                                               const mp_obj_t *args) {
  mp_buffer_info_t phrase = {0};
  mp_get_buffer_raise(args[0], &phrase, MP_BUFFER_READ);
  const char *ppassphrase = phrase.len > 0 ? phrase.buf : "";
  if (n_args > 1) {
    // generate with a progress callback
    ui_wait_callback = args[1];
    // se_set_ui_callback(ui_wait_callback);
    ui_wait_callback = mp_const_none;
  } else {
    // generate without callback
    // se_set_ui_callback(NULL);
  }

  if (!se_gen_session_seed(ppassphrase, false)) {
    return mp_const_false;
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorcrypto_se_thd89_seed_obj,
                                           1, 2,
                                           mod_trezorcrypto_se_thd89_seed);

/// def cardano_seed(
///     passphrase: str,
///     callback: Callable[[int, int], None] | None = None,
/// ) -> bool:
///     """
///     Generate seed from mnemonic and passphrase.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_cardano_seed(size_t n_args,
                                                       const mp_obj_t *args) {
  mp_buffer_info_t phrase = {0};
  mp_get_buffer_raise(args[0], &phrase, MP_BUFFER_READ);
  const char *ppassphrase = phrase.len > 0 ? phrase.buf : "";
  if (n_args > 1) {
    // generate with a progress callback
    ui_wait_callback = args[1];
    // se_set_ui_callback(ui_wait_callback);
    ui_wait_callback = mp_const_none;
  } else {
    // generate without callback
    // se_set_ui_callback(NULL);
  }

  if (!se_gen_session_seed(ppassphrase, true)) {
    return mp_const_false;
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(
    mod_trezorcrypto_se_thd89_cardano_seed_obj, 1, 2,
    mod_trezorcrypto_se_thd89_cardano_seed);

/// def start_session(
///     session_id: bytes,
/// ) -> bytes:
///     """
///     start session.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_start_session(mp_obj_t session_id) {
  mp_buffer_info_t sid = {0};
  sid.buf = NULL;
  if (session_id != mp_const_none) {
    mp_get_buffer_raise(session_id, &sid, MP_BUFFER_READ);
    if (sid.len != 32) {
      mp_raise_ValueError("session_id must be 32 bytes");
    }
  }

  uint8_t *id = se_session_startSession(sid.buf);
  return mp_obj_new_bytes(id, 32);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorcrypto_se_thd89_start_session_obj,
                                 mod_trezorcrypto_se_thd89_start_session);

/// def end_session() -> None:
///     """
///     end current session.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_end_session(void) {
  se_sessionClose();
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorcrypto_se_thd89_end_session_obj,
                                 mod_trezorcrypto_se_thd89_end_session);

/// def get_session_state() -> bytes:
///     """
///     get current session secret state.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_get_session_state(void) {
  uint8_t status[2] = {0};
  if (!se_get_session_seed_state(status)) {
    mp_raise_msg(&mp_type_RuntimeError, "Failed to get se state.");
  }
  return mp_obj_new_bytes(status, 2);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(
    mod_trezorcrypto_se_thd89_get_session_state_obj,
    mod_trezorcrypto_se_thd89_get_session_state);

/// def session_is_open() -> bool:
///     """
///     get current session secret state.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_session_is_open(void) {
  if (!se_session_is_open()) {
    return mp_const_false;
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorcrypto_se_thd89_session_is_open_obj,
                                 mod_trezorcrypto_se_thd89_session_is_open);

/// def nist256p1_sign(
///     secret_key: bytes, digest: bytes, compressed: bool = True
/// ) -> bytes:
///     """
///     Uses secret key to produce the signature of the digest.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_nist256p1_sign(size_t n_args,
                                                         const mp_obj_t *args) {
  // mp_buffer_info_t sk = {0};
  mp_buffer_info_t dig = {0};
  // mp_get_buffer_raise(args[0], &sk, MP_BUFFER_READ);
  mp_get_buffer_raise(args[1], &dig, MP_BUFFER_READ);
  bool compressed = n_args < 3 || args[2] == mp_const_true;
  // if (sk.len != 32) {
  //   mp_raise_ValueError("Invalid length of secret key");
  // }
  if (dig.len != 32) {
    mp_raise_ValueError("Invalid length of digest");
  }
  vstr_t sig = {0};
  vstr_init_len(&sig, 65);
  uint8_t pby = 0;
  if (0 != se_nist256p1_sign_digest((const uint8_t *)dig.buf,
                                    (uint8_t *)sig.buf + 1, &pby)) {
    vstr_clear(&sig);
    mp_raise_ValueError("Signing failed");
  }
  sig.buf[0] = 27 + pby + compressed * 4;
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &sig);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(
    mod_trezorcrypto_se_thd89_nist256p1_sign_obj, 2, 3,
    mod_trezorcrypto_se_thd89_nist256p1_sign);

/// def secp256k1_sign_digest(
///     seckey: bytes,
///     digest: bytes,
///     compressed: bool = True,
///     canonical: int | None = None,
/// ) -> bytes:
///     """
///     Uses secret key to produce the signature of the digest.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_secp256k1_sign_digest(
    size_t n_args, const mp_obj_t *args) {
  // mp_buffer_info_t sk = {0};
  mp_buffer_info_t dig = {0};
  // mp_get_buffer_raise(args[0], &sk, MP_BUFFER_READ);
  mp_get_buffer_raise(args[1], &dig, MP_BUFFER_READ);
  bool compressed = (n_args < 3) || (args[2] == mp_const_true);
  uint8_t canonical_type = 0;
#if !BITCOIN_ONLY
  mp_int_t canonical = (n_args > 3) ? mp_obj_get_int(args[3]) : 0;
  switch (canonical) {
    case CANONICAL_SIG_ETHEREUM:
      canonical_type = 1;
      break;
    case CANONICAL_SIG_EOS:
      canonical_type = 2;
      break;
  }
#endif
  // if (sk.len != 32) {
  //   mp_raise_ValueError("Invalid length of secret key");
  // }
  if (dig.len != 32) {
    mp_raise_ValueError("Invalid length of digest");
  }
  vstr_t sig = {0};
  vstr_init_len(&sig, 65);
  uint8_t pby = 0;
  int ret = 0;
  ret = se_secp256k1_sign_digest(canonical_type, (const uint8_t *)dig.buf,
                                 (uint8_t *)sig.buf + 1, &pby);

  if (0 != ret) {
    vstr_clear(&sig);
    mp_raise_ValueError("Signing failed");
  }
  sig.buf[0] = 27 + pby + compressed * 4;
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &sig);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(
    mod_trezorcrypto_se_thd89_secp256k1_sign_digest_obj, 2, 4,
    mod_trezorcrypto_se_thd89_secp256k1_sign_digest);

/// def bip340_sign(
///     secret_key: bytes,
///     digest: bytes,
/// ) -> bytes:
///     """
///     Uses secret key to produce the signature of the digest.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_bip340_sign(mp_obj_t secret_key,
                                                      mp_obj_t digest) {
  // mp_buffer_info_t sk = {0};
  mp_buffer_info_t dig = {0};
  // mp_get_buffer_raise(secret_key, &sk, MP_BUFFER_READ);
  mp_get_buffer_raise(digest, &dig, MP_BUFFER_READ);
  // if (sk.len != 32) {
  //   mp_raise_ValueError("Invalid length of secret key");
  // }
  if (dig.len != 32) {
    mp_raise_ValueError("Invalid length of digest");
  }

  vstr_t sig = {0};
  vstr_init_len(&sig, 64);
  int ret = se_bip340_sign_digest((const uint8_t *)dig.buf, (uint8_t *)sig.buf);
  if (0 != ret) {
    vstr_clear(&sig);
    mp_raise_ValueError("Signing failed");
  }
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &sig);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorcrypto_se_thd89_bip340_sign_obj,
                                 mod_trezorcrypto_se_thd89_bip340_sign);

/// def ed25519_sign(secret_key: bytes, message: bytes, hasher: str = "") ->
/// bytes:
///     """
///     Uses secret key to produce the signature of message.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_ed25519_sign(size_t n_args,
                                                       const mp_obj_t *args) {
  mp_buffer_info_t sk = {0};
  mp_buffer_info_t msg = {0};
  // mp_get_buffer_raise(args[0], &sk, MP_BUFFER_READ);
  mp_get_buffer_raise(args[1], &msg, MP_BUFFER_READ);
  // if (sk.len != 32) {
  //   mp_raise_ValueError("Invalid length of secret key");
  // }
  if (msg.len == 0) {
    mp_raise_ValueError("Empty data to sign");
  }
  mp_buffer_info_t hash_func = {0};
  vstr_t sig = {0};
  vstr_init_len(&sig, sizeof(ed25519_signature));

  if (n_args == 3) {
    mp_get_buffer_raise(args[2], &hash_func, MP_BUFFER_READ);
    // if hash_func == 'keccak':
    if (memcmp(hash_func.buf, "keccak", sizeof("keccak")) == 0) {
      ed25519_sign_keccak(msg.buf, msg.len, *(const ed25519_secret_key *)sk.buf,
                          *(ed25519_signature *)sig.buf);
    } else {
      vstr_clear(&sig);
      mp_raise_ValueError("Unknown hash function");
    }
  } else {
    se_ed25519_sign(msg.buf, msg.len, (uint8_t *)sig.buf);
  }

  return mp_obj_new_str_from_vstr(&mp_type_bytes, &sig);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(
    mod_trezorcrypto_se_thd89_ed25519_sign_obj, 2, 3,
    mod_trezorcrypto_se_thd89_ed25519_sign);

/// def ecdh(curve: str, public_key: bytes) -> bytes:
///     """
///     Multiplies point defined by public_key with scalar defined by
///     secret_key. Useful for ECDH.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_ecdh(mp_obj_t curve,
                                               mp_obj_t public_key) {
  mp_buffer_info_t curveb = {0}, pk = {0};
  mp_get_buffer_raise(curve, &curveb, MP_BUFFER_READ);
  mp_get_buffer_raise(public_key, &pk, MP_BUFFER_READ);

  mp_get_buffer_raise(curve, &curveb, MP_BUFFER_READ);
  if (curveb.len == 0) {
    mp_raise_ValueError("Invalid curve name");
  }
  if (pk.len != 33 && pk.len != 65) {
    mp_raise_ValueError("Invalid length of public key");
  }
  const ecdsa_curve *ecdsa_curve_para;
  if (memcmp(curveb.buf, "secp256k1", sizeof("secp256k1")) == 0) {
    ecdsa_curve_para = &secp256k1;
  } else if (memcmp(curveb.buf, "nist256p1", sizeof("nist256p1")) == 0) {
    ecdsa_curve_para = &nist256p1;
  } else {
    mp_raise_ValueError("Invalid curve name");
  }
  uint8_t pubkey[65] = {0};
  if (pk.len == 33) {
    if (!ecdsa_uncompress_pubkey(ecdsa_curve_para, pk.buf, pubkey)) {
      mp_raise_ValueError("Invalid public key");
    }
  } else {
    memcpy(pubkey, pk.buf, 65);
  }
  vstr_t out = {0};
  vstr_init_len(&out, 65);
  if (0 != se_get_shared_key((const char *)curveb.buf, (const uint8_t *)pubkey,
                             (uint8_t *)out.buf)) {
    vstr_clear(&out);
    mp_raise_ValueError("Multiply failed");
  }
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &out);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorcrypto_se_thd89_ecdh_obj,
                                 mod_trezorcrypto_se_thd89_ecdh);

/// def uncompress_pubkey(pubkey: bytes) -> bytes:
///     """
///     Uncompress public.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_uncompress_pubkey(
    mp_obj_t curve, mp_obj_t public_key) {
  mp_buffer_info_t curveb = {0}, pk = {0};
  mp_get_buffer_raise(curve, &curveb, MP_BUFFER_READ);
  mp_get_buffer_raise(public_key, &pk, MP_BUFFER_READ);

  mp_get_buffer_raise(curve, &curveb, MP_BUFFER_READ);
  if (curveb.len == 0) {
    mp_raise_ValueError("Invalid curve name");
  }

  if (pk.len == 65) {
    return public_key;
  }

  if (pk.len != 33) {
    mp_raise_ValueError("Invalid length of public key");
  }

  const ecdsa_curve *ecdsa_curve_para;
  if (memcmp(curveb.buf, "secp256k1", sizeof("secp256k1")) == 0) {
    ecdsa_curve_para = &secp256k1;
  } else if (memcmp(curveb.buf, "nist256p1", sizeof("nist256p1")) == 0) {
    ecdsa_curve_para = &nist256p1;
  } else {
    mp_raise_ValueError("Invalid curve name");
  }

  vstr_t pub = {0};
  vstr_init_len(&pub, 65);

  if (pk.len == 33) {
    if (!ecdsa_uncompress_pubkey(ecdsa_curve_para, pk.buf,
                                 (uint8_t *)pub.buf)) {
      mp_raise_ValueError("Invalid public key");
    }
  }

  return mp_obj_new_str_from_vstr(&mp_type_bytes, &pub);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(
    mod_trezorcrypto_se_thd89_uncompress_pubkey_obj,
    mod_trezorcrypto_se_thd89_uncompress_pubkey);

/// def aes256_encrypt(data: bytes, value: bytes, iv: bytes | None) ->
/// bytes:
///     """
///     Uses secret key to produce the signature of message.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_aes256_encrypt(size_t n_args,
                                                         const mp_obj_t *args) {
  mp_buffer_info_t data = {0};
  mp_buffer_info_t value = {0};
  mp_get_buffer_raise(args[0], &data, MP_BUFFER_READ);
  mp_get_buffer_raise(args[1], &value, MP_BUFFER_READ);

  mp_buffer_info_t iv = {0};
  const uint8_t *piv = NULL;
  if (n_args == 3) {
    mp_get_buffer_raise(args[2], &iv, MP_BUFFER_READ);
    if (iv.len != 16) {
      mp_raise_ValueError("Invalid length of iv");
    }
    piv = (const uint8_t *)iv.buf;
  }

  vstr_t vstr = {0};
  vstr_init_len(&vstr, value.len);

  if (se_aes256_encrypt(data.buf, data.len, piv, value.buf, value.len,
                        (uint8_t *)vstr.buf) != 0) {
    mp_raise_ValueError("Encrypt failed");
  }

  return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(
    mod_trezorcrypto_se_thd89_aes256_encrypt_obj, 2, 3,
    mod_trezorcrypto_se_thd89_aes256_encrypt);

/// def aes256_decrypt(data: bytes, value: bytes, iv: bytes | None) ->
/// bytes:
///     """
///     Uses secret key to produce the signature of message.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_aes256_decrypt(size_t n_args,
                                                         const mp_obj_t *args) {
  mp_buffer_info_t data = {0};
  mp_buffer_info_t value = {0};
  mp_get_buffer_raise(args[0], &data, MP_BUFFER_READ);
  mp_get_buffer_raise(args[1], &value, MP_BUFFER_READ);

  mp_buffer_info_t iv = {0};
  const uint8_t *piv = NULL;
  if (n_args == 3) {
    mp_get_buffer_raise(args[2], &iv, MP_BUFFER_READ);
    if (iv.len != 16) {
      mp_raise_ValueError("Invalid length of iv");
    }
    piv = (const uint8_t *)iv.buf;
  }

  vstr_t vstr = {0};
  vstr_init_len(&vstr, value.len);

  if (se_aes256_decrypt(data.buf, data.len, piv, value.buf, value.len,
                        (uint8_t *)vstr.buf) != 0) {
    mp_raise_ValueError("Encrypt failed");
  }

  return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(
    mod_trezorcrypto_se_thd89_aes256_decrypt_obj, 2, 3,
    mod_trezorcrypto_se_thd89_aes256_decrypt);

/// def slip21_node() -> bytes:
///     """
///     Returns slip21 node.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_slip21_node(void) {
  vstr_t vstr = {0};
  vstr_init_len(&vstr, 64);
  if (se_slip21_node((uint8_t *)vstr.buf) != 0) {
    mp_raise_ValueError("slip21_node failed");
  }
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorcrypto_se_thd89_slip21_node_obj,
                                 mod_trezorcrypto_se_thd89_slip21_node);

/// def authorization_set(
///     authorization_type: int,
///     authorization: bytes,
/// ) -> bool:
///     """
///     authorization_set.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_authorization_set(
    mp_obj_t authorization_type, mp_obj_t authorization_data) {
  uint32_t auth_type = trezor_obj_get_uint(authorization_type);
  mp_buffer_info_t auth_data = {0};
  mp_get_buffer_raise(authorization_data, &auth_data, MP_BUFFER_READ);

  if (auth_data.len > MAX_AUTHORIZATION_LEN) {
    mp_raise_ValueError("Invalid length of authorization data");
  }

  if (sectrue !=
      se_authorization_set(auth_type, auth_data.buf, auth_data.len)) {
    mp_raise_ValueError("authorization_set failed");
  }
  return mp_const_true;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(
    mod_trezorcrypto_se_thd89_authorization_set_obj,
    mod_trezorcrypto_se_thd89_authorization_set);

/// def authorization_get_type(
/// ) -> int:
///     """
///     authorization_get.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_authorization_get_type(void) {
  uint32_t auth_type = 0;

  if (sectrue != se_authorization_get_type(&auth_type)) {
    return mp_const_none;
  }
  return mp_obj_new_int_from_uint(auth_type);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(
    mod_trezorcrypto_se_thd89_authorization_get_type_obj,
    mod_trezorcrypto_se_thd89_authorization_get_type);

/// def authorization_get_data(
/// ) -> bytes:
///     """
///     authorization_get.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_authorization_get_data(void) {
  uint32_t data_len = 0;

  vstr_t resp = {0};
  vstr_init_len(&resp, MAX_AUTHORIZATION_LEN);

  if (sectrue != se_authorization_get_data((uint8_t *)resp.buf, &data_len)) {
    mp_raise_ValueError("authorization_get failed");
  }
  resp.len = data_len;
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &resp);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(
    mod_trezorcrypto_se_thd89_authorization_get_data_obj,
    mod_trezorcrypto_se_thd89_authorization_get_data);

/// def authorization_clear(
/// ) -> None:
///     """
///     authorization clear.
///     """
STATIC mp_obj_t mod_trezorcrypto_se_thd89_authorization_clear(void) {
  se_authorization_clear();
  return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(
    mod_trezorcrypto_se_thd89_authorization_clear_obj,
    mod_trezorcrypto_se_thd89_authorization_clear);

STATIC const mp_rom_map_elem_t mod_trezorcrypto_se_thd89_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_se_thd89)},
    {MP_ROM_QSTR(MP_QSTR_check),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_check_obj)},
    {MP_ROM_QSTR(MP_QSTR_seed),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_seed_obj)},
    {MP_ROM_QSTR(MP_QSTR_cardano_seed),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_cardano_seed_obj)},
    {MP_ROM_QSTR(MP_QSTR_start_session),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_start_session_obj)},
    {MP_ROM_QSTR(MP_QSTR_end_session),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_end_session_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_session_state),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_get_session_state_obj)},
    {MP_ROM_QSTR(MP_QSTR_session_is_open),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_session_is_open_obj)},
    {MP_ROM_QSTR(MP_QSTR_nist256p1_sign),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_nist256p1_sign_obj)},
    {MP_ROM_QSTR(MP_QSTR_secp256k1_sign_digest),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_secp256k1_sign_digest_obj)},
    {MP_ROM_QSTR(MP_QSTR_bip340_sign),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_bip340_sign_obj)},
    {MP_ROM_QSTR(MP_QSTR_ed25519_sign),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_ed25519_sign_obj)},
    {MP_ROM_QSTR(MP_QSTR_ecdh),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_ecdh_obj)},
    {MP_ROM_QSTR(MP_QSTR_uncompress_pubkey),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_uncompress_pubkey_obj)},
    {MP_ROM_QSTR(MP_QSTR_aes256_encrypt),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_aes256_encrypt_obj)},
    {MP_ROM_QSTR(MP_QSTR_aes256_decrypt),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_aes256_decrypt_obj)},
    {MP_ROM_QSTR(MP_QSTR_slip21_node),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_slip21_node_obj)},
    {MP_ROM_QSTR(MP_QSTR_authorization_set),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_authorization_set_obj)},
    {MP_ROM_QSTR(MP_QSTR_authorization_get_type),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_authorization_get_type_obj)},
    {MP_ROM_QSTR(MP_QSTR_authorization_get_data),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_authorization_get_data_obj)},
    {MP_ROM_QSTR(MP_QSTR_authorization_clear),
     MP_ROM_PTR(&mod_trezorcrypto_se_thd89_authorization_clear_obj)},
};
STATIC MP_DEFINE_CONST_DICT(mod_trezorcrypto_se_thd89_globals,
                            mod_trezorcrypto_se_thd89_globals_table);

STATIC const mp_obj_module_t mod_trezorcrypto_se_thd89_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mod_trezorcrypto_se_thd89_globals,
};
