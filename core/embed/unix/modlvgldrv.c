#include <stdbool.h>
#include <stdint.h>
#include "../../vendor/lvgl_mp/driver/include/common.h"
#include "lv_conf.h"
#include "lvgl.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "display.h"
#include "touch.h"

lv_color_t *fb[2] = {NULL, NULL};

static mp_obj_t mp_disp_drv_framebuffer(mp_obj_t n_obj) {
  int n = mp_obj_get_int(n_obj) - 1;

  if (n < 0 || n > 1) {
    return mp_const_none;
  }

  if (fb[n] == NULL) {
    // allocation on extRAM with 1KB alignment to speed up LTDC burst access on
    // AHB
    static lv_color_t lv_disp_buf_1[480 * 800];
    fb[n] = MP_STATE_PORT(disp_drv_fb[n]) = lv_disp_buf_1;
    // fb[n] = (lv_color_t*)((uint32_t)fb[n]);
  }
  return mp_obj_new_bytearray_by_ref(sizeof(lv_color_t) * 480 * 800,
                                     (void *)fb[n]);
}

static void mp_disp_drv_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                              lv_color_t *color_p) {
  display_buffer(area->x1, area->y1, area->x2 - area->x1, area->y2 - area->y1,
                 (uint16_t *)color_p);
  display_refresh();
  /*IMPORTANT!!!
   *Inform the graphics library that you are ready with the flushing*/
  lv_disp_flush_ready(disp_drv);
}

static bool mp_ts_read(struct _lv_indev_drv_t *indev_drv,
                       lv_indev_data_t *data) {
  static lv_coord_t last_x = 0;
  static lv_coord_t last_y = 0;

  int pos;
  pos = touch_read();
  /*Save the pressed coordinates and the state*/
  if (pos & TOUCH_START) {
    last_x = (pos >> 12) & 0xfff;
    last_y = pos & 0xfff;
    data->state = LV_INDEV_STATE_PR;
  } else if (pos & TOUCH_END) {
    last_x = (pos >> 12) & 0xfff;
    last_y = pos & 0xfff;
    data->state = LV_INDEV_STATE_REL;
  } else if (pos & TOUCH_MOVE) {
    last_x = (pos >> 12) & 0xfff;
    last_y = pos & 0xfff;
    data->state = LV_INDEV_STATE_PR;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }

  /*Set the last pressed coordinates*/
  data->point.x = last_x;
  data->point.y = last_y;
  return false;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_disp_drv_framebuffer_obj,
                                 mp_disp_drv_framebuffer);
DEFINE_PTR_OBJ(mp_disp_drv_flush);
DEFINE_PTR_OBJ(mp_ts_read);

STATIC const mp_rom_map_elem_t disp_indev_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_lvgldispdrv)},
    {MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&PTR_OBJ(mp_disp_drv_flush))},
    {MP_ROM_QSTR(MP_QSTR_framebuffer),
     MP_ROM_PTR(&PTR_OBJ(mp_disp_drv_framebuffer))},
    {MP_ROM_QSTR(MP_QSTR_ts_read), MP_ROM_PTR(&PTR_OBJ(mp_ts_read))},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_disp_drv_globals,
                            disp_indev_globals_table);

const mp_obj_module_t mp_module_lvgldispdrv = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mp_module_disp_drv_globals};

MP_REGISTER_MODULE(MP_QSTR_lvgldispdrv, mp_module_lvgldispdrv, 1);
