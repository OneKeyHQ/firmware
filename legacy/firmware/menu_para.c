#include "menu_para.h"
#include "ble.h"
#include "config.h"
#include "gettext.h"
#include "util.h"

extern uint8_t ui_language;

char* menu_para_ble_state(void) {
  return ble_get_switch() ? _("On") : _("Off");
}

char* menu_para_language(void) {
  return ui_language ? _("English") : "English";
}

char* menu_para_shutdown(void) {
  uint32_t seconds = 0;
  static char seconds_str[16] = "";

  seconds = config_getAutoLockDelayMs() / 1000;
  uint2str(seconds, seconds_str);
  strcat(seconds_str, _("s"));
  return seconds_str;
}

void menu_para_set_ble(int index) {
  bool ble_state = index ? false : true;
  if (ble_state != ble_get_switch()) {
    change_ble_sta(ble_state);
  }
}

void menu_para_set_language(int index) {
  const char* lang[2] = {"en-US", "zh-CN"};
  if (ui_language != index) config_setLanguage(lang[index]);
}

void menu_para_set_shutdown(int index) {
  uint32_t ms[3] = {60 * 1000, 300 * 1000, 600 * 100};
  config_setAutoLockDelayMs(ms[index]);
}
