#include "menu_list.h"
#include "menu_core.h"
#include "menu_para.h"

#include "buttons.h"
#include "config.h"
#include "gettext.h"
#include "layout2.h"
#include "oled.h"
#include "protect.h"
#include "recovery.h"
#include "reset.h"

static struct menu settings_menu, main_menu;

void menu_recovery_device(int index) {
  (void)index;
  if (protectPinOnDevice(true)) {
    recovery_on_device();
    if (config_isInitialized()) {
      protectChangePinOnDevice();
    }
  }
}

void menu_reset_device(int index) {
  (void)index;
  if (protectPinOnDevice(true)) {
    reset_on_device();
    if (config_isInitialized()) {
      protectChangePinOnDevice();
    }
  }
}

void menu_manual(int index) {
  (void)index;
  int page = 0;
  uint8_t key = KEY_NULL;
  char index_str[] = "1/3";

refresh_menu:

  index_str[0] = page + '1';

  switch (page) {
    case 0:
      oledClear();
      oledDrawStringAdapter(0, 0, index_str, FONT_STANDARD);
      oledDrawBitmap(48, 10, &bmp_btn_enter);
      oledDrawBitmap(48, 2 * 10, &bmp_btn_exit);
      oledDrawBitmap(48, 3 * 10, &bmp_btn_up);
      oledDrawBitmap(48, 4 * 10, &bmp_btn_down);

      oledDrawStringAdapter(58, 10, ":", FONT_STANDARD);
      oledDrawStringAdapter(58, 2 * 10, ":", FONT_STANDARD);
      oledDrawStringAdapter(58, 3 * 10, ":", FONT_STANDARD);
      oledDrawStringAdapter(58, 4 * 10, ":", FONT_STANDARD);

      oledDrawStringAdapter(64, 10, _("Confirm"), FONT_STANDARD);
      oledDrawStringAdapter(64, 2 * 10, _("Cancel"), FONT_STANDARD);
      oledDrawStringAdapter(64, 3 * 10, _("Page up"), FONT_STANDARD);
      oledDrawStringAdapter(64, 4 * 10, _("Page down"), FONT_STANDARD);

      oledDrawBitmap(60, OLED_HEIGHT - 8, &bmp_btn_down);

      oledRefresh();
      break;
    case 1:
      layoutQRCode(index_str, &bmp_btn_up, &bmp_btn_down, _("Download APP"),
                   "https://onekey.so/download");
      break;

    case 2:
      layoutQRCode(index_str, &bmp_btn_up, NULL, _("For more information"),
                   "https://onekey.zendesk.com/hc/zh-cn/articles/360002123856");
      break;
  }

  key = protectWaitKey(timer1s * 30, 0);
  switch (key) {
    case KEY_DOWN:
    case KEY_CONFIRM:
      if (page < 2) page++;
      goto refresh_menu;
    case KEY_UP:
      if (page) page--;
      goto refresh_menu;
    case KEY_CANCEL:
      break;
    default:
      break;
  }
}

void menu_erase_device(int index) {
  (void)index;
  uint8_t key = KEY_NULL;
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Do you really want to"), _("wipe the device?"), NULL,
                    _("All data will be lost."), NULL, NULL);
  key = protectWaitKey(timer1s * 60, 1);
  if (key == KEY_CONFIRM) {
    if (protectPinOnDevice(false)) {
      config_wipe();
    }
  }
  layoutHome();
}

void menu_changePin(int index) {
  (void)index;
  protectChangePinOnDevice();
}

void menu_showMnemonic(int index) {
  (void)index;
  if (protectPinOnDevice(false)) {
    char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
    config_getMnemonic(mnemonic, sizeof(mnemonic));
    scroll_mnemonic(_("Mnemonic"), mnemonic);
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

static struct menu_item security_set_menu_items[] = {
    {"Change PIN", NULL, true, menu_changePin, NULL},
    {"Mnemonic", NULL, true, menu_showMnemonic, NULL},
    {"Reset", NULL, true, menu_erase_device, NULL}};

static struct menu security_set_menu = {
    .start = (COUNT_OF(security_set_menu_items) - 1) / 2,
    .current = (COUNT_OF(security_set_menu_items) - 1) / 2,
    .counts = COUNT_OF(security_set_menu_items),
    .title = NULL,
    .items = security_set_menu_items,
    .previous = &main_menu,
};

static struct menu_item main_menu_items[] = {
    {"Settings", NULL, false, .sub_menu = &settings_menu, NULL},
    {"About", NULL, true, layoutDeviceParameters, NULL},
    {"Security", NULL, false, .sub_menu = &security_set_menu, NULL}};

static struct menu main_menu = {
    .start = (COUNT_OF(main_menu_items) - 1) / 2,
    .current = (COUNT_OF(main_menu_items) - 1) / 2,
    .counts = COUNT_OF(main_menu_items),
    .title = NULL,
    .items = main_menu_items,
    .previous = NULL,
};

static struct menu_item main_uninitialized_menu_items[] = {
    {"Manual", NULL, true, menu_manual, NULL},
    {"Create wallet", NULL, true, menu_reset_device, NULL},
    {"Import mnemonic", NULL, true, menu_recovery_device, NULL},
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
  menu_update(&language_set_menu, title, NULL);
refresh_menu:
  menu_display(&language_set_menu);
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      menu_up();
      goto refresh_menu;
    case KEY_DOWN:
      menu_down();
      goto refresh_menu;
    case KEY_CONFIRM:
      menu_enter();
      menu_update(&language_set_menu, title, "Language");
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
