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
bool exitBlindSignByInitialize;
static struct menu settings_menu, main_menu, security_set_menu, blind_sign_menu;

void menu_recovery_device(int index) {
  (void)index;
  if (!protectPinOnDevice(true, true)) {
    return;
  }
  recovery_on_device();
  if (config_isInitialized()) {
#if ONEKEY_MINI
  refresh:
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

#if ONEKEY_MINI
    uint8_t key = protectWaitKey(0, 1);
    setRgbBitmap(false);
    if (key != KEY_CONFIRM) goto refresh;
#else
    protectWaitKey(0, 1);
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
  refresh:
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

#if ONEKEY_MINI
    uint8_t key = protectWaitKey(0, 1);
    setRgbBitmap(false);
    if (key != KEY_CONFIRM) goto refresh;
#else
    protectWaitKey(0, 1);
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
#if ONEKEY_MINI
  oledDrawStringAdapter(0, HIGH_OFFSET, index_str, FONT_STANDARD | FONT_FIXED);
#else
  oledDrawStringAdapter(0, 0, index_str, FONT_STANDARD | FONT_FIXED);
#endif

  switch (page) {
    case 0:
      len = oledStringWidthAdapter(_("Press"), FONT_STANDARD);
      memset(desc, 0, sizeof(desc));
      strcat(desc, _("Press"));
#if ONEKEY_MINI
      strcat(desc, "      ");
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
      strcat(desc, "      ");
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
      strcat(desc, "      ");
      strcat(desc, _("Cancel or Back"));
      oledDrawBitmap(offset + len + 4, 48, &bmp_button_back);
      oledDrawStringAdapter(offset, 48, desc, FONT_STANDARD);

      memset(desc, 0, sizeof(desc));
      strcat(desc, _("Press"));
      strcat(desc, "      ");
      strcat(desc, _("OK or Next"));

      // English
      if (desc[0] == 'P')
        offset = 0;
      else
        offset = 20;
      oledDrawBitmap(offset + len + 4, 62, &bmp_button_forward);
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
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * 10,
                                  _("Download OneKey Apps"), FONT_STANDARD);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 6 * 10, "onekey.so/download",
                                  FONT_STANDARD);
      oledBox(23, 70, 108, 70, true);
#else
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 2 * 10, _("Download OneKey"),
                                  FONT_STANDARD);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 3 * 10, "onekey.so/download",
                                  FONT_STANDARD);
#endif
      oledDrawBitmap(60, OLED_HEIGHT - 8, &bmp_btn_down);
      break;

#if ONEKEY_MINI
    case 2:
      oledDrawBitmap(60, 0, &bmp_btn_up);
      oledDrawStringCenterAdapterEx(
          OLED_WIDTH / 2, 3 * 10,
          _("To learn more about\nhow to use, go to the\nHelp Center"),
          FONT_STANDARD);
      oledDrawStringCenterAdapter(OLED_WIDTH / 2, 7 * 10, "help.onekey.so",
                                  FONT_STANDARD);
      oledBox(36, 80, 95, 80, true);
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
  layoutDialogSwipeCenterAdapterEx(
      NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
      true, NULL, NULL, NULL, NULL,
      _("Before you reset the\ndevice, please ensure\nyou've backed up "
        "your\nrecovery phrase"),
      NULL, NULL, NULL, NULL, NULL, NULL, NULL);
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
  layoutDialogSwipeCenterAdapterEx(
      NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
      true, NULL, NULL, NULL, NULL,
      _("All data on the device\nwill be erased if the\ndevice is reset, "
        "and\nthis operation cannot\nbe reversed."),
      NULL, NULL, NULL, NULL, NULL, NULL, NULL);
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

#if ONEKEY_MINI
  layoutDialogCenterAdapterEx(NULL, NULL, NULL, NULL, NULL, NULL, true, NULL,
                              NULL, _("Device reset in\nprogress"), NULL, NULL,
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif

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
#if !ONEKEY_MINI
  uint8_t key = KEY_NULL;

  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_confirm, _("Next"), NULL, NULL,
      NULL, _("Before your start"), _("Check PIN Code"), NULL, NULL);

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
#endif

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

void menu_set_eth_eip(int index) {
  char desc[64] = {0};

  bool state = index ? false : true;
  if (config_hasPin()) {
    if (!protectPinOnDevice(false, true)) {
      return;
    }
  }

  config_setCoinSwitch(COIN_SWITCH_ETH_EIP712, state);

  strcat(desc, _("ETH advance signing turn"));
  strcat(desc, state ? _(" On") : _(" Off"));
  layoutDialogSwipeCenterAdapter(&bmp_icon_ok, NULL, NULL, &bmp_btn_confirm,
                                 _("Done"), NULL, NULL, NULL, NULL, desc, NULL,
                                 NULL);
  protectWaitKey(0, 1);
  layoutHome();
}

void menu_set_sol_blind_sign(int index) {
  char desc[64] = {0};

  bool state = index ? false : true;
  if (config_hasPin()) {
    if (!protectPinOnDevice(false, true)) {
      return;
    }
  }

  config_setCoinSwitch(COIN_SWITCH_SOLANA, state);

  strcat(desc, _("SOL advance signing turn"));
  strcat(desc, state ? _(" On") : _(" Off"));
  layoutDialogSwipeCenterAdapter(&bmp_icon_ok, NULL, NULL, &bmp_btn_confirm,
                                 _("Done"), NULL, NULL, NULL, NULL, desc, NULL,
                                 NULL);
  protectWaitKey(0, 1);
  layoutHome();
}

void menu_blind_sign(int index) {
  (void)index;

  uint8_t key = KEY_NULL;

step1:
#if ONEKEY_MINI
  layoutDialogSwipeCenterAdapterEx(
      NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
      true, NULL, NULL, NULL, NULL,
      _("After enabling\"Blind \nSigning\",your device \nwill support signing "
        "\n"
        "for messages and \ntransactions, but it \ncan't decode the metadata."),
      NULL, NULL, NULL, NULL, NULL, NULL, NULL);

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }

  layoutDialogSwipeCenterAdapterEx(
      NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
      true, NULL, NULL, NULL, NULL,
      _("Visiting Help Center \nand search \"Blind \nSigning\" to learn \n"
        "more\n \nhelp.onekey.so"),
      NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#else
  layoutDialogAdapter_ex(NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward,
                         _("Next"), NULL,
                         _("After enabling \n\"Blind Signing\",your device "
                           " will support signing for messages and "
                           "transactions, but it can't decode the metadata."),
                         NULL, NULL, NULL, NULL, NULL);

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }

  layoutDialogAdapter_ex(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL,
      _("Visiting Help Center and search \"Blind Signing\" to "
        "learn more\n help.onekey.so"),
      NULL, NULL, NULL, NULL);
#endif

  key = protectWaitKey(0, 1);
  if (key == KEY_CANCEL) {
    goto step1;
  } else if (key != KEY_CONFIRM) {
    return;
  }

  menu_init(&blind_sign_menu);
  menu_display(&blind_sign_menu);

  while (1) {
    key = blindsignWaitKey();
    if (exitBlindSignByInitialize) {
      return;
    }
    if (key == KEY_CANCEL) {
      menu_init(&security_set_menu);
      return;
    }
    menu_run(key, 0);
    if (layoutLast == layoutHome) {
      return;
    }
  }
}

void menu_set_passphrase(int index) {
  (void)index;

  uint8_t key = KEY_NULL;

  if (index) {
    layoutDialogAdapter_ex(
        NULL, &bmp_btn_back, _("Cancel"), &bmp_btn_forward, _("Confirm"), NULL,
        NULL, _("Do you really want to \ndisable passphrase protection?"), NULL,
        NULL, NULL, NULL);
  } else {
    layoutDialogAdapter_ex(
        NULL, &bmp_btn_back, _("Cancel"), &bmp_btn_forward, _("Confirm"), NULL,
        NULL, _("Do you really want to \nenable passphrase protection?"), NULL,
        NULL, NULL, NULL);
  }

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }

  config_setPassphraseProtection(index ? false : true);
}

#if !ONEKEY_MINI
static struct menu_item ble_set_menu_items[] = {
    {"On", NULL, true, menu_para_set_ble, NULL, true},
    {"Off", NULL, true, menu_para_set_ble, NULL, true}};

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
    {"English ", NULL, true, menu_para_set_language, NULL, true},
    {"简体中文", NULL, true, menu_para_set_language, NULL, true}};

static struct menu language_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(language_set_menu_items),
    .title = NULL,
    .items = language_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item autolock_set_menu_items[] = {
    {"1", "minute", true, menu_para_set_sleep, NULL, true},
    {"2", "minutes", true, menu_para_set_sleep, NULL, true},
    {"5", "minutes", true, menu_para_set_sleep, NULL, true},
    {"10", "minutes", true, menu_para_set_sleep, NULL, true},
    {"Never", NULL, true, menu_para_set_sleep, NULL, true}};

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
    {"10", "minute", true, menu_para_set_shutdown, NULL, true},
    {"30", "minutes", true, menu_para_set_shutdown, NULL, true},
    {"1", "hour", true, menu_para_set_shutdown, NULL, true},
    {"2", "hours", true, menu_para_set_shutdown, NULL, true},
    {"Never", NULL, true, menu_para_set_shutdown, NULL, true}};

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
    {"High", NULL, true, menu_para_set_brightness, NULL, true},
    {"Medium", NULL, true, menu_para_set_brightness, NULL, true},
    {"Low", NULL, true, menu_para_set_brightness, NULL, true}};

static struct menu brightness_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(brightness_set_menu_items),
    .title = NULL,
    .items = brightness_set_menu_items,
    .previous = &settings_menu,
};
#endif

static struct menu_item passphrase_set_menu_items[] = {
    {"On", NULL, true, menu_set_passphrase, NULL, true},
    {"Off", NULL, true, menu_set_passphrase, NULL, true}};

static struct menu passphrase_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(passphrase_set_menu_items),
    .title = NULL,
    .items = passphrase_set_menu_items,
    .previous = &security_set_menu,
};

static struct menu_item settings_menu_items[] = {
#if !ONEKEY_MINI
    {"Bluetooth", NULL, false, .sub_menu = &ble_set_menu, menu_para_ble_state,
     false},
#endif
    {"Language", NULL, false, .sub_menu = &language_set_menu,
     menu_para_language, false},
    {"AutoLock", NULL, false, .sub_menu = &autolock_set_menu,
     menu_para_autolock, false},
#if !ONEKEY_MINI
    {"Shutdown", NULL, false, .sub_menu = &shutdown_set_menu,
     menu_para_shutdown, false},
#endif
#if ONEKEY_MINI
    {"Brightness", NULL, false, .sub_menu = &brightness_set_menu,
     menu_para_brightness, false}
#endif
};

static struct menu settings_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(settings_menu_items),
    .title = NULL,
#if ONEKEY_MINI
    .button_type = BTN_TYPE_NEXT,
#endif
    .items = settings_menu_items,
    .previous = &main_menu,
};

#if ONEKEY_MINI
void menu_check_all_words(int index) {
  (void)index;
  char desc[64] = "";
  uint8_t key = KEY_NULL;
  uint32_t word_count = 0;

  if (protectPinOnDevice(false, true)) {
    char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
    memset(desc, 0, sizeof(desc));
    config_getMnemonic(mnemonic, sizeof(mnemonic));

    word_count = get_mnemonic_number(mnemonic);
    if (word_count == 12)
      strcat(
          desc,
          _("Please enter 12 words\nin order to verify\nyour recovery phrase"));
    else if (word_count == 18)
      strcat(
          desc,
          _("Please enter 18 words\nin order to verify\nyour recovery phrase"));
    else if (word_count == 24)
      strcat(
          desc,
          _("Please enter 24 words\nin order to verify\nyour recovery phrase"));
    else
      return;

    layoutDialogSwipeCenterAdapterEx(NULL, &bmp_button_back, _("BACK"),
                                     &bmp_button_forward, _("NEXT"), NULL, true,
                                     NULL, NULL, NULL, NULL, desc, NULL, NULL,
                                     NULL, NULL, NULL, NULL, NULL);
    key = protectWaitKey(0, 1);
    if (key != KEY_CONFIRM) {
      return;
    }

    if (!verify_words(word_count)) {
      return;
    }
  }
}

void menu_check_specified_word(int index) {
  (void)index;
  uint32_t word_count = 0;

  if (protectPinOnDevice(false, true)) {
    char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
    config_getMnemonic(mnemonic, sizeof(mnemonic));
    word_count = get_mnemonic_number(mnemonic);

    if (!verify_words(word_count)) {
      return;
    }
  }
}

#if 0
static struct menu_item check_word_menu_items[] = {
    {"Verify All Words", NULL, true, menu_check_all_words, NULL, false},
    {"Verify Specified Word", NULL, true, menu_check_specified_word, NULL,
     false}};

static struct menu check_word_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(check_word_menu_items),
    .title = NULL,
    .items = check_word_menu_items,
    .previous = &security_set_menu,
};
#endif
#endif

static struct menu_item security_set_menu_items[] = {
    {"Change PIN", NULL, true, menu_changePin, NULL, false},
    {"Blind Signing", NULL, true, menu_blind_sign, NULL, false},
    {"Recovery Phrase verify", NULL, true, menu_check_all_words, NULL, false},
    {"Passphrase", NULL, false, .sub_menu = &passphrase_set_menu,
     menu_para_passphrase, true},
    {"Reset", NULL, true, menu_erase_device, NULL, false},
    //{"Check Mnemonic", NULL, true, menu_showMnemonic, NULL}
};

static struct menu security_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(security_set_menu_items),
    .title = NULL,
#if ONEKEY_MINI
    .button_type = BTN_TYPE_NEXT,
#endif
    .items = security_set_menu_items,
    .previous = &main_menu,
};

static struct menu_item main_menu_items[] = {
#if ONEKEY_MINI
    {"Settings ", NULL, false, .sub_menu = &settings_menu, NULL, false},
#else
    {"Settings", NULL, false, .sub_menu = &settings_menu, NULL, false},
#endif
    {"Security", NULL, false, .sub_menu = &security_set_menu, NULL, false},
    {"About", NULL, true, layoutDeviceParameters, NULL, false}};

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
    {"Guide", NULL, true, menu_manual, NULL, false},
#if ONEKEY_MINI
    {"Create New Wallet", NULL, true, menu_reset_device, NULL, false},
    {"Restore Wallet", NULL, true, menu_recovery_device, NULL, false},
    {"About", NULL, true, layoutDeviceParameters, NULL, false},
    {"Settings ", NULL, false, .sub_menu = &settings_menu, NULL, false}
#else
    {"Create", NULL, true, menu_reset_device, NULL, false},
    {"Restore", NULL, true, menu_recovery_device, NULL, false},
    {"About", NULL, true, layoutDeviceParameters, NULL, false},
    {"Settings", NULL, false, .sub_menu = &settings_menu, NULL, false}
#endif
};

static struct menu main_uninitilized_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(main_uninitialized_menu_items),
    .title = NULL,
    .items = main_uninitialized_menu_items,
    .previous = NULL,
    .button_type = BTN_TYPE_NEXT,
};

static struct menu_item eth_eip_set_menu_items[] = {
    {"On", NULL, true, menu_set_eth_eip, NULL, false},
    {"Off", NULL, true, menu_set_eth_eip, NULL, false}};

static struct menu eth_eip_switch_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(eth_eip_set_menu_items),
    .title = NULL,
    .items = eth_eip_set_menu_items,
    .previous = &blind_sign_menu,
};

static struct menu_item sol_set_menu_items[] = {
    {"On", NULL, true, menu_set_sol_blind_sign, NULL, false},
    {"Off", NULL, true, menu_set_sol_blind_sign, NULL, false}};

static struct menu sol_switch_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(sol_set_menu_items),
    .title = NULL,
    .items = sol_set_menu_items,
    .previous = &blind_sign_menu,
};

static struct menu_item blind_sign_menu_items[] = {
    {"Advance ETH Sign", NULL, false, .sub_menu = &eth_eip_switch_menu,
     menu_para_eth_eip_switch, false},
    {"Advance SOL Sign", NULL, false, .sub_menu = &sol_switch_menu,
     menu_para_sol_switch, false}};

static struct menu blind_sign_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(blind_sign_menu_items),
    .title = NULL,
    .items = blind_sign_menu_items,
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
