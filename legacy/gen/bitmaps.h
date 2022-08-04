#ifndef __BITMAPS_H__
#define __BITMAPS_H__

#include <stdint.h>

typedef struct {
  uint8_t width, height;
  const uint8_t *data;
} BITMAP;

extern const BITMAP bmp_BiXin_logo32;
extern const BITMAP bmp_arrow_up;
extern const BITMAP bmp_boot_icon;
extern const BITMAP bmp_btn_back;
extern const BITMAP bmp_btn_cancel;
extern const BITMAP bmp_btn_confirm;
extern const BITMAP bmp_btn_down;
extern const BITMAP bmp_btn_forward;
extern const BITMAP bmp_btn_retry;
extern const BITMAP bmp_btn_switch;
extern const BITMAP bmp_btn_up;
extern const BITMAP bmp_button_back;
extern const BITMAP bmp_button_down;
extern const BITMAP bmp_button_forward;
extern const BITMAP bmp_button_up;
extern const BITMAP bmp_digit0;
extern const BITMAP bmp_digit1;
extern const BITMAP bmp_digit2;
extern const BITMAP bmp_digit3;
extern const BITMAP bmp_digit4;
extern const BITMAP bmp_digit5;
extern const BITMAP bmp_digit6;
extern const BITMAP bmp_digit7;
extern const BITMAP bmp_digit8;
extern const BITMAP bmp_digit9;
extern const BITMAP bmp_gears0;
extern const BITMAP bmp_gears1;
extern const BITMAP bmp_gears2;
extern const BITMAP bmp_gears3;
extern const BITMAP bmp_icon_down;
extern const BITMAP bmp_icon_enter;
extern const BITMAP bmp_icon_error;
extern const BITMAP bmp_icon_exit;
extern const BITMAP bmp_icon_info;
extern const BITMAP bmp_icon_ok;
extern const BITMAP bmp_icon_question;
extern const BITMAP bmp_icon_up;
extern const BITMAP bmp_icon_warning;
extern const BITMAP bmp_launch_icon;
extern const BITMAP bmp_logo48;
extern const BITMAP bmp_logo48_empty;
extern const BITMAP bmp_logo64_empty_half;
extern const BITMAP bmp_logo64_half;
extern const BITMAP bmp_webauthn;

#endif
