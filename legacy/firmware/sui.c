#include "sui.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "memzero.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "stdint.h"
#include "util.h"

static bool sui_signing = false;
static uint32_t data_total, data_left;
static CONFIDENTIAL uint8_t privkey[32];
static uint8_t pubkey[32];
static BLAKE2B_CTX hash_ctx = {0};
static SuiTxRequest msg_tx_request;

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
  uint8_t digest[32] = {0};

  sui_get_address_from_public_key(node->public_key + 1, address);
  // INTENT_BYTES = b'\x00\x00\x00'
  if ((msg->raw_tx.bytes[0] != 0x00) && ((msg->raw_tx.bytes[1] != 0x00)) &&
      ((msg->raw_tx.bytes[2] != 0x00))) {
    fsm_sendFailure(FailureType_Failure_DataError, "Invalid raw tx");
    layoutHome();
  }

  BLAKE2B_CTX ctx;
  blake2b_Init(&ctx, 32);
  blake2b_Update(&ctx, msg->raw_tx.bytes, msg->raw_tx.size);
  blake2b_Final(&ctx, digest, 32);

  if (!layoutBlindSign("Sui", address)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled,
                    "Signing cancelled by user");
    layoutHome();
    return;
  }

  ed25519_sign(digest, 32, node->private_key, resp->signature.bytes);
  memcpy(resp->public_key.bytes, node->public_key + 1, 32);
  resp->signature.size = 64;
  resp->public_key.size = 32;
  msg_write(MessageType_MessageType_SuiSignedTx, resp);
}

static void uleb_encode(int num, uint8_t *num_bytes, int *len) {
  while (num > 0) {
    num_bytes[*len] = num & 127;
    if (num >>= 7) {
      num_bytes[*len] |= 128;
    }
    *len += 1;
  }
}

void sui_message_sign(const SuiSignMessage *msg, const HDNode *node,
                      SuiMessageSignature *resp) {
  uint8_t digest[32] = {0};
  uint8_t num_bytes[32] = {0x3, 0x0, 0x0};  // Personal Message
  int num_bytes_len = 3;

  uleb_encode(msg->message.size, num_bytes, &num_bytes_len);

  BLAKE2B_CTX ctx;
  blake2b_Init(&ctx, 32);
  blake2b_Update(&ctx, num_bytes, num_bytes_len);
  blake2b_Update(&ctx, msg->message.bytes, msg->message.size);
  blake2b_Final(&ctx, digest, 32);

  ed25519_sign(digest, 32, node->private_key, resp->signature.bytes);
  resp->signature.size = 64;
  msg_write(MessageType_MessageType_SuiMessageSignature, resp);
}

void sui_signing_abort(void) {
  if (sui_signing) {
    memzero(privkey, sizeof(privkey));
    layoutHome();
    sui_signing = false;
  }
}

static inline void hash_data(const uint8_t *buf, size_t size) {
  blake2b_Update(&hash_ctx, buf, size);
}

static void send_signature(void) {
  uint8_t digest[32] = {0};
  SuiSignedTx tx;

  blake2b_Final(&hash_ctx, digest, 32);

  ed25519_sign(digest, 32, privkey, tx.signature.bytes);
  memcpy(tx.public_key.bytes, pubkey, 32);
  tx.signature.size = 64;
  tx.public_key.size = 32;
  msg_write(MessageType_MessageType_SuiSignedTx, &tx);

  memzero(privkey, sizeof(privkey));
  sui_signing_abort();
}

static void send_request_chunk(void) {
  msg_tx_request.has_data_length = true;
  msg_tx_request.data_length = data_left <= 1024 ? data_left : 1024;
  msg_write(MessageType_MessageType_SuiTxRequest, &msg_tx_request);
}

void sui_signing_init(const SuiSignTx *msg, const HDNode *node) {
  char address[67] = {0};

  sui_signing = true;
  blake2b_Init(&hash_ctx, 32);

  sui_get_address_from_public_key(node->public_key + 1, address);
  // INTENT_BYTES = b'\x00\x00\x00'
  if ((msg->data_initial_chunk.bytes[0] != 0x00) &&
      ((msg->data_initial_chunk.bytes[1] != 0x00)) &&
      ((msg->data_initial_chunk.bytes[2] != 0x00))) {
    fsm_sendFailure(FailureType_Failure_DataError, "Invalid raw tx");
    sui_signing_abort();
    return;
  }
  if (!layoutBlindSign("Sui", address)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled,
                    "Signing cancelled by user");
    layoutHome();
    return;
  }

  memcpy(privkey, node->private_key, 32);
  memcpy(pubkey, node->public_key + 1, 32);

  hash_data(msg->data_initial_chunk.bytes, msg->data_initial_chunk.size);

  data_total = msg->data_length;
  data_left = data_total - msg->data_initial_chunk.size;
  if (data_left > 0) {
    send_request_chunk();
  } else {
    send_signature();
  }
}

void sui_signing_txack(SuiTxAck *tx) {
  if (!sui_signing) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    _("Not in sui signing mode"));
    layoutHome();
    return;
  }
  if (tx->data_chunk.size > data_left) {
    fsm_sendFailure(FailureType_Failure_DataError, _("Too much data"));
    sui_signing_abort();
    return;
  }
  if (data_left > 0 && tx->data_chunk.size == 0) {
    fsm_sendFailure(FailureType_Failure_DataError,
                    _("Empty data chunk received"));
    sui_signing_abort();
    return;
  }
  hash_data(tx->data_chunk.bytes, tx->data_chunk.size);

  data_left -= tx->data_chunk.size;

  if (data_left > 0) {
    send_request_chunk();
  } else {
    send_signature();
  }
}