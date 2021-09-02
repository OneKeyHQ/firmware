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
  if (!protectPinOnDevice(true, true)) {
    return;
  }
  recovery_on_device();
  if (config_isInitialized()) {
#if ONEKEY_MINI
    setRgbBitmap(true);
    layoutDialogSwipeCenterAdapterEx(
        &bmp_icon_success, NULL, NULL, &bmp_button_forward, _("Done"), NULL,
        true, NULL, NULL, NULL, NULL, NULL, NULL,
        _("Wallet restored\nsuccessfully"), NULL, NULL, NULL, NULL, NULL);
#else
    layoutDialogSwipeCenterAdapter(&bmp_icon_ok, NULL, NULL, &bmp_btn_confirm,
                                   _("Done"), NULL, NULL, NULL, NULL,
                                   _("Wallet Recovery Success"), NULL, NULL);
#endif

    protectWaitKey(0, 1);
#if ONEKEY_MINI
    setRgbBitmap(false);
#endif
    layoutHome();
  }
}

void menu_reset_device(int index) {
  (void)index;
  if (!protectPinOnDevice(true, true)) {
    return;
  }
  reset_on_device();
  if (config_isInitialized()) {
#if ONEKEY_MINI
    setRgbBitmap(true);
    layoutDialogSwipeCenterAdapterEx(
        &bmp_icon_success, NULL, NULL, &bmp_button_forward, _("Done"), NULL,
        true, NULL, NULL, NULL, NULL, NULL, _("Wallet created\nsuccessfully"),
        NULL, NULL, NULL, NULL, NULL, NULL);
#else
    layoutDialogSwipeCenterAdapter(&bmp_icon_ok, NULL, NULL, &bmp_btn_confirm,
                                   _("Done"), NULL, NULL, NULL, NULL,
                                   _("Wallet created success"), NULL, NULL);
#endif

    protectWaitKey(0, 1);
#if ONEKEY_MINI
    setRgbBitmap(false);
#endif
    layoutHome();
  }
}

void menu_manual(int index) {
  (void)index;
  int page = 0;
  uint8_t key = KEY_NULL;
#if ONEKEY_MINI
  char index_str[] = "1/3";
#else
  char index_str[] = "1/5";
#endif
  char desc[64] = "";
  int offset = 0;
  int len = 0;
refresh_menu:

  index_str[0] = page + '1';

  oledClear_ex();
  oledDrawStringAdapter(0, 0, index_str, FONT_STANDARD | FONT_FIXED);

  switch (page) {
    case 0:
      len = oledStringWidthAdapter(_("Press"), FONT_STANDARD);
      memset(desc, 0, sizeof(desc));
      strcat(desc, _("Press"));
#if ONEKEY_MINI
      strcat(desc, "    ");
#else
      strcat(desc, "        ");
#endif
      strcat(desc, _("Previous Page"));
      // English
      if (desc[0] == 'P')
#if ONEKEY_MINI
        offset = 0;
#else
        offset = 10;
#endif
      else
        offset = 20;
#if ONEKEY_MINI
      oledDrawBitmap(offset + len + 4, 20, &bmp_button_up);
#else
      oledDrawBitmap(offset + len + 2, 16, &bmp_icon_up);
#endif
      oledDrawStringAdapter(offset, 20, desc, FONT_STANDARD);

      memset(desc, 0, sizeof(desc));

      strcat(desc, _("Press"));
#if ONEKEY_MINI
      strcat(desc, "    ");
#else
      strcat(desc, "        ");
#endif
      strcat(desc, _("Next Page"));
#if ONEKEY_MINI
      oledDrawBitmap(offset + len + 4, 34, &bmp_button_down);
#else
      oledDrawBitmap(offset + len + 2, 30, &bmp_icon_down);
#endif
      oledDrawStringAdapter(offset, 34, desc, FONT_STANDARD);
#if ONEKEY_MINI
      memset(desc, 0, sizeof(desc));
      strcat(desc, _("Press"));
      strcat(desc, "    ");
      strcat(desc, _("OK or Next"));
      oledDrawBitmap(offset + len + 4, 48, &bmp_button_forward);
      oledDrawStringAdapter(offset, 48, desc, FONT_STANDARD);

      memset(desc, 0, sizeof(desc));
      strcat(desc, _("Press"));
      strcat(desc, "    ");
      strcat(desc, _("Cancel or Next"));
      // English
      if (desc[0] == 'P')
        offset = 0;
      else
        offset = 20;
      oledDrawBitmap(offset + len + 4, 62, &bmp_button_back);
      oledDrawStringAdapter(offset, 62, desc, FONT_STANDARD);
#endif
      oledDrawBitmap(60, OLED_HEIGHT - 8, &bmp_btn_down);

      break;
#if !ONEKEY_MINI
    case 1:
      oledDrawBitmap(60, 0, &bmp_btn_up);

      memset(desc, 0, sizeof(desc));
      strcat(desc, _("Press"));
      strcat(desc, "        ");
      strcat(desc, _("to back"));
      // English
      if (desc[0] == 'P')
        offset = 20;
      else
        offset = 40;
      oledDrawBitmap(offset + len + 2, 16, &bmp_icon_exit);
      oledDrawStringAdapter(offset, 20, desc, FONT_STANDARD);

      memset(desc, 0, sizeof(desc));

      strcat(desc, _("Press"));
      strcat(desc, "        ");
      strcat(desc, _("to confirm"));
      oledDrawBitmap(offset + len + 2, 30, &bmp_icon_enter);
      oledDrawStringAdapter(offset, 34, desc, FONT_STANDARD);

      oledDrawBitmap(60, OLED_HEIGHT - 8, &bmp_btn_down);

      break;
    case 2:
#else
    case 1:
#endif
      oledDrawBitmap(60, 0, &bmp_btn_up);
#if ONEKEY_MINI
      oledDrawStringCenterAdapter(0, 3 * 10, _("Download Onekey Apps"),
                                  FONT_STANDARD);
      oledDrawStringAdapter(11, 6 * 10, "onekey.so/download", FONT_STANDARD);
      oledBox(13, 70, 117, 70, true);
#else
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2 * 10, _("Download Onekey"),
                                  FONT_STANDARD);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * 10, "onekey.so/download",
                                  FONT_STANDARD);
#endif
      oledDrawBitmap(60, OLED_HEIGHT - 8, &bmp_btn_down);
      break;

#if ONEKEY_MINI
    case 2:
      oledDrawBitmap(60, 0, &bmp_btn_up);
      oledDrawStringCenterAdapter(
          OLED_WIDTH / 2, 3 * 10,
          _("To learn more about\nhow to use, go to the\nHelp Center"),
          FONT_STANDARD);
      oledDrawStringAdapter(20, 7 * 10, "help.onekey.so", FONT_STANDARD);
      oledBox(22, 80, 100, 80, true);
      layoutButtonYesAdapter(_("OK"), &bmp_button_forward);
#else
    case 3:
      oledDrawBitmap(60, 0, &bmp_btn_up);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2 * 10,
                                  _("Scan the QR code below"), FONT_STANDARD);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * 10,
                                  _("For more information"), FONT_STANDARD);
      oledDrawBitmap(60, OLED_HEIGHT - 8, &bmp_btn_down);
#endif
      break;

    case 4:
      layoutQRCode(index_str, &bmp_btn_up, NULL, NULL,
                   "https://onekey.zendesk.com/hc/zh-cn/articles/360002123856");
      break;
  }
#if !ONEKEY_MINI
  layoutButtonYesAdapter(_("Okay"), &bmp_btn_confirm);
#endif
  oledRefresh();
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_DOWN:
    case KEY_CONFIRM:
#if ONEKEY_MINI
      if (page < 2)
#else
      if (page < 4)
#endif
        page++;
      else if (key == KEY_CONFIRM) {
        break;
      }
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
#if ONEKEY_MINI
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
      NULL, NULL, NULL, NULL,
      _("Before you reset the\ndevice, please ensure\nyou've backed up "
        "your\nrecovery phrase"),
      NULL);
#else
  layoutDialogSwipeCenterAdapter(NULL, &bmp_btn_back, _("Back"),
                                 &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
                                 _("Make sure you still have"),
                                 _("backup of seed phrases"), NULL, NULL);
#endif
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  if (!protectPinOnDevice(false, true)) {
    return;
  }
#if ONEKEY_MINI
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
      NULL, NULL,
      _("All data on the device\nwill be erased if the\ndevice is reset, "
        "and\nthis operation cannot\nbe reversed."),
      NULL, NULL, NULL);
#else
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL,
      NULL, _("All data will be lost."), _("This cannot be undo!"), NULL, NULL);
#endif
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
#if ONEKEY_MINI
  setRgbBitmap(true);
  layoutDialogSwipeCenterAdapterEx(
      &bmp_icon_warn, &bmp_button_back, _("BACK"), &bmp_button_forward, _("OK"),
      NULL, true, NULL, NULL, NULL, NULL, NULL, NULL, _("Reset Device?"), NULL,
      NULL, NULL, NULL, NULL);
#else
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_confirm, _("Reset "), NULL, NULL,
      NULL, NULL, _("Are you sure to reset?"), NULL, NULL);
#endif
  key = protectWaitKey(0, 1);
#if ONEKEY_MINI
  setRgbBitmap(false);
#endif

  if (key != KEY_CONFIRM) {
    return;
  }
  uint8_t ui_language_bak = ui_language;

  config_wipe();
  if (ui_language_bak) {
    ui_language = ui_language_bak;
  }
#if ONEKEY_MINI
  setRgbBitmap(true);
  layoutDialogSwipeCenterAdapterEx(&bmp_icon_success, NULL, NULL,
                                   &bmp_button_forward, _("OK"), NULL, true,
                                   NULL, NULL, NULL, NULL, NULL, NULL,
                                   _("Reset successfully,\nrestart the device"),
                                   NULL, NULL, NULL, NULL, NULL);
#else
  layoutDialogSwipeCenterAdapter(
      &bmp_icon_info, NULL, NULL, &bmp_btn_confirm, _("Confirm"), NULL, NULL,
      NULL, NULL, _("Device has been reset"), _("Please reboot"), NULL);
#endif
  protectWaitKey(0, 0);
#if ONEKEY_MINI
  setRgbBitmap(false);
#endif

#if !EMULATOR
  svc_system_reset();
#endif
}

void menu_changePin(int index) {
  (void)index;
  uint8_t key = KEY_NULL;

#if ONEKEY_MINI
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
      NULL, NULL, _("Before your start"), _("Check PIN Code"), NULL, NULL);
#else
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_confirm, _("Next"), NULL, NULL,
      NULL, _("Before your start"), _("Check PIN Code"), NULL, NULL);
#endif

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  protectChangePinOnDevice(true, false);
}

void menu_showMnemonic(int index) {
  (void)index;
  if (protectPinOnDevice(false, true)) {
    char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
    config_getMnemonic(mnemonic, sizeof(mnemonic));
    scroll_mnemonic(_("Mnemonic"), mnemonic, 0);
  }
}

#if !ONEKEY_MINI
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
#endif

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

#if !ONEKEY_MINI
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
#endif

#if ONEKEY_MINI
static struct menu_item brightness_set_menu_items[] = {
    {"High", NULL, true, menu_para_set_brightness, NULL},
    {"Medium", NULL, true, menu_para_set_brightness, NULL},
    {"Low", NULL, true, menu_para_set_brightness, NULL}};

static struct menu brightness_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(brightness_set_menu_items),
    .title = NULL,
    .items = brightness_set_menu_items,
    .previous = &settings_menu,
};
#endif

static struct menu_item settings_menu_items[] = {
#if !ONEKEY_MINI
    {"Bluetooth", NULL, false, .sub_menu = &ble_set_menu, menu_para_ble_state},
#endif
    {"Language", NULL, false, .sub_menu = &language_set_menu,
     menu_para_language},
    {"AutoLock", NULL, false, .sub_menu = &autolock_set_menu,
     menu_para_autolock},
#if !ONEKEY_MINI
    {"Shutdown", NULL, false, .sub_menu = &shutdown_set_menu,
     menu_para_shutdown},
#endif
#if ONEKEY_MINI
    {"Brightness", NULL, false, .sub_menu = &brightness_set_menu,
     menu_para_brightness}
#endif
};

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
    //{"Check Mnemonic", NULL, true, menu_showMnemonic, NULL}
};

static struct menu security_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(security_set_menu_items),
    .title = NULL,
    .items = security_set_menu_items,
    .previous = &main_menu,
};

static struct menu_item main_menu_items[] = {
#if ONEKEY_MINI
    {"Settings ", NULL, false, .sub_menu = &settings_menu, NULL},
#else
    {"Settings", NULL, false, .sub_menu = &settings_menu, NULL},
#endif
    {"Security", NULL, false, .sub_menu = &security_set_menu, NULL},
#if ONEKEY_MINI
    {"About Device", NULL, true, layoutDeviceParameters, NULL}
#else
    {"About", NULL, true, layoutDeviceParameters, NULL}
#endif
};

static struct menu main_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(main_menu_items),
    .title = NULL,
    .items = main_menu_items,
    .previous = NULL,
    .button_type = BTN_TYPE_NEXT,
};

static struct menu_item main_uninitialized_menu_items[] = {
    {"Guide", NULL, true, menu_manual, NULL},
#if ONEKEY_MINI
    {"Create New Wallet", NULL, true, menu_reset_device, NULL},
    {"Restore Wallet", NULL, true, menu_recovery_device, NULL},
    {"About Device", NULL, true, layoutDeviceParameters, NULL},
#else
    {"Create", NULL, true, menu_reset_device, NULL},
    {"Restore", NULL, true, menu_recovery_device, NULL},
    {"About", NULL, true, layoutDeviceParameters, NULL},
#endif
    {"Settings", NULL, false, .sub_menu = &settings_menu, NULL}};

static struct menu main_uninitilized_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(main_uninitialized_menu_items),
    .title = NULL,
    .items = main_uninitialized_menu_items,
    .previous = NULL,
    .button_type = BTN_TYPE_NEXT,
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

  key = waitKey(0, 0);
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
