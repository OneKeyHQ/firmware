/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 * Copyright (C) 2016 Jochen Hoenicke <hoenicke@gmail.com>
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

#include "recovery.h"
#include <ctype.h>
#include "bip39.h"
#include "buttons.h"
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
#include "recovery-table.h"
#include "rng.h"
#include "timer.h"
#include "usb.h"
#include "util.h"

/* number of words expected in the new seed */
static uint32_t word_count;

/* recovery mode:
 * 0: not recovering
 * 1: recover by scrambled plain text words
 * 2: recover by matrix entry
 */
static int awaiting_word = 0;

/* True if we should not write anything back to config
 * (can be used for testing seed for correctness).
 */
static bool dry_run;

/* True if we should check that seed corresponds to bip39.
 */
static bool enforce_wordlist;

/* For scrambled recovery Trezor may ask for faked words if
 * seed is short.  This contains the fake word.
 */
static char fake_word[12];

/* Word position in the seed that we are currently asking for.
 * This is 0 if we ask for a fake word.  Only for scrambled recovery.
 */
static uint32_t word_pos;

/* Scrambled recovery:  How many words has the user already entered.
 * Matrix recovery: How many digits has the user already entered.
 */
static uint32_t word_index;

/* Order in which we ask for the words.  It holds that
 * word_order[word_index] == word_pos.  Only for scrambled recovery.
 */
static char word_order[24];

/* The recovered seed.  This is filled during the recovery process.
 */
static char words[24][12];

/* The "pincode" of the current word.  This is basically the "pin"
 * that the user would have entered for the current word if the words
 * were displayed in alphabetical order.  Note that it is base 9, not
 * base 10.  Only for matrix recovery.
 */
static uint16_t word_pincode;

/* The pinmatrix currently displayed on screen.
 * Only for matrix recovery.
 */
static uint8_t word_matrix[9];

static bool recovery_byself = false;

/* The words are stored in two tables.
 *
 * The low bits of the first table (TABLE1) store the index into the
 * second table, for each of the 81 choices for the first two levels
 * of the matrix.  The final entry points to the final entry of the
 * second table.  The difference TABLE1(idx+1)-TABLE1(idx) gives the
 * number of choices for the third level.  The value
 * TABLE2(TABLE1(idx)) gives the index of the first word in the range
 * and TABLE2(TABLE1(idx+1))-1 gives the index of the last word.
 *
 * The low bits of the second table (TABLE2) store the index into the
 * word list for each of the choices for the first three levels.  The
 * final entry stores the value 2048 (number of bip39 words).  table.
 * The difference TABLE2(idx+1)-TABLE2(idx) gives the number of
 * choices for the last level.  The value TABLE2(idx) gives the index
 * of the first word in the range and TABLE2(idx)-1 gives the index of
 * the last word.
 *
 * The high bits in each table is the "prefix length", i.e. the number
 * of significant letters for the corresponding choice.  There is no
 * prefix length or table for the very first level, as the prefix length
 * is always one and there are always nine choices on the second level.
 */
#define MASK_IDX(x) ((x)&0xfff)
#define TABLE1(x) MASK_IDX(word_table1[x])
#define TABLE2(x) MASK_IDX(word_table2[x])

#define goto_check(label)       \
  if (layoutLast == layoutHome) \
    return false;               \
  else                          \
    goto label;

/* Helper function to format a two digit number.
 * Parameter dest is buffer containing the string. It should already
 * start with "##th".  The number is written in place.
 * Parameter number gives the number that we should format.
 */
static void format_number(char *dest, int number) {
  strcat(dest, _("##th"));
  if (dest[0] == '#') {
    if (number < 10) {
      dest[0] = ' ';
    } else {
      dest[0] = '0' + number / 10;
    }
    dest[1] = '0' + number % 10;
    if (number == 1 || number == 21) {
      dest[2] = 's';
      dest[3] = 't';
    } else if (number == 2 || number == 22) {
      dest[2] = 'n';
      dest[3] = 'd';
    } else if (number == 3 || number == 23) {
      dest[2] = 'r';
      dest[3] = 'd';
    }
  } else {
    if (number < 10) {
      dest[3] = ' ';
    } else {
      dest[3] = '0' + number / 10;
    }
    dest[4] = '0' + number % 10;
  }
  strcat(dest, _("word"));
}

/* Send a request for a new word/matrix code to the PC.
 */
static void recovery_request(void) {
  WordRequest resp = {0};
  memzero(&resp, sizeof(WordRequest));
  resp.has_type = true;
  resp.type = awaiting_word == 1      ? WordRequestType_WordRequestType_Plain
              : (word_index % 4 == 3) ? WordRequestType_WordRequestType_Matrix6
                                      : WordRequestType_WordRequestType_Matrix9;
  msg_write(MessageType_MessageType_WordRequest, &resp);
}

/* Called when the last word was entered.
 * Check mnemonic and send success/failure.
 */
static bool recovery_done(void) {
  bool success = false;
  char new_mnemonic[MAX_MNEMONIC_LEN + 1] = {0};

  strlcpy(new_mnemonic, words[0], sizeof(new_mnemonic));
  for (uint32_t i = 1; i < word_count; i++) {
    strlcat(new_mnemonic, " ", sizeof(new_mnemonic));
    strlcat(new_mnemonic, words[i], sizeof(new_mnemonic));
  }
  if (!enforce_wordlist || mnemonic_check(new_mnemonic)) {
    // New mnemonic is valid.
    if (!dry_run) {
      // Update mnemonic on config.
      if (config_setMnemonic(new_mnemonic, true)) {
        if (!enforce_wordlist) {
          // not enforcing => mark config as imported
          config_setImported(true);
        }
        if (!recovery_byself) {
          fsm_sendSuccess(_("Device recovered"));
        } else {
        }
        success = true;
      } else {
        fsm_sendFailure(FailureType_Failure_ProcessError,
                        _("Failed to store mnemonic"));
      }
      memzero(new_mnemonic, sizeof(new_mnemonic));
    } else {
      // Inform the user about new mnemonic correctness (as well as whether it
      // is the same as the current one).
      bool match =
          (config_isInitialized() && config_containsMnemonic(new_mnemonic));
      memzero(new_mnemonic, sizeof(new_mnemonic));
      if (match) {
#if ONEKEY_MINI
        layoutDialogAdapterEx(&bmp_icon_ok, NULL, _("Confirm"), NULL,
#else
        layoutDialogAdapter(&bmp_icon_ok, NULL, _("Confirm"), NULL,
#endif
                              _("The seed is valid"), _("and MATCHES"),
                              _("the one in the device."), NULL, NULL, NULL);
        protectButton(ButtonRequestType_ButtonRequest_Other, true);
        fsm_sendSuccess(
            _("The seed is valid and matches the one in the device"));
      } else {
#if ONEKEY_MINI
        layoutDialogAdapterEx(&bmp_icon_error, NULL, _("Confirm"), NULL,
#else
        layoutDialogAdapter(&bmp_icon_error, NULL, _("Confirm"), NULL,
#endif
                              _("The seed is valid"), _("but does NOT MATCH"),
                              _("the one in the device."), NULL, NULL, NULL);
        protectButton(ButtonRequestType_ButtonRequest_Other, true);
        fsm_sendFailure(
            FailureType_Failure_DataError,
            _("The seed is valid but does not match the one in the device"));
      }
    }
  } else {
    // New mnemonic is invalid.
    memzero(new_mnemonic, sizeof(new_mnemonic));
    if (!dry_run) {
      session_clear(true);
    } else {
#if ONEKEY_MINI
      layoutDialogAdapterEx(&bmp_icon_error, NULL, _("Confirm"), NULL,
                            _("The seed is INVALID!"), NULL, NULL, NULL, NULL,
                            NULL);
#else
      layoutDialogAdapter(&bmp_icon_error, NULL, _("Confirm"), NULL,
                          _("The seed is"), _("INVALID!"), NULL, NULL, NULL,
                          NULL);
#endif
      protectButton(ButtonRequestType_ButtonRequest_Other, true);
    }
    fsm_sendFailure(FailureType_Failure_DataError,
                    _("Invalid seed, are words in correct order?"));
  }
  awaiting_word = 0;
  layoutHome();

  return success;
}

/* Helper function for matrix recovery:
 * Formats a string describing the word range from first to last where
 * prefixlen gives the number of characters in first and last that are
 * significant, i.e. the word before first or the word after last differ
 * exactly at the prefixlen-th character.
 *
 * Invariants:
 *  memcmp("first - 1", first, prefixlen) != 0
 *  memcmp(last, "last + 1", prefixlen) != 0
 *  first[prefixlen-2] == last[prefixlen-2]  except for range WI-Z.
 */
static void add_choice(char choice[12], int prefixlen, const char *first,
                       const char *last) {
  // assert 1 <= prefixlen <= 4
  char *dest = choice;
  for (int i = 0; i < prefixlen; i++) {
    *dest++ = toupper((int)first[i]);
  }
  if (first[0] != last[0]) {
    /* special case WI-Z; also used for T-Z, etc. */
    *dest++ = '-';
    *dest++ = toupper((int)last[0]);
  } else if (last[prefixlen - 1] == first[prefixlen - 1]) {
    /* single prefix */
  } else if (prefixlen < 3) {
    /* AB-AC, etc. */
    *dest++ = '-';
    for (int i = 0; i < prefixlen; i++) {
      *dest++ = toupper((int)last[i]);
    }
  } else {
    /* RE[A-M] etc. */
    /* remove last and replace with space */
    dest[-1] = ' ';
    if (first[prefixlen - 1]) {
      /* handle special case: CAN[-D] */
      *dest++ = toupper((int)first[prefixlen - 1]);
    }
    *dest++ = '-';
    *dest++ = toupper((int)last[prefixlen - 1]);
  }
  *dest++ = 0;
}

/* Helper function for matrix recovery:
 * Display the recovery matrix given in choices.  If twoColumn is set
 * use 2x3 layout, otherwise 3x3 layout.  Also generates a random
 * scrambling and stores it in word_matrix.
 */
static void display_choices(bool twoColumn, char choices[9][12], int num) {
  const int nColumns = twoColumn ? 2 : 3;
  const int displayedChoices = nColumns * 3;
  for (int i = 0; i < displayedChoices; i++) {
    word_matrix[i] = i;
  }
  /* scramble matrix */
  random_permute((char *)word_matrix, displayedChoices);

  if (word_index % 4 == 0) {
    char desc[32] = "";
    int nr = (word_index / 4) + 1;
    format_number(desc, nr);
    layoutDialogSwipe(&bmp_icon_info, NULL, NULL, NULL, _("Please enter the"),
                      desc, _("of your mnemonic"), NULL, NULL, NULL);
  } else {
#if ONEKEY_MINI
    oledBox(0, 91, 127, 127, false);
#else
    oledBox(0, 27, 127, 63, false);
#endif
  }

  for (int row = 0; row < 3; row++) {
#if ONEKEY_MINI
    int y = 119 - row * 11;
#else
    int y = 55 - row * 11;
#endif
    for (int col = 0; col < nColumns; col++) {
      int x = twoColumn ? 64 * col + 32 : 42 * col + 22;
      int choice = word_matrix[nColumns * row + col];
      const char *text = choice < num ? choices[choice] : "-";
      oledDrawString(x - oledStringWidth(text, FONT_STANDARD) / 2, y, text,
                     FONT_STANDARD);
      if (twoColumn) {
        oledInvert(x - 32 + 1, y - 1, x - 32 + 63 - 1, y + 8);
      } else {
        oledInvert(x - 22 + 1, y - 1, x - 22 + 41 - 1, y + 8);
      }
    }
  }
  oledRefresh();

  /* avoid picking out of range numbers */
  for (int i = 0; i < displayedChoices; i++) {
    if (word_matrix[i] >= num) word_matrix[i] = 0;
  }
  /* two column layout: middle column = right column */
  if (twoColumn) {
    static const uint8_t twolayout[9] = {0, 1, 1, 2, 3, 3, 4, 5, 5};
    for (int i = 8; i >= 2; i--) {
      word_matrix[i] = word_matrix[twolayout[i]];
    }
  }
}

/* Helper function for matrix recovery:
 * Generates a new matrix and requests the next pin.
 */
static void next_matrix(void) {
  char word_choices[9][12] = {0};
  uint32_t idx = 0, num = 0;
  bool last = (word_index % 4) == 3;

  /* Build the matrix:
   * num: number of choices
   * word_choices[][]: the strings containing the choices
   */
  switch (word_index % 4) {
    case 3:
      /* last level: show up to six words */
      /* idx: index in table2 for the entered choice. */
      /* first: the first word. */
      /* num: the number of words to choose from. */
      idx = TABLE1(word_pincode / 9) + word_pincode % 9;
      const uint32_t first = TABLE2(idx);
      num = TABLE2(idx + 1) - first;
      for (uint32_t i = 0; i < num; i++) {
        strlcpy(word_choices[i], mnemonic_get_word(first + i),
                sizeof(word_choices[i]));
      }
      break;

    case 2:
      /* third level: show up to nine ranges (using table2) */
      /* idx: first index in table2 corresponding to pin code. */
      /* num: the number of choices. */
      idx = TABLE1(word_pincode);
      num = TABLE1(word_pincode + 1) - idx;
      for (uint32_t i = 0; i < num; i++) {
        add_choice(word_choices[i], (word_table2[idx + i] >> 12),
                   mnemonic_get_word(TABLE2(idx + i)),
                   mnemonic_get_word(TABLE2(idx + i + 1) - 1));
      }
      break;

    case 1:
      /* second level: exactly nine ranges (using table1) */
      /* idx: first index in table1 corresponding to pin code. */
      /* num: the number of choices. */
      idx = word_pincode * 9;
      num = 9;
      for (uint32_t i = 0; i < num; i++) {
        add_choice(word_choices[i], (word_table1[idx + i] >> 12),
                   mnemonic_get_word(TABLE2(TABLE1(idx + i))),
                   mnemonic_get_word(TABLE2(TABLE1(idx + i + 1)) - 1));
      }
      break;

    case 0:
      /* first level: exactly nine ranges */
      /* num: the number of choices. */
      num = 9;
      for (uint32_t i = 0; i < num; i++) {
        add_choice(word_choices[i], 1, mnemonic_get_word(TABLE2(TABLE1(9 * i))),
                   mnemonic_get_word(TABLE2(TABLE1(9 * (i + 1))) - 1));
      }
      break;
  }
  display_choices(last, word_choices, num);

  recovery_request();
}

/* Function called when a digit was entered by user.
 * digit: ascii code of the entered digit ('1'-'9') or
 * '\x08' for backspace.
 */
static void recovery_digit(const char digit) {
  if (digit == 8) {
    /* backspace: undo */
    if ((word_index % 4) == 0) {
      /* undo complete word */
      if (word_index > 0) word_index -= 4;
    } else {
      word_index--;
      word_pincode /= 9;
    }
    next_matrix();
    return;
  }

  if (digit < '1' || digit > '9') {
    recovery_request();
    return;
  }

  int choice = word_matrix[digit - '1'];
  if ((word_index % 4) == 3) {
    /* received final word */

    /* Mark the chosen word for 250 ms */
#if ONEKEY_MINI
    int y = 118 - ((digit - '1') / 3) * 11;
#else
    int y = 54 - ((digit - '1') / 3) * 11;
#endif

    int x = 64 * (((digit - '1') % 3) > 0);
    oledInvert(x + 1, y, x + 62, y + 9);
    oledRefresh();
    usbTiny(1);
    usbSleep(250);
    usbTiny(0);

    /* index of the chosen word */
    int idx = TABLE2(TABLE1(word_pincode / 9) + (word_pincode % 9)) + choice;
    uint32_t widx = word_index / 4;

    word_pincode = 0;
    strlcpy(words[widx], mnemonic_get_word(idx), sizeof(words[widx]));
    if (widx + 1 == word_count) {
      recovery_done();
      return;
    }
    /* next word */
  } else {
    word_pincode = word_pincode * 9 + choice;
  }
  word_index++;
  next_matrix();
}

/* Helper function for scrambled recovery:
 * Ask the user for the next word.
 */
void next_word(void) {
  layoutLast = layoutDialogSwipe;
  layoutSwipe();
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, 8, _("Please enter"),
                              FONT_STANDARD);
  word_pos = word_order[word_index];
  if (word_pos == 0) {
    strlcpy(fake_word, mnemonic_get_word(random_uniform(BIP39_WORDS)),
            sizeof(fake_word));
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 24, fake_word,
                                FONT_FIXED | FONT_DOUBLE);
  } else {
    fake_word[0] = 0;
    char desc[32] = "";
    format_number(desc, word_pos);
    oledDrawStringCenterAdapter(OLED_WIDTH / 2, 24, desc,
                                FONT_FIXED | FONT_DOUBLE);
  }
  oledDrawStringCenterAdapter(OLED_WIDTH / 2, 48, _("on your computer"),
                              FONT_STANDARD);
  // 35 is the maximum pixels used for a pixel row ("the 21st word")
  oledSCA(24 - 2, 24 + 15 + 2, 35);
  oledInvert(0, 24 - 2, OLED_WIDTH - 1, 24 + 15 + 2);
  oledRefresh();
  recovery_request();
}

void recovery_init(uint32_t _word_count, bool passphrase_protection,
                   bool pin_protection, const char *language, const char *label,
                   bool _enforce_wordlist, uint32_t type, uint32_t u2f_counter,
                   bool _dry_run) {
  if (_word_count != 12 && _word_count != 18 && _word_count != 24) return;

  word_count = _word_count;
#if !EMULATOR
  _enforce_wordlist = true;
#endif
  enforce_wordlist = _enforce_wordlist;
  dry_run = _dry_run;

  if (!dry_run) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL, NULL,
                      _("Do you really want to"), _("recover the device?"),
                      NULL, NULL, NULL);
    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }

  if (!dry_run) {
    if (pin_protection && !protectChangePin(false)) {
      layoutHome();
      return;
    }

    config_setPassphraseProtection(passphrase_protection);
    config_setLanguage(language);
    config_setLabel(label);
    config_setU2FCounter(u2f_counter);
  }

  if ((type & RecoveryDeviceType_RecoveryDeviceType_Matrix) != 0) {
    awaiting_word = 2;
    word_index = 0;
    word_pincode = 0;
    next_matrix();
  } else {
    for (uint32_t i = 0; i < word_count; i++) {
      word_order[i] = i + 1;
    }
    for (uint32_t i = word_count; i < 24; i++) {
      word_order[i] = 0;
    }
    random_permute(word_order, 24);
    awaiting_word = 1;
    word_index = 0;
    next_word();
  }
}

static void recovery_scrambledword(const char *word) {
  int index = -1;
  if (enforce_wordlist) {  // check if word is valid
    index = mnemonic_find_word(word);
  }
  if (word_pos == 0) {  // fake word
    if (strcmp(word, fake_word) != 0) {
      if (!dry_run) {
        session_clear(true);
      }
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Wrong word retyped"));
      layoutHome();
      return;
    }
  } else {  // real word
    if (enforce_wordlist) {
      if (index < 0) {  // not found
        if (!dry_run) {
          session_clear(true);
        }
        fsm_sendFailure(FailureType_Failure_DataError,
                        _("Word not found in a wordlist"));
        layoutHome();
        return;
      }
    }
    strlcpy(words[word_pos - 1], word, sizeof(words[word_pos - 1]));
  }

  if (word_index + 1 == 24) {  // last one
    recovery_done();
  } else {
    word_index++;
    next_word();
  }
}

/* Function called when a word was entered by user. Used
 * for scrambled recovery.
 */
void recovery_word(const char *word) {
  switch (awaiting_word) {
    case 2:
      recovery_digit(word[0]);
      break;
    case 1:
      recovery_scrambledword(word);
      break;
    default:
      fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                      _("Not in Recovery mode"));
      break;
  }
}

/* Abort recovery.
 */
void recovery_abort(void) {
  if (awaiting_word) {
    layoutHome();
    awaiting_word = 0;
  }
}

#define CANDIDATE_MAX_LEN 8  // DO NOT CHANGE THIS
static bool select_complete_word(char *title, int start, int len) {
  uint8_t key = KEY_NULL;
  int index = 0;
#if ONEKEY_MINI
  char *data[CANDIDATE_MAX_LEN];

  if (len > CANDIDATE_MAX_LEN) return false;

  for (int i = 0; i < len; i++) {
    data[i] = (char *)mnemonic_get_word(start + i);
  }
  (void)data;
#endif

refresh_menu:
#if ONEKEY_MINI
  layoutItemsSelectAdapterAlign(
      NULL, NULL, &bmp_button_back, &bmp_button_forward, _("PREV"), _("OK"),
      index + 1, len, false, title, NULL, NULL, data, FONT_FIXED);
#else
  layoutItemsSelectAdapter(
      &bmp_btn_up, &bmp_btn_down, &bmp_btn_back, &bmp_btn_forward, _("Cancel"),
      _("Confirm"), 0, 0, title, NULL, mnemonic_get_word(start + index),
      index > 0 ? mnemonic_get_word(start + index - 1) : NULL,
      index < len - 1 ? mnemonic_get_word(start + index + 1) : NULL);
#endif

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_DOWN:
      if (index < len - 1) index++;
      goto refresh_menu;
    case KEY_UP:
      if (index > 0) index--;
      goto refresh_menu;
    case KEY_CONFIRM:
      strlcpy(words[word_index], mnemonic_get_word(start + index),
              sizeof(words[word_index]));
      word_index++;
      break;
    case KEY_CANCEL:
      return false;
    default:
      break;
  }

  return true;
}

static bool recovery_check_words(void) {
  uint8_t key = KEY_NULL;
  uint32_t index = 0;

#if ONEKEY_MINI
  char desc[16] = "";
  uint32_t j = 0, pages = 0;
  const struct font_desc *font = find_cur_font();

  pages = word_count / WORD_PER_PAGE;
  pages += (word_count % WORD_PER_PAGE) ? 1 : 0;
  if (pages == 0) return false;

refresh_menu:
  oledClear_ex();

  switch (index) {
    case 0:
      for (j = 0; j < WORD_PER_PAGE; j++) {
        memzero(desc, sizeof(desc));
        uint2str(j + 1, desc);
        strcat(desc, ". ");
        strcat(desc, words[j]);
        if ((j + 1) > 9) {
          oledDrawStringAdapter(OLED_WIDTH / 4 - font->width,
                                (j + 2) * (font->pixel + 1), desc, FONT_FIXED);
        } else {
          oledDrawStringAdapter(OLED_WIDTH / 4, (j + 2) * (font->pixel + 1),
                                desc, FONT_FIXED);
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
                                (j + 2) * (font->pixel + 1), desc, FONT_FIXED);
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
                                (j + 2) * (font->pixel + 1), desc, FONT_FIXED);
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
      if (index > 0) index--;
      goto refresh_menu;
    case KEY_DOWN:
      if (index < pages - 1) index++;
      goto refresh_menu;
    case KEY_CANCEL:
      goto refresh_menu;
    case KEY_CONFIRM:
      if (index == pages - 1) {
        char new_mnemonic[MAX_MNEMONIC_LEN + 1] = {0};

        strlcpy(new_mnemonic, words[0], sizeof(new_mnemonic));

        for (uint32_t i = 1; i < word_count; i++) {
          strlcat(new_mnemonic, " ", sizeof(new_mnemonic));
          strlcat(new_mnemonic, words[i], sizeof(new_mnemonic));
        }

        if (!mnemonic_check(new_mnemonic)) {
          return false;
        } else {
          return true;
        }
      } else {
        index++;
        goto refresh_menu;
      }
      return false;
    default:
      break;
  }
#else
  char desc[64] = "";

refresh_menu:
  memzero(desc, sizeof(desc));
  strcat(desc, " #");
  uint2str(index + 1, desc + strlen(desc));
  layoutItemsSelectAdapter(&bmp_btn_up, &bmp_btn_down, NULL, &bmp_btn_confirm,
                           NULL, _("Okay"), index + 1, word_count, NULL, desc,
                           words[index], index > 0 ? words[index - 1] : NULL,
                           index < word_count - 1 ? words[index + 1] : NULL);

  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_DOWN:
      if (index < word_count - 1) index++;
      goto refresh_menu;
    case KEY_UP:
      if (index > 0) index--;
      goto refresh_menu;
    case KEY_CONFIRM:
      if (index < word_count - 1) {
        index++;
        goto refresh_menu;
      } else {
        char new_mnemonic[MAX_MNEMONIC_LEN + 1] = {0};

        strlcpy(new_mnemonic, words[0], sizeof(new_mnemonic));
        for (uint32_t i = 1; i < word_count; i++) {
          strlcat(new_mnemonic, " ", sizeof(new_mnemonic));
          strlcat(new_mnemonic, words[i], sizeof(new_mnemonic));
        }
        if (!mnemonic_check(new_mnemonic)) return false;
      }
      return true;
    case KEY_CANCEL:
      goto refresh_menu;
    default:
      break;
  }
#endif
  return false;
}

static bool input_words(void) {
  uint32_t prefix_len = 0;
  uint8_t key = KEY_NULL;
  char letter_list[52] = "";
  int index = 0;
  int letter_count = 0;
  char desc[64] = "";

  memzero(words[word_index], sizeof(words[word_index]));

refresh_menu:
  memzero(desc, sizeof(desc));
#if ONEKEY_MINI
  strcat(desc, _("Enter word"));
  strcat(desc, _(" #"));
#else
  strcat(desc, _("Enter seed phrase "));
  strcat(desc, _("#"));
#endif

  uint2str(word_index + 1, desc + strlen(desc));
  memzero(letter_list, sizeof(letter_list));
  letter_count = mnemonic_next_letter_with_prefix(words[word_index], prefix_len,
                                                  letter_list);

  layoutInputWord(desc, prefix_len, words[word_index], letter_list + 2 * index);
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index < letter_count - 1)
        index++;
      else
        index = 0;
      goto refresh_menu;
    case KEY_DOWN:
      if (index > 0)
        index--;
      else
        index = letter_count - 1;
      goto refresh_menu;
    case KEY_CONFIRM:
      words[word_index][prefix_len++] = letter_list[2 * index];
      letter_count = mnemonic_count_with_prefix(words[word_index], prefix_len);
      if (letter_count > CANDIDATE_MAX_LEN) {
        index = 0;
        goto refresh_menu;
      } else {
        uint32_t candidate_location =
            mnemonic_word_index_with_prefix(words[word_index], prefix_len);
        select_complete_word(NULL, candidate_location, letter_count);
        if (word_index == word_count) {
          return true;
        } else {  // next word
          prefix_len = 0;
          index = 0;
          memzero(words[word_index], sizeof(words[word_index]));
          goto refresh_menu;
        }
      }
    case KEY_CANCEL:
      if (prefix_len > 0) {
        prefix_len--;
        index = 0;
        words[word_index][prefix_len] = 0;
      } else if (word_index > 0) {
        word_index--;
        prefix_len = 0;
        index = 0;
        memzero(words[word_index], sizeof(words[word_index]));
      } else {
        break;
      }
      goto refresh_menu;
    default:
      break;
  }
  return false;
}

bool recovery_on_device(void) {
  char desc[72] = "";
  uint8_t key = KEY_NULL;

prompt_recovery:
#if ONEKEY_MINI
  layoutDialogSwipeCenterAdapterEx(
      NULL, &bmp_button_back, _("BACK"), &bmp_button_forward, _("NEXT"), NULL,
      true, NULL, NULL, NULL, NULL,
      _("Follow the guide and\nenter recovery phrase\nto restore wallet."),
      NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#else
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward, _("Next"), NULL, NULL,
      NULL, _("Enter seed phrases to"), _("restore wallet"), NULL, NULL);
#endif
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }

select_mnemonic_count:
  if (!protectSelectMnemonicNumber(&word_count)) {
    goto_check(prompt_recovery);
  }

  memzero(desc, sizeof(desc));

#if ONEKEY_MINI
  switch (word_count) {
    case 12:
      strcat(desc,
             _("Please enter your 12-\nwords recovery phrase\nin order."));
      break;
    case 18:
      strcat(desc,
             _("Please enter your 18-\nwords recovery phrase\nin order."));
      break;
    case 24:
      strcat(desc,
             _("Please enter your 24-\nwords recovery phrase\nin order."));
      break;
    default:
      break;
  }

  layoutDialogSwipeCenterAdapterEx(NULL, &bmp_button_back, _("BACK"),
                                   &bmp_button_forward, _("NEXT"), NULL, true,
                                   NULL, NULL, NULL, NULL, desc, NULL, NULL,
                                   NULL, NULL, NULL, NULL, NULL);
#else
  strcat(desc, _("Please enter your"));
  uint2str(word_count, desc + strlen(desc));
  layoutDialogSwipeCenterAdapter(NULL, &bmp_btn_back, _("Back"),
                                 &bmp_btn_forward, _("Next"), NULL, NULL, NULL,
                                 desc, _("seed phrases"), NULL, NULL);
#endif
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    goto_check(select_mnemonic_count);
  }

input_word:
  word_index = 0;
  recovery_byself = true;
  enforce_wordlist = true;
  dry_run = false;

  if (!input_words()) {
    goto_check(select_mnemonic_count);
  }

  memzero(desc, sizeof(desc));
#if ONEKEY_MINI
  switch (word_count) {
    case 12:
      strcat(desc, _("Check the 12 words\nyou entered are the\ncorrect "
                     "recovery\nphrase"));
      break;
    case 18:
      strcat(desc, _("Check the 18 words\nyou entered are the\ncorrect "
                     "recovery\nphrase"));
      break;
    case 24:
      strcat(desc, _("Check the 24 words\nyou entered are the\ncorrect "
                     "recovery\nphrase"));
      break;
    default:
      break;
  }

  layoutDialogCenterAdapterEx(NULL, &bmp_button_back, _("BACK"),
                              &bmp_button_forward, _("NEXT"), NULL, true, NULL,
                              NULL, NULL, NULL, desc, NULL, NULL, NULL, NULL,
                              NULL, NULL, NULL);
#else
  strcat(desc, _("Check the entered "));
  uint2str(word_count, desc + strlen(desc));
  layoutDialogCenterAdapter(NULL, &bmp_btn_back, _("Back"), &bmp_btn_forward,
                            _("Next"), NULL, NULL, NULL, desc,
                            _("seed phrases"), NULL, NULL);
#endif

  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    goto_check(input_word);
  }

check_word:
  if (!recovery_check_words()) {
#if ONEKEY_MINI
    setRgbBitmap(true);
    layoutDialogSwipeCenterAdapterEx(
        &bmp_icon_forbid, NULL, NULL, &bmp_btn_retry, _("RETRY"), NULL, true,
        NULL, NULL, NULL, NULL, NULL, NULL,
        _("Incorrect recovery\nphrase, try again."), NULL, NULL, NULL, NULL,
        NULL);
#else
    layoutDialogSwipeCenterAdapter(
        &bmp_icon_error, NULL, NULL, &bmp_btn_retry, _("Retry"), NULL, NULL,
        NULL, NULL, _("Invalid seed phrases"), _("Please try again"), NULL);
#endif

    protectWaitKey(0, 1);
#if ONEKEY_MINI
    setRgbBitmap(false);
#endif
    goto_check(prompt_recovery);
  }

#if ONEKEY_MINI
  setRgbBitmap(true);
  layoutDialogSwipeCenterAdapterEx(
      &bmp_icon_success, NULL, NULL, &bmp_button_forward, _("NEXT"), NULL, true,
      NULL, NULL, NULL, NULL, NULL, NULL, _("Recovery Phrase\nimported."), NULL,
      NULL, NULL, NULL, NULL);
  protectWaitKey(0, 1);
  setRgbBitmap(false);
#endif

  if (!protectChangePinOnDevice(false, true)) {
    goto_check(check_word);
  }

  recovery_done();

  recovery_byself = false;
  return true;
}

#if ONEKEY_MINI
#define MAX_ERROR_CNT 5

static char stored_words[24][12];

uint32_t get_mnemonic_number(char *mnemonic) {
  uint32_t i = 0, count = 0;

  memzero(stored_words, sizeof(stored_words));

  while (mnemonic[i] != 0) {
    int j = 0;
    while (mnemonic[i] != ' ' && mnemonic[i] != 0 &&
           j + 1 < (int)sizeof(stored_words[count])) {
      stored_words[count][j] = mnemonic[i];
      i++;
      j++;
    }
    count++;
    if (mnemonic[i] != 0) {
      i++;
    }
  }

  return count;
}

uint32_t select_mnemonic_number(uint32_t count) {
  uint8_t key = KEY_NULL;
  uint32_t index = 0;
  uint32_t num_s[24] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
  char *numbers[24] = {"1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
                       "9",  "10", "11", "12", "13", "14", "15", "16",
                       "17", "18", "19", "20", "21", "22", "23", "24"};

refresh_menu:
  layoutItemsSelectAdapterImp(
      &bmp_btn_up, &bmp_btn_down, &bmp_button_back, &bmp_button_forward,
      _("BACK"), _("OK"), index + 1, count, false, NULL, NULL, NULL, numbers);
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) index--;
      goto refresh_menu;
    case KEY_DOWN:
      if (index < count - 1) index++;
      goto refresh_menu;
    case KEY_CONFIRM:
      return num_s[index];
    case KEY_CANCEL:
      return 0;
    default:
      return 0;
  }
}

bool verify_words(char *mnemonic, uint32_t count, uint32_t mode) {
  bool selected = false;
  uint32_t no = 1, prefix_len = 0;
  uint8_t key = KEY_NULL;
  char letter_list[52] = "";
  int index = 0;
  int letter_count = 0;
  char desc[64] = "";
  static uint32_t counter = 0;

  if (mnemonic == NULL) {
    return false;
  }

select_indicate:
  if (mode == 1) {
    memset(desc, 0, sizeof(desc));
    strcat(desc, _("Select the sequence\nnumber of word to be\nchecked"));
    layoutDialogSwipeCenterAdapterEx(NULL, &bmp_button_back, _("BACK"),
                                     &bmp_button_forward, _("NEXT"), NULL, true,
                                     NULL, NULL, NULL, NULL, desc, NULL, NULL,
                                     NULL, NULL, NULL, NULL, NULL);
    key = protectWaitKey(0, 1);
    if (key != KEY_CONFIRM) {
      return false;
    }

  select_number:
    no = select_mnemonic_number(count);
    if (no == 0) {
      goto select_indicate;
    }
  }

  if (mode == 0) {
    word_index = 0;
  } else {
    word_index = no - 1;
  }

  memzero(words[word_index], sizeof(words[word_index]));

refresh_menu:
  memzero(desc, sizeof(desc));
  strcat(desc, _("Enter word"));
  strcat(desc, _(" #"));

  uint2str(word_index + 1, desc + strlen(desc));
  memzero(letter_list, sizeof(letter_list));
  letter_count = mnemonic_next_letter_with_prefix(words[word_index], prefix_len,
                                                  letter_list);

  layoutInputWord(desc, prefix_len, words[word_index], letter_list + 2 * index);
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index < letter_count - 1)
        index++;
      else
        index = 0;
      goto refresh_menu;
    case KEY_DOWN:
      if (index > 0)
        index--;
      else
        index = letter_count - 1;
      goto refresh_menu;
    case KEY_CONFIRM:
      words[word_index][prefix_len++] = letter_list[2 * index];
      letter_count = mnemonic_count_with_prefix(words[word_index], prefix_len);
      if (letter_count > CANDIDATE_MAX_LEN) {
        index = 0;
        goto refresh_menu;
      } else {
        uint32_t candidate_location =
            mnemonic_word_index_with_prefix(words[word_index], prefix_len);
        selected = select_complete_word(NULL, candidate_location, letter_count);

        if (word_index == count && mode != 1) {
          char new_mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
          strlcpy(new_mnemonic, words[0], sizeof(new_mnemonic));
          for (uint32_t i = 1; i < count; i++) {
            strlcat(new_mnemonic, " ", sizeof(new_mnemonic));
            strlcat(new_mnemonic, words[i], sizeof(new_mnemonic));
          }

          if (!mnemonic_check(new_mnemonic)) {
            setRgbBitmap(true);
            layoutDialogSwipeCenterAdapterEx(
                &bmp_icon_err, NULL, NULL, &bmp_button_forward, _("QUIT"), NULL,
                true, NULL, NULL, NULL, NULL, NULL, NULL,
                _("Check failed, Quit\nand try again"), NULL, NULL, NULL, NULL,
                NULL);
            protectWaitKey(0, 1);
            setRgbBitmap(false);
            counter = 0;
            return false;
          } else {
            setRgbBitmap(true);
            layoutDialogSwipeCenterAdapterEx(
                &bmp_icon_success, NULL, NULL, &bmp_button_forward, _("DONE"),
                NULL, true, NULL, NULL, NULL, NULL, NULL, NULL,
                _("Check Passed"), NULL, NULL, NULL, NULL, NULL);
            protectWaitKey(0, 1);
            setRgbBitmap(false);
          }
          return true;
        } else {  // next word
          prefix_len = 0;
          index = 0;

          if (word_index < count)
            memzero(words[word_index], sizeof(words[word_index]));

          if (strcmp(words[word_index - 1], stored_words[word_index - 1]) !=
              0) {
            if (!selected) {
              goto refresh_menu;
            }

            counter++;
            if (counter < MAX_ERROR_CNT) {
              if (word_index > 0) {
                word_index--;
                prefix_len = 0;
                index = 0;
                memzero(words[word_index], sizeof(words[word_index]));
              }

              setRgbBitmap(true);
              layoutDialogSwipeCenterAdapterEx(
                  &bmp_icon_forbid, NULL, NULL, &bmp_btn_retry, _("RETRY"),
                  NULL, true, NULL, NULL, NULL, NULL, NULL, NULL,
                  _("Incorrect word. Try\nagain"), NULL, NULL, NULL, NULL,
                  NULL);
              protectWaitKey(0, 1);
              setRgbBitmap(false);
            } else {
              setRgbBitmap(true);
              layoutDialogSwipeCenterAdapterEx(
                  &bmp_icon_err, NULL, NULL, &bmp_button_forward, _("QUIT"),
                  NULL, true, NULL, NULL, NULL, NULL, NULL, NULL,
                  _("Check failed, Quit\nand try again"), NULL, NULL, NULL,
                  NULL, NULL);
              protectWaitKey(0, 1);
              setRgbBitmap(false);
              counter = 0;
              return false;
            }
          } else {
            if (mode == 1) {
              setRgbBitmap(true);
              layoutDialogSwipeCenterAdapterEx(
                  &bmp_icon_success, &bmp_button_back, _("QUIT"),
                  &bmp_button_forward, _("OK"), NULL, true, NULL, NULL, NULL,
                  NULL, NULL, NULL, _("Check Passed"), NULL, NULL, NULL, NULL,
                  NULL);
              key = protectWaitKey(0, 1);
              setRgbBitmap(false);
              if (key == KEY_CONFIRM) {
                goto select_number;
              }
              return true;
            }
          }
          goto refresh_menu;
        }
      }
    case KEY_CANCEL:
      if (prefix_len > 0) {
        prefix_len--;
        index = 0;
        words[word_index][prefix_len] = 0;
      } else if (word_index > 0) {
        if (mode == 0) {
          word_index--;
          prefix_len = 0;
          index = 0;
          memzero(words[word_index], sizeof(words[word_index]));
        } else {
          goto select_number;
        }
      } else {
        break;
      }
      goto refresh_menu;
    default:
      break;
  }

  return false;
}
#endif

#if DEBUG_LINK

const char *recovery_get_fake_word(void) { return fake_word; }

uint32_t recovery_get_word_pos(void) { return word_pos; }

#endif
