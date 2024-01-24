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

bool layout_lnurl_auth(const LnurlAuth *msg) {
  uint8_t key = KEY_NULL;
  char data[65] = {0};
  if (msg->data.size > 0 && msg->data.size != 32) return false;
  data2hexaddr(msg->data.bytes, msg->data.size, data);

loop:
  layoutDialogAdapterEx(_("LNURL Authorization"), &bmp_bottom_left_close, NULL,
                        &bmp_bottom_right_arrow, NULL, NULL, _("Domain:"),
                        (char *)msg->domain.bytes, NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    return false;
  }
  layoutDialogAdapterEx(_("LNURL Authorization"), &bmp_bottom_left_arrow, NULL,
                        &bmp_bottom_right_arrow, NULL, NULL, _("Data:"), data,
                        NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    goto loop;
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
