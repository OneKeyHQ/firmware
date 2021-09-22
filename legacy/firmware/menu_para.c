#include "menu_para.h"
#include "ble.h"
#include "config.h"
#include "gettext.h"
#include "protect.h"
#include "util.h"

extern uint8_t ui_language;

static char* format_time(uint32_t ms) {
  static char line[sizeof("4294967296 minutes?")] = {0};

  const char* unit = _("second");

  if (ms == 0) {
    return _("Never");
  }
  uint32_t num = ms / 1000U;

  if (ms >= 60 * 60 * 1000) {
    unit = _("hour");
    num /= 60 * 60U;
  } else if (ms >= 60 * 1000) {
    unit = _("minute");
    num /= 60U;
  }

  uint2str(num, line);
  strlcat(line, " ", sizeof(line));
  strlcat(line, unit, sizeof(line));
  if (num > 1 && ui_language == 0) {
    strlcat(line, "s", sizeof(line));
  }
  return line;
}

char* menu_para_ble_state(void) {
  return ble_get_switch() ? _(" On") : _(" Off");
}

char* menu_para_language(void) {
  return ui_language ? _("English") : "English";
}

char* menu_para_shutdown(void) {
  return format_time(config_getAutoLockDelayMs());
}

char* menu_para_autolock(void) { return format_time(config_getSleepDelayMs()); }

#if ONEKEY_MINI
char* menu_para_brightness(void) {
  uint32_t value = config_getBrightness();
  switch (value) {
    case brightnessLow:
      return _("Low");
    case brightnessMedium:
      return _("Medium");
    case brightnessHigh:
      return _("High");
    default:
      return _("Medium");
  }
}
#endif

char* menu_para_eth_eip_switch(void) {
  return config_getCoinSwitch(COIN_SWITCH_ETH_EIP712) ? _(" On") : _(" Off");
};

char* menu_para_sol_switch(void) {
  return config_getCoinSwitch(COIN_SWITCH_SOLANA) ? _(" On") : _(" Off");
};

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
  uint32_t ms[5] = {10 * 60 * 1000, 30 * 60 * 1000, 60 * 60 * 1000,
                    2 * 60 * 60 * 1000, 0};
  config_setAutoLockDelayMs(ms[index]);
}

void menu_para_set_sleep(int index) {
  uint32_t ms[5] = {60 * 1000, 2 * 60 * 1000, 5 * 60 * 1000, 10 * 60 * 1000, 0};
  config_setSleepDelayMs(ms[index]);
}

#if ONEKEY_MINI
void menu_para_set_brightness(int index) {
  uint32_t value[3] = {brightnessHigh, brightnessMedium, brightnessLow};
  config_setBrightness(value[index]);
}
#endif

void menu_para_set_eth_eip(int index) {
  bool state = index ? false : true;
  if (config_hasPin()) {
    if (!protectPinOnDevice(false, true)) {
      return;
    }
  }

  config_setCoinSwitch(COIN_SWITCH_ETH_EIP712, state);
}
