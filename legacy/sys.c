#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/gpio.h>
#include <string.h>

#include "bitmaps.h"
#include "ble.h"
#include "oled.h"
#include "si2c.h"
#include "sys.h"
#include "timer.h"

uint8_t battery_cap = 0xFF;

bool sys_nfcState(void) {
  if (get_nfc_state() == 0) {
    return true;
  }
  return false;
}
bool sys_usbState(void) {
  if (get_usb_state()) {
    return true;
  }
  return false;
}
bool sys_bleState(void) { return ble_connect_state(); }

void sys_shutdown(void) {
  delay_ms(500);  // delay for prevois display
  oledClear();
  oledDrawStringCenter(64, 30, "power off ...", FONT_STANDARD);
  oledRefresh();
  delay_ms(500);
  oledClear();
  oledRefresh();
#if ONEKEY_MINI
#else
  ble_power_off();
  stm32_power_off();
  delay_ms(100);
#endif
  scb_reset_system();
}

void sys_poweron(void) {
  uint32_t count = 0;
  while (1) {
    if (get_power_key_state()) {
      delay_ms(100);
      count++;
      if (count > 5) {
        oledClear();
        oledDrawStringCenter(64, 30, "power on...", FONT_STANDARD);
        oledRefresh();
        while (get_power_key_state())
          ;
        break;
      }
    }
    if (sys_nfcState() || sys_usbState()) break;
  }
  stm32_power_on();
  ble_power_on();
}

extern uint8_t _ram_start[], _ram_end[];

void sys_backtoboot(void) {
  oledClear();
  oledDrawStringCenter(64, 30, "Back to boot", FONT_STANDARD);
  oledRefresh();
  memcpy((uint8_t *)(ST_RAM_END - 4), "boot", 4);
  svc_system_reset();
}
