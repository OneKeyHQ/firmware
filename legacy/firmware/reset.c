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
static bool reset_byself = false;
static uint32_t words_count;

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
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("create a new wallet?"),
                      _("By continuing you"), _("agree to trezor.io/tos"), NULL,
                      NULL);

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

  SHA256_CTX ctx = {0};
  sha256_Init(&ctx);
  sha256_Update(&ctx, int_entropy, 32);
  sha256_Update(&ctx, ext_entropy, len);
  sha256_Final(&ctx, int_entropy);
  const char *mnemonic = mnemonic_from_data(int_entropy, strength / 8);
  memzero(int_entropy, 32);

  if (!g_bSelectSEFlag) {
    if (skip_backup || no_backup) {
      if (no_backup) {
        config_setNoBackup();
      } else {
        config_setNeedsBackup(true);
      }
      if (config_setMnemonic(mnemonic)) {
        fsm_sendSuccess(_("Device successfully initialized"));
      } else {
        fsm_sendFailure(FailureType_Failure_ProcessError,
                        _("Failed to store mnemonic"));
      }
      layoutHome();
    } else {
      if (!reset_byself)
        reset_backup(false, mnemonic);
      else
        writedown_mnemonic(mnemonic);
    }
  } else {
    if (skip_backup || no_backup) {
      if (no_backup) {
        config_setNoBackup();
      } else {
        config_setNeedsBackup(true);
      }
    }
    char passphrase[MAX_PASSPHRASE_LEN + 1] = {0};
    if (!protectPassphrase(passphrase)) {
      if (!se_device_init(ExportType_SeedEncExportType_YES, NULL)) {
        fsm_sendFailure(FailureType_Failure_ProcessError,
                        _("Device failed initialized"));
        layoutHome();
        return;
      }
    } else {
      if (!se_device_init(ExportType_SeedEncExportType_YES, passphrase)) {
        fsm_sendFailure(FailureType_Failure_ProcessError,
                        _("Device failed initialized"));
        layoutHome();
        return;
      }
    }
    fsm_sendSuccess(_("Device successfully initialized"));
    layoutHome();
    return;
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
    if (config_setMnemonic(mnemonic)) {
      fsm_sendSuccess(_("Device successfully initialized"));
    } else {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Failed to store mnemonic"));
    }
  }
  layoutHome();
}

bool scroll_mnemonic(const char *pre_desc, const char *mnemonic, uint8_t type) {
  uint8_t key = KEY_NULL;
  char desc[64] = "";
  char words[24][12];
  uint32_t i = 0, word_count = 0;

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
refresh_menu:
  if (type == 0) {
    memzero(desc, sizeof(desc));
    strcat(desc, pre_desc);
    strcat(desc, " #");
    uint2str(i + 1, desc + strlen(desc));
    layoutDialogSwipeCenterAdapter(NULL, &bmp_btn_back, _("Prev"),
                                   &bmp_btn_confirm, _("Confirm"), NULL, NULL,
                                   NULL, desc, words[i], NULL, NULL);
  } else if (type == 1) {
    memzero(desc, sizeof(desc));
    strcat(desc, " #");
    uint2str(i + 1, desc + strlen(desc));
    layoutItemsSelectAdapter(&bmp_btn_up, &bmp_btn_down, NULL, &bmp_btn_confirm,
                             NULL, _("Okay"), i + 1, word_count, NULL, desc,
                             words[i], i > 0 ? words[i - 1] : NULL,
                             i < word_count - 1 ? words[i + 1] : NULL);
  }
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (i > 0) i--;
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
      if (i < word_count - 1) i++;
      goto refresh_menu;
    case KEY_CONFIRM:
      if (i == word_count - 1)
        break;
      else {
        i++;
        goto refresh_menu;
      }
    default:
      break;
  }

  memzero(words, sizeof(words));
  return true;
}

void writedown_mnemonic(const char *mnemonic) {
  uint8_t key = KEY_NULL;
  char desc[32] = "";
  strcat(desc, _("Please check the written"));
  uint2str(words_count, desc + strlen(desc));
  if (scroll_mnemonic(_("Seed Phrase"), mnemonic, 0)) {
    layoutDialogSwipeCenterAdapter(NULL, &bmp_btn_back, _("Back"),
                                   &bmp_btn_forward, _("Next"), NULL, NULL,
                                   NULL, desc, _("seed phrases"), NULL, NULL);
    key = protectWaitKey(0, 1);
    if (key != KEY_CONFIRM) {
      return;
    }
    if (scroll_mnemonic(NULL, mnemonic, 1)) {
      layoutDialogSwipeCenterAdapter(
          NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL,
          NULL, _("The seed phrases are the"), _("only way to recover your"),
          _("asset,Keep it safe"), NULL, NULL);
      key = protectWaitKey(0, 1);
      if (key != KEY_CONFIRM) {
        return;
      }
      if (protectChangePinOnDevice(false)) {
        config_setMnemonic(mnemonic);
      }
    }
  }
}

bool reset_on_device(void) {
  char desc[64] = "";
  uint8_t key = KEY_NULL;
  words_count = 0;
refresh_menu:
  if (!protectSelectMnemonicNumber(&words_count)) return false;
  switch (words_count) {
    case 12:
      strength = 128;
      break;
    case 18:
      strength = 196;
      break;
    case 24:
      strength = 256;
      break;
    default:
      return false;
  }

  memzero(desc, sizeof(desc));
  strcat(desc, _("Write down your "));
  uint2str(words_count, desc + strlen(desc));

  layoutDialogSwipeCenterAdapter(NULL, &bmp_btn_back, _("Prev"),
                                 &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
                                 desc, _("seed phrases"), NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key == KEY_CANCEL) {
    goto refresh_menu;
  } else if (key == KEY_NULL) {
    return false;
  }

  skip_backup = false;
  no_backup = false;
  awaiting_entropy = true;
  reset_byself = true;

  random_buffer(int_entropy, 32);
  reset_entropy(0, 0);

  reset_byself = false;
  return true;
}

#if DEBUG_LINK

uint32_t reset_get_int_entropy(uint8_t *entropy) {
  memcpy(entropy, int_entropy, 32);
  return 32;
}

const char *reset_get_word(void) { return current_word; }

#endif
