#include "tron_ui.h"
#include <stdio.h>
#include "gettext.h"
#include "layout2.h"
#include "tron.h"

void layoutTronConfirmTx(const char *to_str, const uint64_t value,
                         const uint8_t *value_bytes, ConstTronTokenPtr token) {
  char amount[60];
  if (token == NULL) {
    if (value == 0) {
      strcpy(amount, _("Unknown Token"));
    } else {
      tron_format_amount(value, amount, sizeof(amount));
    }
  } else {
    bignum256 val;
    bn_read_be(value_bytes, &val);
    tron_format_token_amount(&val, token, amount, sizeof(amount));
  }

  char _to1[] = "to   ________";
  char _to2[] = "_____________";
  char _to3[] = "_____________?";

  int to_len = strlen(to_str);
  if (to_len) {
    memcpy(_to1 + 5, to_str, 8);
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
