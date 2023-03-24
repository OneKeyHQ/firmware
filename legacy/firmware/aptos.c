
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

static const char *MESSAGE_PREFIX = "APTOS\n";

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
  data2hexaddr((const uint8_t *)buf, SIZE_PUBKEY, address + 2);
}

void aptos_sign_tx(const AptosSignTx *msg, const HDNode *node,
                   AptosSignedTx *resp) {
  char address[67] = {0};
  aptos_get_address_from_public_key(node->public_key + 1, address);

  if (!layoutBlindSign("Aptos", false, NULL, address, msg->raw_tx.bytes,
                       msg->raw_tx.size, NULL, NULL, NULL, NULL, NULL, NULL)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled,
                    "Signing cancelled by user");
    layoutHome();
    return;
  }

  uint8_t buf[sizeof(AptosSignTx_raw_tx_t) + 32] = {0};
  memcpy(buf, APTOS_RAW_TX_PREFIX, 32);
  memcpy(buf + 32, msg->raw_tx.bytes, msg->raw_tx.size);
  ed25519_sign(buf, msg->raw_tx.size + 32, node->private_key,
               resp->signature.bytes);
  memcpy(resp->public_key.bytes, node->public_key + 1, 32);
  resp->signature.size = 64;
  resp->public_key.size = 32;
  msg_write(MessageType_MessageType_AptosSignedTx, resp);
}

void aptos_sign_message(const AptosSignMessage *msg, const HDNode *node,
                        AptosMessageSignature *resp) {
  AptosMessagePayload payload = msg->payload;
  char full_message[sizeof(AptosMessagePayload) + 58] = {0};

  strcat(full_message, MESSAGE_PREFIX);
  if (payload.has_address) {
    char *address = payload.address;
    strcat(full_message, "address: ");
    strcat(full_message, address);
    strcat(full_message, "\n");
  }
  if (payload.has_application) {
    char *application = payload.application;
    strcat(full_message, "application: ");
    strcat(full_message, application);
    strcat(full_message, "\n");
  }
  if (payload.has_chain_id) {
    char *chain_id = payload.chain_id;
    strcat(full_message, "chainId: ");
    strcat(full_message, chain_id);
    strcat(full_message, "\n");
  }
  char *message = payload.message;
  strcat(full_message, "message: ");
  strcat(full_message, message);
  strcat(full_message, "\n");
  char *nonce = payload.nonce;
  strcat(full_message, "nonce: ");
  strcat(full_message, nonce);

  aptos_get_address_from_public_key(node->public_key + 1, resp->address);
  // display here
  if (!fsm_layoutSignMessage("Aptos", resp->address,
                             (const uint8_t *)full_message,
                             strlen(full_message))) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  ed25519_sign((const uint8_t *)full_message, strlen(full_message),
               node->private_key, resp->signature.bytes);

  resp->signature.size = 64;
  msg_write(MessageType_MessageType_AptosMessageSignature, resp);
}
