#include <stdbool.h>
#include <stdint.h>
#include "lv_conf.h"
#include "lvgl.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "lv_jpeg_stm32.h"

static mp_obj_t mp_st_jpeg_int(void) {
  lv_st_jpeg_init();
  return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_st_jpeg_int_obj, mp_st_jpeg_int);

STATIC const mp_rom_map_elem_t st_jpeg_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_stjpeg)},
    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_st_jpeg_int_obj)},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_st_jpeg_globals, st_jpeg_globals_table);

const mp_obj_module_t mp_module_stjpeg = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mp_module_st_jpeg_globals};

MP_REGISTER_MODULE(MP_QSTR_stjpeg, mp_module_stjpeg, 1);
