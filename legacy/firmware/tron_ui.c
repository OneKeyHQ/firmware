#include "tron_ui.h"
#include <stdio.h>
#include "gettext.h"
#include "layout2.h"
#include "tron.h"
#include "util.h"

void layoutTronConfirmTx(const char *to_str, const uint64_t value,
                         const uint8_t *value_bytes, ConstTronTokenPtr token) {
  char amount[60];
  if (token == NULL) {
    if (value == 0) {
      strcpy(amount, _("message"));
    } else {
      tron_format_amount(value, amount, sizeof(amount));
    }
  } else {
    bignum256 val;
    bn_read_be(value_bytes, &val);
    tron_format_token_amount(&val, token, amount, sizeof(amount));
  }

  char _to1[30] = {0};
  char _to2[] = "_____________";
  char _to3[] = "_____________?";

  snprintf(_to1, 30, "%s   ________", _("to"));
  int to_len = strlen(to_str);
  if (to_len) {
    memcpy(_to1 + strlen(_("to")) + 3, to_str, 8);
    memcpy(_to2, to_str + 8, 13);
    memcpy(_to3, to_str + 21, 13);
  } else {
    strlcpy(_to1, _("to new contract?"), sizeof(_to1));
    strlcpy(_to2, "", sizeof(_to2));
    strlcpy(_to3, "", sizeof(_to3));
  }

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Send"), amount, _to1, _to2, _to3, NULL);
}

void layoutTronFee(const uint64_t value, const uint8_t *value_bytes,
                   ConstTronTokenPtr token, const uint64_t fee) {
  char gas_value[32];
  tron_format_amount(fee, gas_value, sizeof(gas_value));

  char tx_value[60];
  if (token == NULL) {
    if (value == 0) {
      strcpy(tx_value, _("Unknown Token"));
    } else {
      tron_format_amount(value, tx_value, sizeof(tx_value));
    }
  } else {
    bignum256 val;
    bn_read_be(value_bytes, &val);
    tron_format_token_amount(&val, token, tx_value, sizeof(tx_value));
  }

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Really send"), tx_value, _("and limit max fee to"),
                    gas_value, _("?"), NULL);
}

void layoutTronData(const uint8_t *data, uint32_t len, uint32_t total_len) {
  char hexdata[3][17] = {0};
  char summary[20] = {0};
  uint32_t printed = 0;
  for (int i = 0; i < 3; i++) {
    uint32_t linelen = len - printed;
    if (linelen > 8) {
      linelen = 8;
    }
    data2hex(data, linelen, hexdata[i]);
    data += linelen;
    printed += linelen;
  }

  strcpy(summary, "...          bytes");
  char *p = summary + 11;
  uint32_t number = total_len;
  while (number > 0) {
    *p-- = '0' + number % 10;
    number = number / 10;
  }
  char *summarystart = summary;
  if (total_len == printed) summarystart = summary + 4;

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Transaction data:"), hexdata[0], hexdata[1], hexdata[2],
                    summarystart, NULL);
}
