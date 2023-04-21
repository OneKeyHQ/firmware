#include "kaspa.h"
#include "blake2b.h"
#include "cash_addr.h"
#include "curves.h"
#include "fsm.h"
#include "layout2.h"
#include "memzero.h"
#include "secp256k1.h"
#include "sha2.h"
#include "transaction.h"
#include "zkp_bip340.h"

#define PUBKEY_VERSION 0
#define PUBKEY_ECDSA_VERSION 1
// #define SCRIP_HASH_VERSION 8
// schnorr pubkey is 32 bytes
#define PUBKEY_LEN 32
// ecdsa pubkey is 33 bytes
#define PUBKEY_ECDSA_LEN 33
static const char *TRANSACTION_SIGNING_DOMAIN = "TransactionSigningHash";
static const uint8_t TRANSACTION_SIGNING_ECDSA_DOMAIN_HASH[32] = {
    164, 242, 236, 228, 90, 40, 108, 177, 236, 10, 78,  77, 56,  52,  104, 208,
    0,   247, 23,  87,  5,  43, 21,  4,   170, 52, 149, 50, 141, 245, 244, 234};

uint16_t input_count;
uint16_t input_index;
static bool kaspa_signing = false;
static char schema[8] = {0};
static char prefix[10] = {0};
// static char *allowed_prefixs[] = {"kaspa", "kaspatest", "kaspasim",
// "kaspadev"};
static char previous_address[72] = {0};

#define CALCULATE_SIGNING_HASH(pre_image, pre_image_len)           \
  uint8_t schnorr_digest[32] = {0};                                \
  BLAKE2B_CTX ctx;                                                 \
  blake2b_InitKey(&ctx, 32, (uint8_t *)TRANSACTION_SIGNING_DOMAIN, \
                  strlen(TRANSACTION_SIGNING_DOMAIN));             \
  blake2b_Update(&ctx, pre_image, pre_image_len);                  \
  blake2b_Final(&ctx, schnorr_digest, 32);

#define CALCULATE_SIGNING_HASH_ECDSA                                  \
  uint8_t ecdsa_digest[32] = {0};                                     \
  SHA256_CTX sha_ctx;                                                 \
  sha256_Init(&sha_ctx);                                              \
  sha256_Update(&sha_ctx, TRANSACTION_SIGNING_ECDSA_DOMAIN_HASH, 32); \
  sha256_Update(&sha_ctx, schnorr_digest, 32);                        \
  sha256_Final(&sha_ctx, ecdsa_digest);

void kaspa_signing_init(const KaspaSignTx *msg) {
  kaspa_signing = true;
  input_count = msg->input_count;
  input_index = 1;
  memcpy(schema, msg->schema, sizeof(msg->schema));
  memcpy(prefix, msg->prefix, sizeof(msg->prefix));
}

void kaspa_signing_abort(void) {
  if (kaspa_signing) {
    kaspa_signing = false;
    input_count = 0;
    input_index = 0;
    memzero(schema, sizeof(schema));
    memzero(prefix, sizeof(prefix));
    memzero(previous_address, sizeof(previous_address));
    layoutHome();
  }
}
static bool show_confirm_signing(const char *address, uint8_t address_len) {
  if (strcmp(address, previous_address) == 0) {
    return false;
  } else {
    memcpy(previous_address, address, address_len);
    return true;
  }
}
void kaspa_get_address(uint8_t *pubkey, const uint8_t pubkey_len,
                       const char *addr_prefix, char *addr) {
  uint8_t payload[pubkey_len + 1];
  if (pubkey_len == PUBKEY_ECDSA_LEN) {
    payload[0] = PUBKEY_ECDSA_VERSION;
  } else if (pubkey_len == PUBKEY_LEN) {
    payload[0] = PUBKEY_VERSION;
    uint8_t tweaked_pubkey[32];
    zkp_bip340_tweak_public_key(pubkey, NULL, tweaked_pubkey);
    memcpy(pubkey, tweaked_pubkey, sizeof(tweaked_pubkey));
  } else {
    fsm_sendFailure(FailureType_Failure_DataError, "Invalid pubkey length");
  }
  memcpy(payload + 1, pubkey, pubkey_len);
  cash_addr_encode(addr, addr_prefix, payload, sizeof(payload));
}

void kaspa_sign_sighash(HDNode *node, const uint8_t *raw_message,
                        uint32_t raw_message_len, uint8_t *signature,
                        pb_size_t *signature_len) {
  if (!kaspa_signing) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    "Not in Kaspa signing mode");
    layoutHome();
    return;
  }
  input_count--;
  char address[72] = {0};
  if (strcmp(schema, "schnorr") == 0) {
    kaspa_get_address(node->public_key + 1, PUBKEY_LEN, prefix, address);
  } else {
    kaspa_get_address(node->public_key, PUBKEY_ECDSA_LEN, prefix, address);
  }
  // show display
  if (show_confirm_signing(address, sizeof(address))) {
    if (!layoutBlindSign("Kaspa", false, NULL, address, raw_message,
                         raw_message_len, NULL, NULL, NULL, NULL, NULL, NULL)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled,
                      "Signing cancelled by user");
      kaspa_signing_abort();
      return;
    }
  }

  CALCULATE_SIGNING_HASH(raw_message, raw_message_len);
  if (strcmp(schema, "schnorr") == 0) {
    // schnorr sign
    tx_sign_bip340(node->private_key, schnorr_digest, signature, signature_len);
  } else {
    // ecdsa sign
    CALCULATE_SIGNING_HASH_ECDSA
    tx_sign_ecdsa(&secp256k1, node->private_key, ecdsa_digest, signature,
                  signature_len);
  }
}
