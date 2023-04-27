#include "menu_list.h"
#include "menu_core.h"
#include "menu_para.h"

#include "buttons.h"
#include "common.h"
#include "config.h"
#include "gettext.h"
#include "layout2.h"
#include "oled.h"
#include "protect.h"
#include "recovery.h"
#include "reset.h"

bool exitBlindSignByInitialize;

static struct menu settings_menu, main_menu, security_set_menu, about_menu;

void menu_erase_device(int index) {
  (void)index;
  uint8_t key = KEY_NULL;

  if (!layoutEraseDevice()) {
    return;
  }
  if (!protectPinOnDevice(false, true)) {
    return;
  }
  layoutDialogAdapterEx(
      _("Erase Device"), &bmp_bottom_left_close, NULL,
      &bmp_bottom_right_confirm, NULL,
      _("Are you sure to reset this \ndevice? This action can not be undo!"),
      NULL, NULL, NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }

  uint8_t ui_language_bak = ui_language;
  config_wipe();
  if (ui_language_bak) {
    ui_language = ui_language_bak;
  }
  if (ui_language == 1) {
    layoutDialogSwipeCenterAdapter(
        &bmp_icon_ok, NULL, NULL, &bmp_bottom_right_confirm, NULL, NULL, NULL,
        NULL, NULL, NULL, _("The device is reset,\nrestart now!"), NULL);
  } else {
    layoutDialogSwipeCenterAdapter(
        &bmp_icon_ok, NULL, NULL, &bmp_bottom_right_confirm, NULL, NULL, NULL,
        NULL, NULL, "Device reset complete,", "restart now!", NULL);
  }
  while (1) {
    key = protectWaitKey(0, 1);
    if (key == KEY_CONFIRM) {
      break;
    }
  }
#if !EMULATOR
  svc_system_reset();
  // reset_to_firmware();
#endif
}

void menu_changePin(int index) {
  (void)index;
  uint8_t key = KEY_NULL;

  layoutDialogAdapterEx(_("Change PIN"), &bmp_bottom_left_arrow, NULL,
                        &bmp_bottom_right_arrow, NULL,
                        _("Before start, verify your \ncurrent PIN."), NULL,
                        NULL, NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }
  protectChangePinOnDevice(true, false, true);
}

void menu_showMnemonic(int index) {
  (void)index;
  if (protectPinOnDevice(false, true)) {
    char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
    config_getMnemonic(mnemonic, sizeof(mnemonic));
    scroll_mnemonic(_("Mnemonic"), mnemonic, 0);
  }
}

void menu_set_passphrase(int index) {
  (void)index;

  uint8_t key = KEY_NULL;

  if (index) {
    layoutDialogAdapterEx(
        _("Disable Passphrase"), &bmp_bottom_left_close, NULL,
        &bmp_bottom_right_confirm, NULL, NULL,
        _("Do you really want to \ndisable passphrase protection?"), NULL, NULL,
        NULL);
  } else {
    layoutDialogAdapterEx(
        _("Enable Passphrase"), &bmp_bottom_left_close, NULL,
        &bmp_bottom_right_confirm, NULL, NULL,
        _("Do you really want to \nenable passphrase protection?"), NULL, NULL,
        NULL);
  }

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }

  config_setPassphraseProtection(index ? false : true);
}

static struct menu_item ble_set_menu_items[] = {
    {"Enable", NULL, true, menu_para_set_ble, NULL, true},
    {"Disable", NULL, true, menu_para_set_ble, NULL, true}};

static struct menu ble_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(ble_set_menu_items),
    .title = "Bluetooth",
    .items = ble_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item language_set_menu_items[] = {
    {"English", NULL, true, menu_para_set_language, NULL, true},
    {"简体中文", NULL, true, menu_para_set_language, NULL, true}};

static struct menu language_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(language_set_menu_items),
    .title = "Language",
    .items = language_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item autolock_set_menu_items[] = {
    {"1", "minute", true, menu_para_set_sleep, NULL, true},
    {"2", "minutes", true, menu_para_set_sleep, NULL, true},
    {"5", "minutes", true, menu_para_set_sleep, NULL, true},
    {"10", "minutes", true, menu_para_set_sleep, NULL, true},
    {"Never", NULL, true, menu_para_set_sleep, NULL, true}};

static struct menu_item autolock_set_menu_items_added_custom[] = {
    {"1", "minute", true, menu_para_set_sleep, NULL, true},
    {"2", "minutes", true, menu_para_set_sleep, NULL, true},
    {"5", "minutes", true, menu_para_set_sleep, NULL, true},
    {"10", "minutes", true, menu_para_set_sleep, NULL, true},
    {"Never", NULL, true, menu_para_set_sleep, NULL, true},
    {"Custom", NULL, false, NULL, NULL, true}};

static struct menu autolock_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(autolock_set_menu_items),
    .title = "Auto-Lock",
    .items = autolock_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item shutdown_set_menu_items[] = {
    {"1", "minute", true, menu_para_set_shutdown, NULL, true},
    {"3", "minutes", true, menu_para_set_shutdown, NULL, true},
    {"5", "minutes", true, menu_para_set_shutdown, NULL, true},
    {"10", "minutes", true, menu_para_set_shutdown, NULL, true},
    {"Never", NULL, true, menu_para_set_shutdown, NULL, true}};

static struct menu shutdown_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(shutdown_set_menu_items),
    .title = "Shutdown",
    .items = shutdown_set_menu_items,
    .previous = &settings_menu,
};

static struct menu_item passphrase_set_menu_items[] = {
    {"Enable", NULL, true, menu_set_passphrase, NULL, true},
    {"Disable", NULL, true, menu_set_passphrase, NULL, true}};

static struct menu passphrase_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(passphrase_set_menu_items),
    .title = "Passphrase",
    .items = passphrase_set_menu_items,
    .previous = &security_set_menu,
};

static struct menu_item settings_menu_items[] = {
    {"Bluetooth", NULL, false, .sub_menu = &ble_set_menu, menu_para_ble_state,
     false},
    {"Language", NULL, false, .sub_menu = &language_set_menu,
     menu_para_language, false},
    {"Auto-Lock", NULL, false, .sub_menu = &autolock_set_menu,
     menu_para_autolock, false},
    {"Shutdown", NULL, false, .sub_menu = &shutdown_set_menu,
     menu_para_shutdown, false}};

static struct menu settings_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(settings_menu_items),
    .title = NULL,
    .items = settings_menu_items,
    .previous = &main_menu,
    .button_type = BTN_TYPE_NEXT,
};

void menu_check_all_words(int index) {
  (void)index;
  char desc[64] = "";
  uint8_t key = KEY_NULL;
  uint32_t word_count = 0;

refresh_menu:
  layoutDialogAdapterEx(_("Check Recovery Phrase"), &bmp_bottom_left_arrow,
                        NULL, &bmp_bottom_right_arrow, NULL,
                        _("Check your Recovery \nPhrase backup, make sure \nit "
                          "is exactly the same as \nthe one stored on device."),
                        NULL, NULL, NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return;
  }

  if (protectPinOnDevice(false, true)) {
    char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
    memset(desc, 0, sizeof(desc));
    config_getMnemonic(mnemonic, sizeof(mnemonic));

    if (!protectSelectMnemonicNumber(&word_count, true)) {
      goto refresh_menu;
    }
    if (word_count == 12)
      strcat(desc, _("Enter your 12-words  \nRecovery Phrase in order."));
    else if (word_count == 18)
      strcat(desc, _("Enter your 18-words  \nRecovery Phrase in order."));
    else if (word_count == 24)
      strcat(desc, _("Enter your 24-words  \nRecovery Phrase in order."));
    else
      return;

    layoutDialogAdapterEx(_("Enter Recovery Phrase"), &bmp_bottom_left_arrow,
                          NULL, &bmp_bottom_right_arrow, NULL, desc, NULL, NULL,
                          NULL, NULL);
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

static struct menu_item security_set_menu_items[] = {
    {"Change PIN", NULL, true, menu_changePin, NULL, false},
    {"Check Recovery Phrase", NULL, true, menu_check_all_words, NULL, false},
    {"Passphrase", NULL, false, .sub_menu = &passphrase_set_menu,
     menu_para_passphrase, true},
    {"Reset Device", NULL, true, menu_erase_device, NULL, false},
};

static struct menu security_set_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(security_set_menu_items),
    .title = NULL,
    .items = security_set_menu_items,
    .previous = &main_menu,
    .button_type = BTN_TYPE_NEXT,
};

void menu_set_trezor_compatibility(int index) {
  (void)index;

  uint8_t key = KEY_NULL;

  if (index) {
    layoutDialogAdapterEx(_("Restore Trezor Compat"), &bmp_bottom_left_close,
                          NULL, &bmp_bottom_right_confirm, NULL, NULL,
                          _("It will take effect after \ndevice restart."),
                          NULL, NULL, NULL);

    while (1) {
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM) {
        break;
      }
      if (key == KEY_CANCEL) {
        return;
      }
    }
  } else {
  _layout_disable:
    layoutDialogAdapterEx(
        _("Disable Trezor Compat"), &bmp_bottom_left_close, NULL,
        &bmp_bottom_right_arrow, NULL,
        _("This will prevent you from \nusing third-party wallet \nclients and "
          "websites which \nonly support Trezor."),
        NULL, NULL, NULL, NULL);
    while (1) {
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM) {
        break;
      }
      if (key == KEY_CANCEL) {
        return;
      }
    }

    layoutDialogAdapterEx(_("Disable Trezor Compat"), &bmp_bottom_left_arrow,
                          NULL, &bmp_bottom_right_confirm, NULL, NULL,
                          _("It will take effect after \ndevice restart."),
                          NULL, NULL, NULL);
    while (1) {
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM) {
        break;
      }
      if (key == KEY_CANCEL) {
        goto _layout_disable;
      }
    }

    layoutDialogSwipeCenterAdapter(
        &bmp_icon_warning, &bmp_bottom_left_close, NULL,
        &bmp_bottom_right_confirm, NULL, NULL, NULL, NULL, NULL, _("WARNING"),
        _("Do not change this setting"), _("if you not sure."));

    while (1) {
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM) {
        break;
      }
      if (key == KEY_CANCEL) {
        return;
      }
    }
  }

  config_setTrezorCompMode(index ? false : true);
#if !EMULATOR
  reset_to_firmware();
#endif
}

// static struct menu_item trezor_compatibility_set_menu_items[] = {
//     {"Enable", NULL, true, menu_set_trezor_compatibility, NULL, true},
//     {"Disable", NULL, true, menu_set_trezor_compatibility, NULL, true}};

// static struct menu trezor_compatibility_set_menu = {
//     .start = 0,
//     .current = 0,
//     .counts = COUNT_OF(trezor_compatibility_set_menu_items),
//     .title = "Trezor Compatibility",
//     .items = trezor_compatibility_set_menu_items,
//     .previous = &about_menu,
// };

static struct menu_item about_menu_items[] = {
    {"Device Info", NULL, true, layoutDeviceParameters, NULL, false},
    {"Certification", NULL, true, layoutAboutCertifications, NULL, false},
    // {"Trezor Compat", NULL, false, .sub_menu =
    // &trezor_compatibility_set_menu,
    //  menu_para_trezor_comp_mode_state, true},
};

static struct menu about_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(about_menu_items),
    .title = NULL,
    .items = about_menu_items,
    .previous = &main_menu,
    .button_type = BTN_TYPE_NEXT,
};

static struct menu_item main_menu_items[] = {
    {"Settings", NULL, false, .sub_menu = &settings_menu, NULL, false},
    {"Security", NULL, false, .sub_menu = &security_set_menu, NULL, false},
    {"About Device", NULL, false, .sub_menu = &about_menu, NULL, false}};

static struct menu main_menu = {
    .start = 0,
    .current = 0,
    .counts = COUNT_OF(main_menu_items),
    .title = NULL,
    .items = main_menu_items,
    .previous = NULL,
    .button_type = BTN_TYPE_NEXT,
};

void menu_autolock_added_custom(void) {
  static char autolock_custom_name[32] = {0};
  uint32_t ms = config_getSleepDelayMs();
  if ((ms != 1 * 60 * 1000) && (ms != 2 * 60 * 1000) && (ms != 5 * 60 * 1000) &&
      (ms != 10 * 60 * 1000) && (ms != 0)) {
    autolock_set_menu.counts = COUNT_OF(autolock_set_menu_items_added_custom);
    autolock_set_menu.items = autolock_set_menu_items_added_custom;
    char *value = format_time(ms);
    memset(autolock_custom_name, 0, 32);
    strcat(autolock_custom_name, value);
    strcat(autolock_custom_name, _("(Custom)"));
    autolock_set_menu_items_added_custom[5].name = autolock_custom_name;
  } else {
    autolock_set_menu.counts = COUNT_OF(autolock_set_menu_items);
    autolock_set_menu.items = autolock_set_menu_items;
  }
}

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
  menu_autolock_added_custom();
  if (state) {
    menu_init(&main_menu);
    menu_update(&settings_menu, previous, &main_menu);
  }
}
