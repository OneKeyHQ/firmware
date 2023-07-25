#include "nexa.h"
#include "cash_addr.h"
#include "crypto.h"
#include "curves.h"
#include "fsm.h"
#include "layout2.h"
#include "memzero.h"
#include "schnorr_bch.h"
#include "secp256k1.h"
#include "sha2.h"

#define PUBKEY_ECDSA_LEN 33
uint16_t input_count_nexa;
uint16_t input_index_nexa;
static bool nexa_signing = false;
static char prefix[9] = {0};

// VALID_PREFIXES = ['nexa', 'nexatest', 'nexareg'];

// function getTypeBits(type) {
//   switch (type) {
//   case 'P2PKH':
//     return 0;
//   case 'SCRIPT':
//     return 1<<3;
//   case 'TEMPLATE':
//     return 19<<3;
//   case 'GROUP':
//     return 11<<3;
//   default:
//     throw new ValidationError('Invalid type: ' + type + '.');
//   }
// }

static char previous_address[58] = {0};

void nexa_signing_init(const NexaSignTx *msg) {
  nexa_signing = true;
  input_count_nexa = msg->input_count;
  input_index_nexa = 1;
  memcpy(prefix, msg->prefix, sizeof(msg->prefix));
}

void nexa_signing_abort(void) {
  if (nexa_signing) {
    nexa_signing = false;
    input_count_nexa = 0;
    input_index_nexa = 0;
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
static void nexa_get_pubkeyhash(const uint8_t *pubkey, uint8_t *pubkeyhash) {
  uint8_t pubkey_ex[34] = {0};
  pubkey_ex[0] = PUBKEY_ECDSA_LEN;
  memcpy(pubkey_ex + 1, pubkey, PUBKEY_ECDSA_LEN);
  uint8_t h[HASHER_DIGEST_LENGTH] = {0};
  hasher_Raw(HASHER_SHA2_RIPEMD, pubkey_ex, 34, h);
  memcpy(pubkeyhash, h, 20);
}

void nexa_get_address(uint8_t *pubkey, const uint8_t pubkey_len,
                      const char *addr_prefix, char *addr) {
  if (!(strcmp(addr_prefix, "nexa") == 0 ||
        strcmp(addr_prefix, "nexatest") == 0 ||
        strcmp(addr_prefix, "nexareg") == 0)) {
    fsm_sendFailure(FailureType_Failure_DataError, "Invalid prefix");
  }
  uint8_t payload[25];
  if (pubkey_len == PUBKEY_ECDSA_LEN) {
    payload[0] = 152;   // version byte
    payload[1] = 0x17;  // len
    payload[2] = 0x00;  // OP_FALSE
    payload[3] = 0x51;  // OP_PUSHDATA1
    payload[4] = 0x14;  // OP_DATA_20
  } else {
    fsm_sendFailure(FailureType_Failure_DataError, "Invalid pubkey length");
  }
  nexa_get_pubkeyhash(pubkey, payload + 5);
  cash_addr_encode(addr, addr_prefix, payload, sizeof(payload));
}

void nexa_sign_sighash(HDNode *node, const uint8_t *raw_message,
                       uint32_t raw_message_len, uint8_t *signature,
                       pb_size_t *signature_len) {
  if (!nexa_signing) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage,
                    "Not in Nexa signing mode");
    layoutHome();
    return;
  }
  if (*(uint32_t *)&raw_message[raw_message_len - 4] != 0) {
    fsm_sendFailure(FailureType_Failure_DataError, "Invalid sighash type");
    nexa_signing_abort();
    return;
  }
  input_count_nexa--;
  char address[58] = {0};
  nexa_get_address(node->public_key, PUBKEY_ECDSA_LEN, prefix, address);
  // show display
  if (show_confirm_signing(address, sizeof(address))) {
    if (!layoutBlindSign("Nexa", false, NULL, address, raw_message,
                         raw_message_len, NULL, NULL, NULL, NULL, NULL, NULL)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled,
                      "Signing cancelled by user");
      nexa_signing_abort();
      return;
    }
  }
  uint8_t digest[32] = {0};
  Hasher ctx = {0};
  hasher_Init(&ctx, HASHER_SHA2D);
  hasher_Update(&ctx, raw_message, raw_message_len);
  hasher_Final(&ctx, digest);

  // schnorr sign
  if (schnorr_sign_digest(&secp256k1, node->private_key, digest, signature) !=
      0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, "Signing failed");
    nexa_signing_abort();
    return;
  }
  *signature_len = 64;
}

bool nexa_path_check(uint32_t address_n_count, const uint32_t *address_n) {
  if (address_n_count < 5) {
    return false;
  }
  if (address_n[0] != (PATH_HARDENED | 44) ||
      address_n[1] != (PATH_HARDENED | 29223) ||
      (address_n[2] < PATH_HARDENED)) {
    return false;
  }
  return true;
}
