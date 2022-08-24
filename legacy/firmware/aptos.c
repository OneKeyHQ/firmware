
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
    129, 233, 10, 2,   66,  120, 128, 71, 174, 165, 61,  66,  59,  155, 9,  13,
    49,  234, 47, 204, 248, 247, 46,  81, 158, 47,  205, 167, 151, 43,  38, 72};

static void layoutRequireConfirmTxSigner(char *address) {
  const char **str =
      split_message((const uint8_t *)address, strlen(address), 22);
  layoutDialogSwipe(NULL, _("Cancel"), _("Continue"), _("CONFIRM SIGNING:"),
                    _("SENDER:"), str[0], str[1], str[2], str[3], NULL);
}

static void layoutRequireConfirmRisk(void) {
  layoutDialogSwipe(NULL, _("CANCEL"), _("APPROVE"), _("CONFIRM SIGNING:"),
                    _("Transaction:"), _("Transaction data cannot be decoded"),
                    NULL, _("Sign at you own risk"), NULL, NULL);
}

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

  layoutRequireConfirmTxSigner(address);
  if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled,
                    "Signing cancelled by user");
    layoutHome();
    return;
  }

  layoutRequireConfirmRisk();
  if (!protectButton(ButtonRequestType_ButtonRequest_SignTx, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled,
                    "Signing cancelled by user");
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
