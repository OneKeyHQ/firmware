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

#include "ble.h"
#include "embed/extmod/trezorobj.h"

/// package: trezorio.__init__

/// class BLE:
///     """
///     """
typedef struct _mp_obj_BLE_t {
  mp_obj_base_t base;
} mp_obj_BLE_t;

/// def __init__(
///     self,
/// ) -> None:
///     """
///     """
STATIC mp_obj_t mod_trezorio_BLE_make_new(const mp_obj_type_t *type,
                                          size_t n_args, size_t n_kw,
                                          const mp_obj_t *args) {
  mp_arg_check_num(n_args, n_kw, 1, 1, false);

  mp_obj_BLE_t *o = m_new_obj(mp_obj_BLE_t);
  o->base.type = type;

  return MP_OBJ_FROM_PTR(o);
}

/// def ctrl(self, cmd: byte, value: byte) -> None:
///     """
///     Send command to the BLE.
///     """
STATIC mp_obj_t mod_trezorio_BLE_ctrl(mp_obj_t self, mp_obj_t cmd,
                                      mp_obj_t value) {
  ble_cmd_req(trezor_obj_get_uint8(cmd), trezor_obj_get_uint8(value));
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_trezorio_BLE_ctrl_obj,
                                 mod_trezorio_BLE_ctrl);

STATIC const mp_rom_map_elem_t mod_trezorio_BLE_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_ctrl), MP_ROM_PTR(&mod_trezorio_BLE_ctrl_obj)},
};

STATIC MP_DEFINE_CONST_DICT(mod_trezorio_BLE_locals_dict,
                            mod_trezorio_BLE_locals_dict_table);

STATIC const mp_obj_type_t mod_trezorio_BLE_type = {
    {&mp_type_type},
    .name = MP_QSTR_BLE,
    .make_new = mod_trezorio_BLE_make_new,
    .locals_dict = (void *)&mod_trezorio_BLE_locals_dict,
};
