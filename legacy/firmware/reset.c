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
#include "font.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "memzero.h"
#include "messages.h"
#include "messages.pb.h"
#include "oled.h"
#include "protect.h"
#include "rng.h"
#include "se_hal.h"
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
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL, NULL,
                      _("Do you really want to"), _("create a new wallet?"),
                      NULL, NULL, NULL);

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
#if ONEKEY_MINI
    SHA256_CTX ctx = {0};

    sha256_Init(&ctx);
    sha256_Update(&ctx, int_entropy, 32);
    sha256_Update(&ctx, ext_entropy, len);
    sha256_Final(&ctx, int_entropy);

#else
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
    se_setNeedsBackup(true);

    memcpy(int_entropy, seed, 32);
#endif

#if !EMULATOR
    if (g_bSelectSEFlag) {
      se_setSeedStrength(strength);
    }
#endif

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
  uint32_t rand_list[3] = {0};
#if ONEKEY_MINI
  char *data[3];
#endif

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

#if ONEKEY_MINI
choose_word:
  layoutDialogSwipeCenterAdapterEx(NULL, &bmp_button_back, _("BACK"),
                                   &bmp_button_forward, _("NEXT"), NULL, true,
                                   NULL, NULL, NULL, NULL,
                                   _("Choose the correct\nwords based on "
                                     "the\nrecovery phrase\nyou've put down."),
                                   NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#else
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL,
      NULL, NULL, _("Select correct word below"), NULL, NULL);
#endif
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }

  rand_list[0] = random_uniform(word_count);
  do {
    rand_list[1] = random_uniform(word_count);
  } while (rand_list[1] == rand_list[0]);

  do {
    rand_list[2] = random_uniform(word_count);
  } while (rand_list[2] == rand_list[0] || rand_list[2] == rand_list[1]);

refresh_menu:
  i = 0;
  memzero(desc, sizeof(desc));
#if ONEKEY_MINI
  strcat(desc, _("Select word"));
#else
  strcat(desc, _("Select your"));
#endif
  strcat(desc, " #");
  uint2str(rand_list[index] + 1, desc + strlen(desc));
  strcat(desc, " ");
#if !ONEKEY_MINI
  strcat(desc, _("mnemonic"));
#endif

#if !ONEKEY_MINI
  layoutDialogSwipeCenterAdapter(NULL, &bmp_button_back, _("BACK"),
                                 &bmp_btn_confirm, _("Confirm"), NULL, NULL,
                                 NULL, desc, NULL, NULL, NULL);

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }
#endif
  selected = mnemonic_find_word(words[rand_list[index]]);
  words_order[0] = selected;
  do {
    words_order[1] = random_uniform(BIP39_WORDS);
  } while (words_order[1] == selected);

  do {
    words_order[2] = random_uniform(BIP39_WORDS);
  } while (words_order[2] == selected || words_order[2] == words_order[1]);

  random_order(words_order, 3);

select_word:
#if ONEKEY_MINI
  data[0] = (char *)mnemonic_get_word(words_order[0]);
  data[1] = (char *)mnemonic_get_word(words_order[1]);
  data[2] = (char *)mnemonic_get_word(words_order[2]);

  layoutItemsSelectAdapterAlign(NULL, NULL, &bmp_button_back,
                                &bmp_button_forward, _("BACK"), _("OK"), i + 1,
                                3, false, NULL, desc, NULL, data);
#else
  layoutItemsSelectAdapter(
      &bmp_btn_up, &bmp_btn_down, NULL, &bmp_btn_confirm, NULL, _("Okay"),
      i + 1, 3, NULL, NULL, mnemonic_get_word(words_order[i]),
      i > 0 ? mnemonic_get_word(words_order[i - 1]) : NULL,
      i < 2 ? mnemonic_get_word(words_order[i + 1]) : NULL);
#endif

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (i > 0) i--;
      goto select_word;
    case KEY_CANCEL:
#if ONEKEY_MINI
      if (index > 0) {
        index--;
        goto refresh_menu;
      } else {
        goto choose_word;
      }
#else
      goto refresh_menu;
#endif
    case KEY_DOWN:
      if (i < 2) i++;
      goto select_word;
    case KEY_CONFIRM:
      if (words_order[i] != selected) {
#if ONEKEY_MINI
      error_refresh:
        setRgbBitmap(true);
        layoutDialogSwipeCenterAdapterEx(
            &bmp_icon_forbid, NULL, NULL, &bmp_btn_retry, _("RETRY"), NULL,
            true, NULL, NULL, NULL, NULL, NULL, NULL,
            _("Incorrect recovery\nphrase, try again."), NULL, NULL, NULL, NULL,
            NULL);
#else
        layoutDialogSwipeCenterAdapter(
            &bmp_icon_error, NULL, NULL, &bmp_btn_retry, _("Retry"), NULL, NULL,
            NULL, NULL, _("Wrong mnemonic"), NULL, NULL);
#endif

        key = protectWaitKey(0, 1);
#if ONEKEY_MINI
        setRgbBitmap(false);
        if (key == KEY_CONFIRM) {
          goto refresh_menu;
        } else {
          goto error_refresh;
        }
#else
        if (key != KEY_CONFIRM) {
          return false;
        }
        goto refresh_menu;
#endif
      } else {
        index++;
        if (index == 3)
          break;
        else
          goto refresh_menu;
      }
    default:
      return false;
  }

#if ONEKEY_MINI
ok_refresh:
  setRgbBitmap(true);
  layoutDialogSwipeCenterAdapterEx(
      &bmp_icon_success, &bmp_button_back, NULL, &bmp_button_forward, _("NEXT"),
      NULL, true, NULL, NULL, NULL, NULL, NULL, NULL,
      _("Recovery Phrase\nverified"), NULL, NULL, NULL, NULL, NULL);
#else
  layoutDialogSwipeCenterAdapter(&bmp_icon_ok, &bmp_btn_back, _("Back"),
                                 &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
                                 NULL, _("Mnemonic verified pass"), NULL, NULL);
#endif

  key = protectWaitKey(0, 1);
#if ONEKEY_MINI
  setRgbBitmap(false);
  if (key != KEY_CONFIRM) {
    goto ok_refresh;
  }
#else
  if (key != KEY_CONFIRM) {
    return false;
  }
#endif

  memzero(words, sizeof(words));
  return true;
}

#if ONEKEY_MINI
bool show_mnemonic(char words[24][12], uint32_t word_count) {
  char desc[16] = "";
  uint8_t key = KEY_NULL;
  uint32_t i = 0, j = 0, pages = 0;
  const struct font_desc *font = find_cur_font();

  pages = word_count / WORD_PER_PAGE;
  pages += (word_count % WORD_PER_PAGE) ? 1 : 0;
  if (pages == 0) return false;

refresh_menu:
  oledClear_ex();

  switch (i) {
    case 0:
      for (j = 0; j < WORD_PER_PAGE; j++) {
        memzero(desc, sizeof(desc));
        uint2str(j + 1, desc);
        strcat(desc, ". ");
        strcat(desc, words[j]);
        if ((j + 1) > 9) {
          oledDrawStringAdapter(OLED_WIDTH / 4 - font->width,
                                (j + 2) * (font->pixel + 1), desc,
                                FONT_STANDARD);
        } else {
          oledDrawStringAdapter(OLED_WIDTH / 4, (j + 2) * (font->pixel + 1),
                                desc, FONT_STANDARD);
        }
      }
      oledDrawBitmap(OLED_WIDTH / 2, OLED_HEIGHT - 9, &bmp_btn_down);
      break;
    case 1:
      oledDrawBitmap(OLED_WIDTH / 2, 0, &bmp_btn_up);
      for (j = 0; j < WORD_PER_PAGE; j++) {
        if ((j + WORD_PER_PAGE) < word_count) {
          memzero(desc, sizeof(desc));
          uint2str(j + WORD_PER_PAGE + 1, desc);
          strcat(desc, ". ");
          strcat(desc, words[j + WORD_PER_PAGE]);
          oledDrawStringAdapter(OLED_WIDTH / 4 - font->width,
                                (j + 2) * (font->pixel + 1), desc,
                                FONT_STANDARD);
        }
      }
      if (pages > 2) {
        oledDrawBitmap(OLED_WIDTH / 2, OLED_HEIGHT - 9, &bmp_btn_down);
      } else {
        layoutButtonYesAdapter(_("Next"), &bmp_btn_forward);
      }
      break;
    case 2:
      oledDrawBitmap(OLED_WIDTH / 2, 0, &bmp_btn_up);
      for (j = 0; j < WORD_PER_PAGE; j++) {
        if ((j + WORD_PER_PAGE * 2) < word_count) {
          memzero(desc, sizeof(desc));
          uint2str(j + WORD_PER_PAGE * 2 + 1, desc);
          strcat(desc, ". ");
          strcat(desc, words[j + WORD_PER_PAGE * 2]);
          oledDrawStringAdapter(OLED_WIDTH / 4 - font->width,
                                (j + 2) * (font->pixel + 1), desc,
                                FONT_STANDARD);
        }
      }
      layoutButtonYesAdapter(_("Next"), &bmp_btn_forward);
      break;
    default:
      break;
  }

  oledRefresh();

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (i > 0) i--;
      goto refresh_menu;
    case KEY_DOWN:
      if (i < pages - 1) i++;
      goto refresh_menu;
    case KEY_CANCEL:
      if (i > 0) {
        i--;
        goto refresh_menu;
      } else {
        return false;
      }
    case KEY_CONFIRM:
      if (i == pages - 1) {
        break;
      } else {
        if (i < pages - 1) i++;
        goto refresh_menu;
      }
    default:
      break;
  }

  return true;
}
#endif

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
#if ONEKEY_MINI
    layoutDialogSwipeCenterAdapterFont(
        NULL, &bmp_btn_back, NULL, &bmp_btn_confirm, NULL, NULL,
        FONT_STANDARD | FONT_DOUBLE, NULL, NULL, desc, NULL, NULL, words[i]);
#else
    layoutDialogSwipeCenterAdapter(NULL, &bmp_btn_back, _("Prev"),
                                   &bmp_btn_confirm, _("Confirm"), NULL, NULL,
                                   NULL, desc, words[i], NULL, NULL);
#endif
  } else if (type == 1) {
#if ONEKEY_MINI
    return show_mnemonic(words, word_count);
#else
    memzero(desc, sizeof(desc));
    strcat(desc, " #");
    uint2str(i + 1, desc + strlen(desc));
    layoutItemsSelectAdapter(&bmp_btn_up, &bmp_btn_down, NULL, &bmp_btn_confirm,
                             NULL, _("Okay"), i + 1, word_count, NULL, desc,
                             words[i], i > 0 ? words[i - 1] : NULL,
                             i < word_count - 1 ? words[i + 1] : NULL);
#endif
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

bool writedown_mnemonic(const char *mnemonic) {
  uint8_t key = KEY_NULL;

#if !ONEKEY_MINI
  char desc[32] = "";
  strcat(desc, _("Check the written"));
  uint2str(words_count, desc + strlen(desc));
#endif

#if ONEKEY_MINI
  if (scroll_mnemonic(_("Word"), mnemonic, 0)) {
  write_mnemonic:
    layoutDialogSwipeCenterAdapterEx(
        NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
        true, NULL, NULL, NULL, NULL,
        _("Check the recovery\nphrase once more and\ncompare it to the\nbackup "
          "copy in your\nhand. Make sure the\nspelling and order\nare "
          "identical."),
        NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#else
  if (scroll_mnemonic(_("Seed Phrase"), mnemonic, 0)) {
    layoutDialogSwipeCenterAdapter(NULL, &bmp_btn_back, _("Back"),
                                   &bmp_btn_forward, _("Next"), NULL, NULL,
                                   NULL, desc, _("seed phrases"), NULL, NULL);
#endif
    key = protectWaitKey(0, 1);
    if (key != KEY_CONFIRM) {
      return false;
    }
  check_mnemonic:
    if (!scroll_mnemonic(NULL, mnemonic, 1)) {
      goto_check(write_mnemonic);
    }

#if ONEKEY_MINI
  verify_mnemonic:
#endif
    if (!verify_mnemonic(mnemonic)) {
      goto_check(check_mnemonic);
    }

#if !ONEKEY_MINI
    layoutDialogSwipeCenterAdapter(
        NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL,
        _("The seed phrases are the"), _("only way to recover your"),
        _("asset,Keep it safe"), NULL, NULL);
    key = protectWaitKey(0, 1);
    if (key != KEY_CONFIRM) {
      return false;
    }
#endif

    if (!protectChangePinOnDevice(false, true)) {
#if ONEKEY_MINI
      goto verify_mnemonic;
#else
      goto_check(check_mnemonic);
#endif
    }
    config_setMnemonic(mnemonic, false);
    return true;
  }
  return false;
}

bool reset_on_device(void) {
  char desc[64] = "";
  uint8_t key = KEY_NULL;

prompt_creat:
#if ONEKEY_MINI
  layoutDialogSwipeCenterAdapterEx(
      NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
      true, NULL, NULL, NULL, NULL, NULL,
      _("Follow the guide to\ncreate a new wallet"), NULL, NULL, NULL, NULL,
      NULL, NULL);
#else
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL,
      NULL, _("Follow the prompts"), _("to creat wallet"), NULL, NULL);
#endif

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }

  words_count = 0;

select_mnemonic_count:
  if (!protectSelectMnemonicNumber(&words_count)) {
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

#if ONEKEY_MINI
  if (words_count == 12)
    strcat(desc, _("Please copy the\nfollowing 12 words in\norder"));
  else if (words_count == 18)
    strcat(desc, _("Please copy the\nfollowing 18 words in\norder"));
  else
    strcat(desc, _("Please copy the\nfollowing 24 words in\norder"));

  layoutDialogSwipeCenterAdapterEx(NULL, &bmp_button_back, _("BACK"),
                                   &bmp_button_forward, _("NEXT"), NULL, true,
                                   NULL, NULL, NULL, NULL, desc, NULL, NULL,
                                   NULL, NULL, NULL, NULL, NULL);
#else
  strcat(desc, _("Write down your "));
  uint2str(words_count, desc + strlen(desc));
  layoutDialogSwipeCenterAdapter(NULL, &bmp_btn_back, _("Back"),
                                 &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
                                 desc, _("seed phrases"), NULL, NULL);
#endif

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    goto_check(select_mnemonic_count);
  }

#if !EMULATOR
  if (g_bSelectSEFlag) {
    uint8_t seed[64];
#if ONEKEY_MINI
    random_buffer(seed, 64);
#else
    if (!se_device_init(ExportType_MnemonicPlainExportType_YES, NULL))
      return false;
    if (!se_export_seed(seed)) return false;
    se_setNeedsBackup(true);
#endif
    memcpy(int_entropy, seed, 32);
    se_setSeedStrength(strength);
  } else
#endif
  {
    random_buffer(int_entropy, 32);
    SHA256_CTX ctx = {0};
    sha256_Init(&ctx);
    sha256_Update(&ctx, int_entropy, 32);
    sha256_Final(&ctx, int_entropy);
  }

  const char *mnemonic = mnemonic_from_data(int_entropy, strength / 8);
  memzero(int_entropy, 32);

  if (!writedown_mnemonic(mnemonic)) {
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
