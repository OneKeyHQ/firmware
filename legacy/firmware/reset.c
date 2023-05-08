/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "reset.h"
#include "bip39.h"
#include "buttons.h"
#include "common.h"
#include "config.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "memzero.h"
#include "messages.h"
#include "messages.pb.h"
#include "oled.h"
#include "protect.h"
#include "rng.h"
#include "se_chip.h"
#include "sha2.h"
#include "sys.h"
#include "util.h"

static uint32_t strength;
static uint8_t int_entropy[32];
static bool awaiting_entropy = false;
static bool skip_backup = false;
static bool no_backup = false;
static uint32_t words_count;

#define goto_check(label)       \
  if (layoutLast == layoutHome) \
    return false;               \
  else                          \
    goto label;

void reset_init(bool display_random, uint32_t _strength,
                bool passphrase_protection, bool pin_protection,
                const char *language, const char *label, uint32_t u2f_counter,
                bool _skip_backup, bool _no_backup) {
  if (_strength != 128 && _strength != 192 && _strength != 256) return;

  strength = _strength;
  skip_backup = _skip_backup;
  no_backup = _no_backup;

  if (display_random && (skip_backup || no_backup)) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    "Can't show internal entropy when backup is skipped");
    layoutHome();
    return;
  }

  if (!g_bIsBixinAPP) {
    layoutDialogAdapterEx(
        _("Create New Wallet"), &bmp_bottom_left_close, NULL,
        &bmp_bottom_right_arrow, NULL,
        _("Generating a standard\nwallet with a new set of\nrecovery phrase."),
        NULL, NULL, NULL, NULL);
    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }

  random_buffer(int_entropy, 32);

  if (display_random) {
    for (int start = 0; start < 2; start++) {
      char ent_str[4][17] = {0};
      char desc[] = "Internal entropy _/2:";
      data2hex(int_entropy + start * 16, 4, ent_str[0]);
      data2hex(int_entropy + start * 16 + 4, 4, ent_str[1]);
      data2hex(int_entropy + start * 16 + 8, 4, ent_str[2]);
      data2hex(int_entropy + start * 16 + 12, 4, ent_str[3]);
      layoutLast = layoutDialogSwipe;
      layoutSwipe();
      desc[17] = '1' + start;
      oledDrawStringCenter(OLED_WIDTH / 2, 0, desc, FONT_STANDARD);
      oledDrawStringCenter(OLED_WIDTH / 2, 2 + 1 * 9, ent_str[0], FONT_FIXED);
      oledDrawStringCenter(OLED_WIDTH / 2, 2 + 2 * 9, ent_str[1], FONT_FIXED);
      oledDrawStringCenter(OLED_WIDTH / 2, 2 + 3 * 9, ent_str[2], FONT_FIXED);
      oledDrawStringCenter(OLED_WIDTH / 2, 2 + 4 * 9, ent_str[3], FONT_FIXED);
      oledHLine(OLED_HEIGHT - 13);
      layoutButtonNoAdapter(_("Cancel"), &bmp_btn_cancel);
      layoutButtonYesAdapter(_("Continue"), &bmp_btn_confirm);
      // 40 is the maximum pixels used for a row
      oledSCA(2 + 1 * 9, 2 + 1 * 9 + 6, 40);
      oledSCA(2 + 2 * 9, 2 + 2 * 9 + 6, 40);
      oledSCA(2 + 3 * 9, 2 + 3 * 9 + 6, 40);
      oledSCA(2 + 4 * 9, 2 + 4 * 9 + 6, 40);
      oledRefresh();
      if (!protectButton(ButtonRequestType_ButtonRequest_ResetDevice, false)) {
        fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
        layoutHome();
        return;
      }
    }
  }
  if (pin_protection && !protectChangePin(false)) {
    layoutHome();
    return;
  }

  config_setPassphraseProtection(passphrase_protection);
  config_setLanguage(language);
  config_setLabel(label);
  config_setU2FCounter(u2f_counter);

  EntropyRequest resp = {0};
  memzero(&resp, sizeof(EntropyRequest));
  msg_write(MessageType_MessageType_EntropyRequest, &resp);
  awaiting_entropy = true;
}

void reset_entropy(const uint8_t *ext_entropy, uint32_t len) {
  if (!awaiting_entropy) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    _("Not in Reset mode"));
    return;
  }
  awaiting_entropy = false;

  if (g_bSelectSEFlag) {
    uint8_t seed[64];

    if (!se_device_init(ExportType_MnemonicPlainExportType_YES, NULL)) {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Device failed initialized"));
      layoutHome();
      return;
    }
    if (!se_export_seed(seed)) {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Device failed initialized"));
      layoutHome();
      return;
    }
    se_setSeedStrength(strength);
    se_setNeedsBackup(true);
    memcpy(int_entropy, seed, 32);
  } else {
    SHA256_CTX ctx = {0};
    sha256_Init(&ctx);
    sha256_Update(&ctx, int_entropy, 32);
    sha256_Update(&ctx, ext_entropy, len);
    sha256_Final(&ctx, int_entropy);
  }

  const char *mnemonic = mnemonic_from_data(int_entropy, strength / 8);
  memzero(int_entropy, 32);

  if (skip_backup || no_backup) {
    if (no_backup) {
      config_setNoBackup();
    } else {
      config_setNeedsBackup(true);
    }
    if (config_setMnemonic(mnemonic, false)) {
      fsm_sendSuccess(_("Device successfully initialized"));
    } else {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Failed to store mnemonic"));
    }
    layoutHome();
  } else {
    reset_backup(false, mnemonic);
  }
  mnemonic_clear();
}

static char current_word[10];

// separated == true if called as a separate workflow via BackupMessage
void reset_backup(bool separated, const char *mnemonic) {
  if (separated) {
    bool needs_backup = false;
    config_getNeedsBackup(&needs_backup);
    if (!needs_backup) {
      fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                      _("Seed already backed up"));
      return;
    }

    config_setUnfinishedBackup(true);
    config_setNeedsBackup(false);
  }

  for (int pass = 0; pass < 2; pass++) {
    int i = 0, word_pos = 1;
    while (mnemonic[i] != 0) {
      // copy current_word
      int j = 0;
      while (mnemonic[i] != ' ' && mnemonic[i] != 0 &&
             j + 1 < (int)sizeof(current_word)) {
        current_word[j] = mnemonic[i];
        i++;
        j++;
      }
      current_word[j] = 0;
      if (mnemonic[i] != 0) {
        i++;
      }
      layoutResetWord(current_word, pass, word_pos, mnemonic[i] == 0);
      if (!protectButton(ButtonRequestType_ButtonRequest_ConfirmWord, false)) {
        if (!separated) {
          session_clear(true);
        }
        layoutHome();
        fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
        return;
      }
      word_pos++;
    }
  }

  config_setUnfinishedBackup(false);

  if (separated) {
    fsm_sendSuccess(_("Seed successfully backed up"));
  } else {
    config_setNeedsBackup(false);
    if (config_setMnemonic(mnemonic, false)) {
      fsm_sendSuccess(_("Device successfully initialized"));
    } else {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Failed to store mnemonic"));
    }
  }
  layoutHome();
}

void random_order(uint32_t *str, size_t len) {
  for (int i = len - 1; i >= 1; i--) {
    int j = random_uniform(i + 1);
    uint32_t t = str[j];
    str[j] = str[i];
    str[i] = t;
  }
}

bool verify_mnemonic(const char *mnemonic) {
  uint8_t key = KEY_NULL;
  char desc[64] = "";
  char words[24][12];
  uint32_t words_order[3];
  uint32_t i = 0, word_count = 0;
  uint32_t index = 0, selected = 0;

  memzero(words, sizeof(words));
  while (mnemonic[i] != 0) {
    // copy current_word
    int j = 0;
    while (mnemonic[i] != ' ' && mnemonic[i] != 0 &&
           j + 1 < (int)sizeof(words[word_count])) {
      words[word_count][j] = mnemonic[i];
      i++;
      j++;
    }
    current_word[j] = 0;
    word_count++;
    if (mnemonic[i] != 0) {
      i++;
    }
  }

  layoutDialogAdapterEx(
      _("Check Recovery Phrase"), &bmp_bottom_left_arrow, NULL,
      &bmp_bottom_right_arrow, NULL,
      _("Next, follow the guide and\ncheck words one by one."), NULL, NULL,
      NULL, NULL);

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }

refresh_menu:
  i = 0;
  memzero(desc, sizeof(desc));
  strcat(desc, _("Check Word "));
  strcat(desc, " #");
  uint2str(index + 1, desc + strlen(desc));

  selected = mnemonic_find_word(words[index]);
  words_order[0] = selected;
  do {
    words_order[1] = random_uniform(BIP39_WORD_COUNT);
  } while (words_order[1] == selected);

  do {
    words_order[2] = random_uniform(BIP39_WORD_COUNT);
  } while (words_order[2] == selected || words_order[2] == words_order[1]);

  random_order(words_order, 3);

select_word:
  layoutItemsSelectAdapterWords(
      &bmp_bottom_middle_arrow_up, &bmp_bottom_middle_arrow_down, NULL,
      &bmp_bottom_right_arrow, NULL, NULL, i + 1, 3, desc,
      mnemonic_get_word(words_order[i]), mnemonic_get_word(words_order[i]),
      i > 0 ? mnemonic_get_word(words_order[i - 1]) : NULL,
      i > 1 ? mnemonic_get_word(words_order[i - 2]) : NULL, NULL,
      i < 2 ? mnemonic_get_word(words_order[i + 1]) : NULL,
      i < 1 ? mnemonic_get_word(words_order[i + 2]) : NULL, NULL, false, true);

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (i > 0) i--;
      goto select_word;
    case KEY_CANCEL:
      goto select_word;
    case KEY_DOWN:
      if (i < 2) i++;
      goto select_word;
    case KEY_CONFIRM:
      if (words_order[i] != selected) {
        layoutDialogSwipeCenterAdapter(
            &bmp_icon_error, NULL, NULL, &bmp_bottom_right_retry, NULL, NULL,
            NULL, NULL, NULL, _("Incorrect word! check your"),
            _("backup and try again."), NULL);

        key = protectWaitKey(0, 1);
        if (key != KEY_CONFIRM) {
          return false;
        }
        goto refresh_menu;
      } else {
        index++;
        if (index == word_count)
          break;
        else
          goto refresh_menu;
      }
    default:
      return false;
  }

  layoutDialogSwipeCenterAdapter(
      &bmp_icon_ok, NULL, NULL, &bmp_bottom_right_arrow, NULL, NULL, NULL, NULL,
      NULL, _("Awesome!"), _("Your backup is complete."), NULL);

  while (1) {
    key = protectWaitKey(0, 1);
    if (key == KEY_CONFIRM) {
      break;
    }
  }

  memzero(words, sizeof(words));
  return true;
}

bool scroll_mnemonic(const char *pre_desc, const char *mnemonic, uint8_t type) {
  uint8_t key = KEY_NULL;
  char desc[64] = "";
  char words[24][12];
  uint32_t pages, i = 0, index = 0, word_count = 0;

  memzero(words, sizeof(words));
  while (mnemonic[i] != 0) {
    // copy current_word
    int j = 0;
    while (mnemonic[i] != ' ' && mnemonic[i] != 0 &&
           j + 1 < (int)sizeof(words[word_count])) {
      words[word_count][j] = mnemonic[i];
      i++;
      j++;
    }
    current_word[j] = 0;
    word_count++;
    if (mnemonic[i] != 0) {
      i++;
    }
  }
  i = 0;
  pages = word_count / 6;
refresh_menu:
  if (type == 0) {
    memzero(desc, sizeof(desc));
    strcat(desc, pre_desc);
    strcat(desc, " #");
    uint2str(i + 1, desc + strlen(desc));

    if (i == 0) {
      layoutItemsSelectAdapterWords(
          &bmp_bottom_middle_arrow_up, &bmp_bottom_middle_arrow_down,
          &bmp_bottom_left_close, &bmp_bottom_right_arrow, NULL, NULL, 1, 1,
          desc, words[i], words[i], NULL, NULL, NULL, NULL, NULL, NULL, false,
          false);
    } else {
      layoutItemsSelectAdapterWords(
          &bmp_bottom_middle_arrow_up, &bmp_bottom_middle_arrow_down,
          &bmp_bottom_left_arrow, &bmp_bottom_right_arrow, NULL, NULL, 1, 1,
          desc, words[i], words[i], NULL, NULL, NULL, NULL, NULL, NULL, false,
          false);
    }
  } else if (type == 1) {
    memzero(desc, sizeof(desc));
    strcat(desc, _("Recovery Phrase "));
    if (index == 0) {
      strcat(desc, "(1-6)");
    } else if (index == 1) {
      strcat(desc, "(7-12)");
    } else if (index == 2) {
      strcat(desc, "(13-18)");
    } else {
      strcat(desc, "(19-24)");
    }
    if (index == pages - 1) {
      layoutWords(desc, &bmp_bottom_middle_arrow_up,
                  &bmp_bottom_middle_arrow_down, NULL, &bmp_bottom_right_arrow,
                  index + 1, pages, words[index * 6 + 0], words[index * 6 + 1],
                  words[index * 6 + 2], words[index * 6 + 3],
                  words[index * 6 + 4], words[index * 6 + 5]);
    } else {
      layoutWords(
          desc, &bmp_bottom_middle_arrow_up, &bmp_bottom_middle_arrow_down,
          NULL, &bmp_bottom_right_arrow_off, index + 1, pages,
          words[index * 6 + 0], words[index * 6 + 1], words[index * 6 + 2],
          words[index * 6 + 3], words[index * 6 + 4], words[index * 6 + 5]);
    }
  }
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) index--;
      goto refresh_menu;
    case KEY_CANCEL:
      if (type == 0) {
        if (i > 0)
          i--;
        else
          return false;
      }
      goto refresh_menu;
    case KEY_DOWN:
      if (index < pages - 1) index++;
      goto refresh_menu;
    case KEY_CONFIRM:
      if (type == 1) {
        if (index == pages - 1) {
          return true;
        } else {
          index++;
        }
      }
      if (i == word_count - 1)
        return true;
      else {
        i++;
        goto refresh_menu;
      }
    default:
      break;
  }

  memzero(words, sizeof(words));
  return false;
}

bool writedown_mnemonic(const char *mnemonic, uint32_t count) {
  uint8_t key = KEY_NULL;
  char desc[63] = "";
  strcat(desc, _("Next, check the written "));
  uint2str(count, desc + strlen(desc));
  strcat(desc, _(" words again."));
write_mnemonic:
  if (scroll_mnemonic(_("Word"), mnemonic, 0)) {
  check_words_again:
    layoutDialogAdapterEx(_("Check Words Again"), &bmp_bottom_left_close, NULL,
                          &bmp_bottom_right_arrow, NULL, desc, NULL, NULL, NULL,
                          NULL);
    while (1) {
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM || key == KEY_CANCEL) {
        break;
      }
    }
    if (key != KEY_CONFIRM) {
      layoutDialogAdapterEx(
          _("Abort Backup?"), &bmp_bottom_left_close, NULL,
          &bmp_bottom_right_confirm, NULL,
          _("Are you sure to abort this\nprocess? All progress\nwill be lost."),
          NULL, NULL, NULL, NULL);
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM) {
        return false;
      } else {
        goto check_words_again;
      }
    }
  check_mnemonic:
    if (!scroll_mnemonic(NULL, mnemonic, 1)) {
      goto_check(write_mnemonic);
    }
    if (!verify_mnemonic(mnemonic)) {
      goto_check(check_mnemonic);
    }
    layoutDialogAdapterEx(_("Almost Done!"), NULL, NULL,
                          &bmp_bottom_right_arrow, NULL,
                          _("Recovery phrase is the \nonly way to recover "
                            "your\nassets. So keep it in a\nsafe place."),
                          NULL, NULL, NULL, NULL);
    while (1) {
      key = protectWaitKey(0, 1);
      if (key == KEY_CONFIRM) {
        break;
      }
    }
    if (!protectChangePinOnDevice(true, true, false)) {
      goto_check(check_mnemonic);
    }

    config_setMnemonic(mnemonic, false);
    return true;
  }
  return false;
}

bool reset_on_device(void) {
  char desc[128] = "";
  uint8_t key = KEY_NULL;

  if (config_hasPin()) {
    uint8_t ui_language_bak = ui_language;
    const char *lang[2] = {"en-US", "zh-CN"};
    config_wipe();
    ui_language = ui_language_bak;
    config_setLanguage(lang[ui_language]);
  }
prompt_creat:
  layoutDialogAdapterEx(
      _("Create New Wallet"), &bmp_bottom_left_close, NULL,
      &bmp_bottom_right_arrow, NULL,
      _("Generating a standard\nwallet with a new set of\nrecovery phrase."),
      NULL, NULL, NULL, NULL);

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }

select_mnemonic_count:
  words_count = 0;
  if (!protectSelectMnemonicNumber(&words_count, true)) {
    goto_check(prompt_creat);
  }
  switch (words_count) {
    case 12:
      strength = 128;
      break;
    case 18:
      strength = 192;
      break;
    case 24:
      strength = 256;
      break;
    default:
      return false;
  }

  memzero(desc, sizeof(desc));
  strcat(desc, _("The next screen will start\ndisplay "));
  uint2str(words_count, desc + strlen(desc));
  strcat(
      desc,
      _(" words called\nRecovery Phrase. Write it\ndown on sheet in order."));

  layoutDialogAdapterEx(_("Back Up Recovery Phrase"), &bmp_bottom_left_arrow,
                        NULL, &bmp_bottom_right_arrow, NULL, desc, NULL, NULL,
                        NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    goto_check(select_mnemonic_count);
  }

  if (g_bSelectSEFlag) {
    uint8_t seed[64];

    if (!se_device_init(ExportType_MnemonicPlainExportType_YES, NULL))
      return false;

    if (!se_export_seed(seed)) return false;
    memcpy(int_entropy, seed, 32);
    se_setSeedStrength(strength);
    se_setNeedsBackup(true);

  } else {
    random_buffer(int_entropy, 32);
    SHA256_CTX ctx = {0};
    sha256_Init(&ctx);
    sha256_Update(&ctx, int_entropy, 32);
    sha256_Final(&ctx, int_entropy);
  }

  const char *mnemonic = mnemonic_from_data(int_entropy, strength / 8);
  memzero(int_entropy, 32);

  if (!writedown_mnemonic(mnemonic, words_count)) {
    goto_check(select_mnemonic_count);
  }

  mnemonic_clear();

  return true;
}

#if DEBUG_LINK

uint32_t reset_get_int_entropy(uint8_t *entropy) {
  memcpy(entropy, int_entropy, 32);
  return 32;
}

const char *reset_get_word(void) { return current_word; }

#endif
