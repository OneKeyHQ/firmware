#include "sui.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "stdint.h"
#include "util.h"

void sui_get_address_from_public_key(const uint8_t *public_key, char *address) {
  uint8_t buf[32] = {0};
  BLAKE2B_CTX ctx;
  blake2b_Init(&ctx, 32);
  blake2b_Update(&ctx, (const uint8_t *)"\x00", 1);
  blake2b_Update(&ctx, public_key, 32);
  blake2b_Final(&ctx, buf, 32);

  address[0] = '0';
  address[1] = 'x';
  data2hexaddr((const uint8_t *)buf, 32, address + 2);
}

void sui_sign_tx(const SuiSignTx *msg, const HDNode *node, SuiSignedTx *resp) {
  char address[67] = {0};

  sui_get_address_from_public_key(node->public_key + 1, address);
  // INTENT_BYTES = b'\x00\x00\x00'
  if ((msg->raw_tx.bytes[0] != 0x00) && ((msg->raw_tx.bytes[1] != 0x00)) &&
      ((msg->raw_tx.bytes[2] != 0x00))) {
    fsm_sendFailure(FailureType_Failure_DataError, "Invalid raw tx");
    layoutHome();
  }

  if (!layoutBlindSign("Sui", false, NULL, address, msg->raw_tx.bytes,
                       msg->raw_tx.size, NULL, NULL, NULL, NULL, NULL, NULL)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled,
                    "Signing cancelled by user");
    layoutHome();
    return;
  }

  ed25519_sign(msg->raw_tx.bytes, msg->raw_tx.size, node->private_key,
               resp->signature.bytes);
  memcpy(resp->public_key.bytes, node->public_key + 1, 32);
  resp->signature.size = 64;
  resp->public_key.size = 32;
  msg_write(MessageType_MessageType_SuiSignedTx, resp);
}