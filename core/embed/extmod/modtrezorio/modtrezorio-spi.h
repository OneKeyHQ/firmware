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

#include "embed/extmod/trezorobj.h"
#include "spi.h"

/// package: trezorio.__init__

/// class SPI:
///     """
///     """
typedef struct _mp_obj_SPI_t {
  mp_obj_base_t base;
  mp_int_t iface_num;
} mp_obj_SPI_t;

/// def __init__(self) -> None:
///     """
///     """
STATIC mp_obj_t mod_trezorio_SPI_make_new(const mp_obj_type_t *type,
                                          size_t n_args, size_t n_kw,
                                          const mp_obj_t *args) {
  mp_arg_check_num(n_args, n_kw, 1, 1, false);

  const mp_int_t iface_num = mp_obj_get_int(args[0]);

  mp_obj_SPI_t *o = m_new_obj(mp_obj_SPI_t);
  o->base.type = type;

  o->iface_num = iface_num;

  return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t mod_trezorio_SPI_iface_num(mp_obj_t self) {
  mp_obj_SPI_t *o = MP_OBJ_TO_PTR(self);
  return MP_OBJ_NEW_SMALL_INT(o->iface_num);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorio_SPI_iface_num_obj,
                                 mod_trezorio_SPI_iface_num);

/// def write(self, msg: bytes) -> int:
///     """
///     Sends message using SPI.
///     """
STATIC mp_obj_t mod_trezorio_SPI_write(mp_obj_t self, mp_obj_t msg) {
  mp_buffer_info_t buf = {0};
  mp_get_buffer_raise(msg, &buf, MP_BUFFER_READ);
  ssize_t r = spi_slave_send(buf.buf, buf.len, 100);
  return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorio_SPI_write_obj,
                                 mod_trezorio_SPI_write);

STATIC const mp_rom_map_elem_t mod_trezorio_SPI_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_iface_num),
     MP_ROM_PTR(&mod_trezorio_SPI_iface_num_obj)},
    {MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mod_trezorio_SPI_write_obj)},
};

STATIC MP_DEFINE_CONST_DICT(mod_trezorio_SPI_locals_dict,
                            mod_trezorio_SPI_locals_dict_table);

STATIC const mp_obj_type_t mod_trezorio_SPI_type = {
    {&mp_type_type},
    .name = MP_QSTR_SPI,
    .make_new = mod_trezorio_SPI_make_new,
    .locals_dict = (void *)&mod_trezorio_SPI_locals_dict,
};
