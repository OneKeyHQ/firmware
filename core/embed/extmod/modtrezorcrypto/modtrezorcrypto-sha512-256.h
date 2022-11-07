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

#include "embed/extmod/trezorobj.h"

#include "sha512.h"

/// package: trezorcrypto.sha512_256

/// def digest(data: bytes) -> bytes:
///     """
///     Returns the digest of hashed data.
///     """
mp_obj_t mod_trezorcrypto_sha512_256_digest(mp_obj_t data) {
  mp_buffer_info_t msg = {0};
  mp_get_buffer_raise(data, &msg, MP_BUFFER_READ);
  vstr_t hash = {0};
  vstr_init_len(&hash, SHA512_DIGEST_LENGTH);
  SHA512_256(msg.buf, msg.len, (uint8_t *)hash.buf);
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &hash);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorcrypto_sha512_256_digest_obj,
                                 mod_trezorcrypto_sha512_256_digest);

STATIC const mp_rom_map_elem_t mod_trezorcrypto_sha512_256_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sha512_256)},
    {MP_ROM_QSTR(MP_QSTR_digest),
     MP_ROM_PTR(&mod_trezorcrypto_sha512_256_digest_obj)},
};
STATIC MP_DEFINE_CONST_DICT(mod_trezorcrypto_sha512_256_globals,
                            mod_trezorcrypto_sha512_256_globals_table);

STATIC const mp_obj_module_t mod_trezorcrypto_sha512_256_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mod_trezorcrypto_sha512_256_globals,
};
