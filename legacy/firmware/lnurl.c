#include <stdio.h>
#include "aes/aes.h"
#include "base64.h"
#include "buttons.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "nostr.h"
#include "protect.h"
#include "ripemd160.h"
#include "rng.h"
#include "secp256k1.h"
#include "segwit_addr.h"
#include "sha3.h"
#include "stdint.h"
#include "transaction.h"
#include "util.h"
#include "zkp_bip340.h"

extern void drawScrollbar(int pages, int index);
bool layout_lnurl_auth(const LnurlAuth *msg) {
  uint8_t key = KEY_NULL;
  char data[65] = {0};
  int index = 0;
  const char **str = NULL;
  if (msg->data.size > 0 && msg->data.size != 32) return false;
  data2hexaddr(msg->data.bytes, msg->data.size, data);
  str = split_message((const uint8_t *)data, 64, 20);

loop:
  layoutDialogAdapterEx(_("LNURL Authorization"), &bmp_bottom_left_close, NULL,
                        &bmp_bottom_right_arrow, NULL, NULL, _("Domain:"),
                        (char *)msg->domain.bytes, NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }

  index = 0;

__data:
  oledClear();
  layoutHeader(_("LNURL Authorization"));
  oledDrawStringAdapter(0, 13, _("Data:"), FONT_STANDARD);
  if (index == 0) {
    oledDrawStringAdapter(0, 13 + 1 * 10, str[0], FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 2 * 10, str[1], FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 3 * 10, str[2], FONT_STANDARD);
    oledDrawBitmap(3 * OLED_WIDTH / 4 - 8, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_down);
  } else {
    oledDrawStringAdapter(0, 13 + 1 * 10, str[1], FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 2 * 10, str[2], FONT_STANDARD);
    oledDrawStringAdapter(0, 13 + 3 * 10, str[3], FONT_STANDARD);
    oledDrawBitmap(OLED_WIDTH / 4, OLED_HEIGHT - 8,
                   &bmp_bottom_middle_arrow_up);
  }
  layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
  layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);

  drawScrollbar(2, index);
  oledRefresh();
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      if (index > 0) {
        index--;
      }
      goto __data;
    case KEY_DOWN:
      if (index < 1) index++;
      goto __data;
    case KEY_CONFIRM:
      break;
    case KEY_CANCEL:
      goto loop;
    default:
      goto __data;
  }

  layoutDialogCenterAdapterV2(_("Approve Request"), NULL,
                              &bmp_bottom_left_close, &bmp_bottom_right_confirm,
                              NULL, NULL, NULL, NULL, NULL, NULL,
                              _("Do you want to approve\nLNURL Auth request?"));
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }

  return true;
}
