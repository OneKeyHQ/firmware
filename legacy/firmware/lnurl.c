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
  char data[65] = {0};
  if (msg->data.size > 0 && msg->data.size != 32) return false;
  data2hexaddr(msg->data.bytes, msg->data.size, data);

  if (!fsm_layoutSignMessage_ex(_("Domain"), (const uint8_t *)msg->domain.bytes,
                                msg->domain.size)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return false;
  }
  if (!fsm_layoutSignMessage_ex(_("Data"), (const uint8_t *)data,
                                strlen(data))) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return false;
  }

  return true;
}
