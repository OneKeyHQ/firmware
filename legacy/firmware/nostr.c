#include "nostr.h"
#include <stdio.h>
#include "aes/aes.h"
#include "algo/base64.h"
#include "base64.h"
#include "cosmos/json_parser.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
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

extern int ethereum_is_canonic(uint8_t v, uint8_t signature[64]);
extern int convert_bits(uint8_t *out, size_t *outlen, int outbits,
                        const uint8_t *in, size_t inlen, int inbits, int pad);

int nostr_get_pubkey(char *address, const uint8_t *public_key) {
  uint8_t data[65] = {0};
  size_t datalen = 0;

  // bech32_encode
  bech32_encoding enc = BECH32_ENCODING_BECH32;
  convert_bits(data, &datalen, 5, public_key, 32, 8, 1);

  return bech32_encode(address, "npub", data, datalen, enc);
}

bool nostr_sign_event(const NostrSignEvent *msg, const HDNode *node,
                      NostrSignedEvent *resp) {
  parsed_json_t json;
  parser_error_t err;
  uint16_t req_root_item_key_token_idx = 0;
  uint8_t unsignedEvent_bytes[2048] = {0};
  int unsignedEvent_index = 0;
  char sig[129] = {0};
  char id[65] = {0};
  char pk[65] = {0};
  char npub[92] = {0};
  int tlen;

  nostr_get_pubkey(npub, node->public_key + 1);
  if (!fsm_layoutSignMessage(msg->event.bytes, msg->event.size)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return false;
  }

  data2hexaddr(node->public_key + 1, 32, pk);

  err = json_parse(&json, (const char *)msg->event.bytes, msg->event.size);
  if (err != parser_ok) {
    return err;
  }

  // [0, pubkey, j["created_at"], j["kind"], j["tags"], j["content"]]
  strcat((char *)unsignedEvent_bytes, "[0,");
  unsignedEvent_index += 3;

  // pubkey
  unsignedEvent_bytes[unsignedEvent_index++] = '"';
  data2hexaddr(node->public_key + 1, 32,
               (char *)(unsignedEvent_bytes + unsignedEvent_index));
  unsignedEvent_index += 64;
  unsignedEvent_bytes[unsignedEvent_index++] = '"';
  unsignedEvent_bytes[unsignedEvent_index++] = ',';

  // created_at
  err = object_get_value(&json, 0, "created_at", &req_root_item_key_token_idx);
  if (err != parser_ok) {
    fsm_sendFailure(FailureType_Failure_DataError, "created_at unexist");
    layoutHome();
    return false;
  }
  tlen = json.tokens[req_root_item_key_token_idx].end -
         json.tokens[req_root_item_key_token_idx].start;
  memcpy(unsignedEvent_bytes + unsignedEvent_index,
         msg->event.bytes + json.tokens[req_root_item_key_token_idx].start,
         tlen);
  unsignedEvent_index += tlen;
  unsignedEvent_bytes[unsignedEvent_index++] = ',';

  // kind
  err = object_get_value(&json, 0, "kind", &req_root_item_key_token_idx);
  if (err != parser_ok) {
    fsm_sendFailure(FailureType_Failure_DataError, "kind unexist");
    layoutHome();
    return false;
  }
  tlen = json.tokens[req_root_item_key_token_idx].end -
         json.tokens[req_root_item_key_token_idx].start;
  memcpy(unsignedEvent_bytes + unsignedEvent_index,
         msg->event.bytes + json.tokens[req_root_item_key_token_idx].start,
         tlen);
  unsignedEvent_index += tlen;
  unsignedEvent_bytes[unsignedEvent_index++] = ',';

  // tags
  err = object_get_value(&json, 0, "tags", &req_root_item_key_token_idx);
  if (err != parser_ok) {
    fsm_sendFailure(FailureType_Failure_DataError, "tags unexist");
    layoutHome();
    return false;
  }
  tlen = json.tokens[req_root_item_key_token_idx].end -
         json.tokens[req_root_item_key_token_idx].start;
  memcpy(unsignedEvent_bytes + unsignedEvent_index,
         msg->event.bytes + json.tokens[req_root_item_key_token_idx].start,
         tlen);
  unsignedEvent_index += tlen;
  unsignedEvent_bytes[unsignedEvent_index++] = ',';

  // content
  err = object_get_value(&json, 0, "content", &req_root_item_key_token_idx);
  if (err != parser_ok) {
    fsm_sendFailure(FailureType_Failure_DataError, "content unexist");
    layoutHome();
    return false;
  }
  unsignedEvent_bytes[unsignedEvent_index++] = '"';
  tlen = json.tokens[req_root_item_key_token_idx].end -
         json.tokens[req_root_item_key_token_idx].start;
  memcpy(unsignedEvent_bytes + unsignedEvent_index,
         msg->event.bytes + json.tokens[req_root_item_key_token_idx].start,
         tlen);
  unsignedEvent_index += tlen;
  unsignedEvent_bytes[unsignedEvent_index++] = '"';
  unsignedEvent_bytes[unsignedEvent_index++] = ']';

  uint8_t hash[32];
  uint8_t signature[64];
  pb_size_t sig_len = 0;
  SHA256_CTX ctx;
  sha256_Init(&ctx);
  sha256_Update(&ctx, unsignedEvent_bytes, unsignedEvent_index);
  sha256_Final(&ctx, hash);

  data2hexaddr(hash, 32, id);

  tx_sign_bip340_internal(node->private_key, hash, signature, &sig_len);
  data2hexaddr(signature, 64, sig);

  resp->event.size =
      snprintf((char *)resp->event.bytes, 512,
               "{\"pubkey\":\"%s\",\"id\":\"%s\",\"sig\":\"%s\"}", pk, id, sig);
  return true;
}

bool nostr_encrypt_message(NostrEncryptMessage *msg, const HDNode *node,
                           NostrEncryptedMessage *resp) {
  uint8_t shared_secret[33] = {0};
  uint8_t pk[33] = {0};
  unsigned int pk_len = 0;
  pk[0] = 0x02;

  if (msg->has_show_display && msg->show_display) {
    if (!fsm_layoutSignMessage_ex(_("Encrypt Message"),
                                  (const uint8_t *)msg->msg,
                                  strlen(msg->msg))) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return false;
    }
  }

  hex2data(msg->pubkey, pk + 1, &pk_len);
  ecdh_multiply(&secp256k1, node->private_key, pk, shared_secret);

  uint8_t iv[16] = {0};
  char iv_b64[64] = {0};

  random_buffer(iv, 16);
  base64_encode((char *)iv, 16, iv_b64, sizeof(iv_b64));

  int msg_len = strlen(msg->msg);
  int pad_len = 16 - (msg_len % 16);
  for (int i = 0; i < pad_len; i++) {
    msg->msg[msg_len++] = pad_len;
  }

  aes_encrypt_ctx ctx = {0};
  uint8_t encrypted[1563] = {0};
  int ret = aes_encrypt_key256(shared_secret + 1, &ctx);
  if (ret != EXIT_SUCCESS) {
    fsm_sendFailure(FailureType_Failure_DataError, "aes_encrypt_key256 failed");
    layoutHome();
    return false;
  }
  if (aes_cbc_encrypt((uint8_t *)msg->msg, encrypted, msg_len, iv, &ctx) !=
      EXIT_SUCCESS) {
    fsm_sendFailure(FailureType_Failure_DataError, "aes_cbc_encrypt failed");
    return false;
  }

  base64_encode((char *)encrypted, msg_len, (char *)resp->msg,
                sizeof(resp->msg));
  ret = strlen(resp->msg);
  memcpy(resp->msg + ret, "?iv=", 4);
  memcpy(resp->msg + ret + 4, iv_b64, strlen(iv_b64));

  return true;
}

bool nostr_decrypt_message(NostrDecryptMessage *msg, const HDNode *node,
                           NostrDecryptedMessage *resp) {
  uint8_t shared_secret[33] = {0};
  uint8_t pk[33] = {0};
  uint8_t ct[1563] = {0};
  uint8_t iv[16] = {0};
  unsigned int pk_len = 0;
  int ct_len, ret, pad_val;

  if (msg->has_show_display && msg->show_display) {
    if (!fsm_layoutSignMessage_ex(_("Decrypt Message"),
                                  (const uint8_t *)msg->msg,
                                  strlen(msg->msg))) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return false;
    }
  }

  pk[0] = 0x02;
  hex2data(msg->pubkey, pk + 1, &pk_len);
  ecdh_multiply(&secp256k1, node->private_key, pk, shared_secret);

  char *iv_b64 = strstr(msg->msg, "?iv=");
  if (!iv_b64) {
    fsm_sendFailure(FailureType_Failure_DataError, "iv null");
    layoutHome();
    return false;
  }
  iv_b64[0] = '\0';
  iv_b64 += 4;

  char *end = b64tobin(iv, iv_b64);
  if (!end) {
    fsm_sendFailure(FailureType_Failure_DataError, "iv error");
    layoutHome();
    return false;
  }
  end = b64tobin(ct, msg->msg);
  if (!end) {
    fsm_sendFailure(FailureType_Failure_DataError, "ct error");
    layoutHome();
    return false;
  }
  ct_len = end - (char *)ct;

  aes_decrypt_ctx ctx = {0};
  ret = aes_decrypt_key256(shared_secret + 1, &ctx);
  if (ret != EXIT_SUCCESS) {
    fsm_sendFailure(FailureType_Failure_DataError, "aes_encrypt_key256 failed");
    layoutHome();
    return false;
  }

  if (aes_cbc_decrypt(ct, (unsigned char *)resp->msg, ct_len, iv, &ctx) !=
      EXIT_SUCCESS) {
    fsm_sendFailure(FailureType_Failure_DataError, "aes_cbc_decrypt failed");
    return false;
  }
  ct_len = strlen(resp->msg);
  pad_val = resp->msg[ct_len - 1];
  if (pad_val <= 0xf) {
    resp->msg[ct_len - pad_val] = '\0';
  }
  return true;
}

bool nostr_sign_schnorr(const NostrSignSchnorr *msg, const HDNode *node,
                        NostrSignedSchnorr *resp) {
  uint8_t hash[32] = {0};
  char npub[92] = {0};
  unsigned int len = 0;
  pb_size_t sig_len = 0;

  hex2data(msg->hash, hash, &len);
  nostr_get_pubkey(npub, node->public_key + 1);

  if (!fsm_layoutSignMessage_ex("Nostr sign hash?", (const uint8_t *)msg->hash,
                                strlen(msg->hash))) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return false;
  }

  tx_sign_bip340_internal(node->private_key, hash, resp->signature.bytes,
                          &sig_len);
  resp->signature.size = sig_len;

  return true;
}
