
#include "aptos.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "stdint.h"
#include "util.h"

// Prefix_bytes with SHA3_256 hash bytes of string `APTOS::RawTransaction`
static const uint8_t APTOS_RAW_TX_PREFIX[32] = {
    181, 233, 125, 176, 127, 160, 189, 14,  85,  152, 170,
    54,  67,  169, 188, 111, 102, 147, 189, 220, 26,  159,
    236, 158, 103, 74,  70,  30,  170, 0,   177, 147};

void aptos_get_address_from_public_key(const uint8_t *public_key,
                                       char *address) {
  uint8_t buf[SIZE_PUBKEY] = {0};
  struct SHA3_CTX ctx = {0};

  sha3_256_Init(&ctx);
  sha3_Update(&ctx, public_key, SIZE_PUBKEY);
  // append single-signature scheme identifier
  sha3_Update(&ctx, (const uint8_t *)"\x00", 1);
  sha3_Final(&ctx, buf);
  address[0] = '0';
  address[1] = 'x';
  data2hex((const uint8_t *)buf, SIZE_PUBKEY, address + 2);
}
void aptos_sign_tx(const AptosSignTx *msg, const HDNode *node,
                   AptosSignedTx *resp) {
  char address[67] = {0};
  aptos_get_address_from_public_key(node->public_key + 1, address);

  if (!layoutBlindSign(address)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
    layoutHome();
    return;
  }

  uint8_t buf[sizeof(AptosSignTx_raw_tx_t) + 32] = {0};
  memcpy(buf, APTOS_RAW_TX_PREFIX, 32);
  memcpy(buf + 32, msg->raw_tx.bytes, msg->raw_tx.size);
  ed25519_sign(buf, msg->raw_tx.size + 32, node->private_key,
               node->public_key + 1, resp->signature.bytes);
  memcpy(resp->public_key.bytes, node->public_key + 1, 32);
  resp->signature.size = 64;
  resp->public_key.size = 32;
  msg_write(MessageType_MessageType_AptosSignedTx, resp);
}
