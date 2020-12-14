#include "menu_list.h"
#include "menu_core.h"
#include "menu_para.h"

#include "buttons.h"
#include "config.h"
#include "gettext.h"
#include "layout2.h"
#include "protect.h"
#include "recovery.h"
#include "reset.h"

static struct menu settings_menu, main_menu;

void menu_recovery_device(int index) {
  (void)index;
  if (protectPinOnDevice(true)) {
    if (recovery_on_device()) {
      protectChangePinOnDevice();
    }
  }
}

void menu_reset_device(int index) {
  (void)index;
  if (protectPinOnDevice(true)) {
    if (reset_on_device()) {
      protectChangePinOnDevice();
    }
  }
}

void menu_showQRCode(int index) {
  (void)index;
  layoutQRCode("https://onekey.zendesk.com/hc/zh-cn/articles/360002123856");
  waitKey(timer1s * 30, 0);
}

void menu_erase_device(int index) {
  (void)index;
  uint8_t key = KEY_NULL;
  if (protectPinOnDevice(false)) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("wipe the device?"), NULL,
                      _("All data will be lost."), NULL, NULL);

    key = waitKey(timer1s * 60, 1);
    if (key == KEY_CONFIRM) config_wipe();
    layoutHome();
  }
}

static struct menu_item ble_set_menu_items[] = {
    {"On", NULL, true, menu_para_set_ble, NULL},
    {"Off", NULL, true, menu_para_set_ble, NULL}};

static struct menu ble_set_menu = {
    .start = (COUNT_OF(ble_set_menu_items) - 1) / 2,
    .current = (COUNT_OF(ble_set_menu_items) - 1) / 2,
    .counts = COUNT_OF(ble_set_menu_items),
    .title = "Bluetooth",
    .items = ble_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item language_set_menu_items[] = {
    {"English ", NULL, true, menu_para_set_language, NULL},
    {"中文", NULL, true, menu_para_set_language, NULL}};

static struct menu language_set_menu = {
    .start = (COUNT_OF(language_set_menu_items) - 1) / 2,
    .current = (COUNT_OF(language_set_menu_items) - 1) / 2,
    .counts = COUNT_OF(language_set_menu_items),
    .title = "Language",
    .items = language_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item shutdown_set_menu_items[] = {
    {"60", "s", true, menu_para_set_shutdown, NULL},
    {"300", "s", true, menu_para_set_shutdown, NULL},
    {"600", "s", true, menu_para_set_shutdown, NULL}};

static struct menu shutdown_set_menu = {
    .start = (COUNT_OF(shutdown_set_menu_items) - 1) / 2,
    .current = (COUNT_OF(shutdown_set_menu_items) - 1) / 2,
    .counts = COUNT_OF(shutdown_set_menu_items),
    .title = "Shutdown",
    .items = shutdown_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item settings_menu_items[] = {
    {"Bluetooth", NULL, false, .sub_menu = &ble_set_menu, menu_para_ble_state},
    {"Language", NULL, false, .sub_menu = &language_set_menu,
     menu_para_language},
    {"Shutdown", NULL, false, .sub_menu = &shutdown_set_menu,
     menu_para_shutdown}};

static struct menu settings_menu = {
    .start = (COUNT_OF(settings_menu_items) - 1) / 2,
    .current = (COUNT_OF(settings_menu_items) - 1) / 2,
    .counts = COUNT_OF(settings_menu_items),
    .title = NULL,
    .items = settings_menu_items,
    .previous = &main_menu,
};

static struct menu_item main_menu_items[] = {
    {"Settings", NULL, false, .sub_menu = &settings_menu, NULL},
    {"Fastpay", NULL, false, NULL, NULL},
    {"About", NULL, true, layoutDeviceParameters, NULL},
    {"Reset", NULL, true, menu_erase_device, NULL}};

static struct menu main_menu = {
    .start = (COUNT_OF(main_menu_items) - 1) / 2,
    .current = (COUNT_OF(main_menu_items) - 1) / 2,
    .counts = COUNT_OF(main_menu_items),
    .title = NULL,
    .items = main_menu_items,
    .previous = NULL,
};

static struct menu_item main_uninitialized_menu_items[] = {
    {"Manual", NULL, true, menu_showQRCode, NULL},
    {"Create wallet", NULL, true, menu_reset_device, NULL},
    {"Import seed", NULL, true, menu_recovery_device, NULL},
    {"Settings", NULL, false, .sub_menu = &settings_menu, NULL},
    {"About", NULL, true, layoutDeviceParameters, NULL}};

static struct menu main_uninitilized_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(main_uninitialized_menu_items),
    .title = NULL,
    .items = main_uninitialized_menu_items,
    .previous = NULL,
};

void menu_language_init(void) {
  uint8_t key = KEY_NULL;
  menu_init(&language_set_menu);
refresh_menu:
  menu_display(&language_set_menu);
  key = waitKey(0, 0);
  switch (key) {
    case KEY_UP:
      menu_up();
      goto refresh_menu;
    case KEY_DOWN:
      menu_down();
      goto refresh_menu;
    case KEY_CONFIRM:
      menu_enter();
      break;
    default:
      break;
  }
}

void main_menu_init(bool state) {
  if (state) {
    menu_init(&main_menu);
    menu_update(&settings_menu, previous, &main_menu);
  } else {
    menu_init(&main_uninitilized_menu);
    menu_update(&settings_menu, previous, &main_uninitilized_menu);
  }
}
