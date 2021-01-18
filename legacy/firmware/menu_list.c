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

extern uint8_t ui_language;

static struct menu settings_menu, main_menu;

void menu_recovery_device(int index) {
  (void)index;
  uint8_t key = KEY_NULL;
  if (!protectPinOnDevice(true, true)) {
    return;
  }
  layoutDialogSwipeCenterAdapter(
      &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
      _("Enter seed phrases to"), _("restore wallet"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  recovery_on_device();
  if (config_isInitialized()) {
    layoutDialogSwipeCenterAdapter(
        NULL, NULL, &bmp_btn_confirm, _("Done"), NULL, NULL, NULL,
        _("Wallet Recovery Success"), NULL, NULL, NULL);
    protectWaitKey(0, 1);
    layoutHome();
  }
}

void menu_reset_device(int index) {
  (void)index;
  uint8_t key = KEY_NULL;
  if (!protectPinOnDevice(true, true)) {
    return;
  }
  layoutDialogSwipeCenterAdapter(
      &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
      _("Follow the prompts"), _("to creat wallet"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  reset_on_device();
  if (config_isInitialized()) {
    layoutDialogSwipeCenterAdapter(NULL, NULL, &bmp_btn_confirm, _("Done"),
                                   NULL, NULL, NULL, _("Wallet created"),
                                   _("successfully"), NULL, NULL);
    protectWaitKey(0, 1);
    layoutHome();
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
      oledClear_ex();
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

      break;
    case 1:
      layoutQRCode(index_str, &bmp_btn_up, &bmp_btn_down, _("Download Onekey"),
                   "https://onekey.so/download");
      break;

    case 2:
      layoutQRCode(index_str, &bmp_btn_up, NULL, _("For more information"),
                   "https://onekey.zendesk.com/hc/zh-cn/articles/360002123856");
      break;
  }
  layoutButtonYesAdapter(_("Okay"), &bmp_btn_confirm);
  oledRefresh();
  key = protectWaitKey(0, 0);
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
  layoutDialogSwipeCenterAdapter(
      &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
      _("Make sure you still have"), _("backup of seed phrases"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  if (!protectPinOnDevice(false, true)) {
    return;
  }
  layoutDialogSwipeCenterAdapter(
      &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
      _("All data will be lost."), _("This cannot be undo!"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  layoutDialogSwipeCenterAdapter(&bmp_btn_back, _("Back"), &bmp_btn_confirm,
                                 _("Reset"), NULL, NULL, NULL, NULL,
                                 _("Are you sure to reset?"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  uint8_t ui_language_bak = ui_language;

  config_wipe();
  if (ui_language_bak) {
    ui_language = ui_language_bak;
  }
  layoutDialogSwipeCenterAdapter(NULL, NULL, &bmp_btn_confirm, _("Confirm"),
                                 NULL, NULL, NULL, _("Device has been reset"),
                                 NULL, _("Please reboot"), NULL);
  protectWaitKey(0, 0);
#if !EMULATOR
  svc_system_reset();
#endif
}

void menu_changePin(int index) {
  (void)index;
  protectChangePinOnDevice(true);
}

void menu_showMnemonic(int index) {
  (void)index;
  if (protectPinOnDevice(false, true)) {
    char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
    config_getMnemonic(mnemonic, sizeof(mnemonic));
    scroll_mnemonic(_("Mnemonic"), mnemonic, 0);
  }
}

static struct menu_item ble_set_menu_items[] = {
    {"On", NULL, true, menu_para_set_ble, NULL},
    {"Off", NULL, true, menu_para_set_ble, NULL}};

static struct menu ble_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(ble_set_menu_items),
    .title = NULL,
    .items = ble_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item language_set_menu_items[] = {
    {"English ", NULL, true, menu_para_set_language, NULL},
    {"简体中文", NULL, true, menu_para_set_language, NULL}};

static struct menu language_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(language_set_menu_items),
    .title = NULL,
    .items = language_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item autolock_set_menu_items[] = {
    {"1", "minute", true, menu_para_set_sleep, NULL},
    {"2", "minutes", true, menu_para_set_sleep, NULL},
    {"5", "minutes", true, menu_para_set_sleep, NULL},
    {"10", "minutes", true, menu_para_set_sleep, NULL},
    {"Never", NULL, true, menu_para_set_sleep, NULL}};

static struct menu autolock_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(autolock_set_menu_items),
    .title = NULL,
    .items = autolock_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item shutdown_set_menu_items[] = {
    {"10", "minute", true, menu_para_set_shutdown, NULL},
    {"30", "minutes", true, menu_para_set_shutdown, NULL},
    {"1", "hour", true, menu_para_set_shutdown, NULL},
    {"2", "hours", true, menu_para_set_shutdown, NULL},
    {"Never", NULL, true, menu_para_set_shutdown, NULL}};

static struct menu shutdown_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(shutdown_set_menu_items),
    .title = NULL,
    .items = shutdown_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item settings_menu_items[] = {
    {"Bluetooth", NULL, false, .sub_menu = &ble_set_menu, menu_para_ble_state},
    {"Language", NULL, false, .sub_menu = &language_set_menu,
     menu_para_language},
    {"AutoLock", NULL, false, .sub_menu = &autolock_set_menu,
     menu_para_autolock},
    {"Shutdown", NULL, false, .sub_menu = &shutdown_set_menu,
     menu_para_shutdown}};

static struct menu settings_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(settings_menu_items),
    .title = NULL,
    .items = settings_menu_items,
    .previous = &main_menu,
};

static struct menu_item security_set_menu_items[] = {
    {"Change PIN", NULL, true, menu_changePin, NULL},
    {"Reset", NULL, true, menu_erase_device, NULL},
    {"Check Mnemonic", NULL, true, menu_showMnemonic, NULL}};

static struct menu security_set_menu = {
    .start = 0,
    .current = 0,
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
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(main_menu_items),
    .title = NULL,
    .items = main_menu_items,
    .previous = NULL,
};

static struct menu_item main_uninitialized_menu_items[] = {
    {"Guide", NULL, true, menu_manual, NULL},
    {"Create", NULL, true, menu_reset_device, NULL},
    {"Restore", NULL, true, menu_recovery_device, NULL},
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
  int index = 0;
refresh_menu:
  layoutItemsSelectAdapter(&bmp_btn_up, &bmp_btn_down, NULL, &bmp_btn_confirm,
                           NULL, index == 0 ? "Okay" : "确认", index + 1, 2,
                           NULL, NULL, index == 0 ? "English" : "简体中文",
                           index > 0 ? "English" : NULL,
                           index == 0 ? "简体中文" : NULL);

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) index--;
      goto refresh_menu;
    case KEY_DOWN:
      if (index < 1) index++;
      goto refresh_menu;
    case KEY_CANCEL:
      goto refresh_menu;
    case KEY_CONFIRM:
      menu_para_set_language(index);
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
