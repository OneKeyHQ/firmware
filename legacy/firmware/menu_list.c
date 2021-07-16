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
    layoutDialogSwipeCenterAdapter(&bmp_icon_ok, NULL, NULL, &bmp_btn_confirm,
                                   _("Done"), NULL, NULL, NULL, NULL,
                                   _("Wallet Recovery Success"), NULL, NULL);
    protectWaitKey(0, 1);
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
    layoutDialogSwipeCenterAdapter(&bmp_icon_ok, NULL, NULL, &bmp_btn_confirm,
                                   _("Done"), NULL, NULL, NULL, NULL,
                                   _("Wallet created success"), NULL, NULL);
    protectWaitKey(0, 1);
    layoutHome();
  }
}

void menu_manual(int index) {
  (void)index;
  int page = 0;
  uint8_t key = KEY_NULL;
  char index_str[] = "1/5";
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
      strcat(desc, "        ");
      strcat(desc, _("to scroll up"));
      // English
      if (desc[0] == 'P')
        offset = 10;
      else
        offset = 20;
      oledDrawBitmap(offset + len + 2, 16, &bmp_icon_up);
      oledDrawStringAdapter(offset, 20, desc, FONT_STANDARD);

      memset(desc, 0, sizeof(desc));

      strcat(desc, _("Press"));
      strcat(desc, "        ");
      strcat(desc, _("to scroll down"));
      oledDrawBitmap(offset + len + 2, 30, &bmp_icon_down);
      oledDrawStringAdapter(offset, 34, desc, FONT_STANDARD);

      oledDrawBitmap(60, OLED_HEIGHT - 8, &bmp_btn_down);

      break;
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
      oledDrawBitmap(60, 0, &bmp_btn_up);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2 * 10, _("Download Onekey"),
                                  FONT_STANDARD);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * 10, "onekey.so/download",
                                  FONT_STANDARD);
      oledDrawBitmap(60, OLED_HEIGHT - 8, &bmp_btn_down);
      break;

    case 3:
      oledDrawBitmap(60, 0, &bmp_btn_up);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2 * 10,
                                  _("Scan the QR code below"), FONT_STANDARD);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * 10,
                                  _("For more information"), FONT_STANDARD);
      oledDrawBitmap(60, OLED_HEIGHT - 8, &bmp_btn_down);
      break;

    case 4:
      layoutQRCode(index_str, &bmp_btn_up, NULL, NULL,
                   "https://onekey.zendesk.com/hc/zh-cn/articles/360002123856");
      break;
  }
  layoutButtonYesAdapter(_("Okay"), &bmp_btn_confirm);
  oledRefresh();
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_DOWN:
    case KEY_CONFIRM:
      if (page < 4)
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
  layoutDialogSwipeCenterAdapter(NULL, &bmp_btn_back, _("Back"),
                                 &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
                                 _("Make sure you still have"),
                                 _("backup of seed phrases"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  if (!protectPinOnDevice(false, true)) {
    return;
  }
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL,
      NULL, _("All data will be lost."), _("This cannot be undo!"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_confirm, _("Reset "), NULL, NULL,
      NULL, NULL, _("Are you sure to reset?"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  uint8_t ui_language_bak = ui_language;

  config_wipe();
  if (ui_language_bak) {
    ui_language = ui_language_bak;
  }
  layoutDialogSwipeCenterAdapter(
      &bmp_icon_info, NULL, NULL, &bmp_btn_confirm, _("Confirm"), NULL, NULL,
      NULL, NULL, _("Device has been reset"), _("Please reboot"), NULL);
  protectWaitKey(0, 0);
#if !EMULATOR
  svc_system_reset();
#endif
}

void menu_changePin(int index) {
  (void)index;
  uint8_t key = KEY_NULL;

  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_confirm, _("Next"), NULL, NULL,
      NULL, _("Before your start"), _("Check PIN Code"), NULL, NULL);
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
     menu_para_shutdown}
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
    {"Settings", NULL, false, .sub_menu = &settings_menu, NULL},
    {"Security", NULL, false, .sub_menu = &security_set_menu, NULL},
    {"About", NULL, true, layoutDeviceParameters, NULL}};

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
    {"Create", NULL, true, menu_reset_device, NULL},
    {"Restore", NULL, true, menu_recovery_device, NULL},
    {"About", NULL, true, layoutDeviceParameters, NULL},
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
