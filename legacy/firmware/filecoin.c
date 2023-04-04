
#include "filecoin.h"
#include "base32.h"
#include "buttons.h"
#include "filecoin/coin.h"
#include "filecoin/tx.h"
#include "font.h"
#include "fsm.h"
#include "gettext.h"
#include "layout2.h"
#include "messages.h"
#include "messages.pb.h"
#include "protect.h"
#include "secp256k1.h"
#include "stdint.h"
#include "util.h"

const char *BASE32_ALPHABET_RFC4648_S = "abcdefghijklmnopqrstuvwxyz234567";
bool filecoin_testnet = false;
extern int ethereum_is_canonic(uint8_t v, uint8_t signature[64]);

uint16_t decompressLEB128(const uint8_t *input, uint16_t inputSize,
                          uint64_t *v) {
  uint16_t i = 0;

  *v = 0;
  uint16_t shift = 0;
  while (i < 10u && i < inputSize) {
    uint64_t b = input[i] & 0x7fu;

    if (shift >= 63 && b > 1) {
      // This will overflow uint64_t
      break;
    }

    *v |= b << shift;

    if (!(input[i] & 0x80u)) {
      return i + 1;
    }

    shift += 7;
    i++;
  }

  // exit because of overflowing outputSize
  *v = 0;
  return 0;
}

uint16_t formatProtocol(const uint8_t *addressBytes, uint16_t addressSize,
                        uint8_t *formattedAddress,
                        uint16_t formattedAddressSize) {
  if (formattedAddress == NULL || formattedAddressSize < 2u) {
    return 0;
  }
  if (addressBytes == NULL || addressSize < 2u) {
    return 0;
  }

  // Clean output buffer
  memset(formattedAddress, 0, formattedAddressSize);

  const uint8_t protocol = addressBytes[0];

  if (filecoin_testnet) {
    formattedAddress[0] = 't';
  } else {
    formattedAddress[0] = 'f';
  }
  formattedAddress[1] = (char)(protocol + '0');

  uint16_t payloadSize = 0;
  switch (protocol) {
    case ADDRESS_PROTOCOL_ID: {
      uint64_t val = 0;

      if (!decompressLEB128(addressBytes + 1, addressSize - 1, &val)) {
        return 0;
      }

      if (uint64_to_str((char *)formattedAddress + 2,
                        (uint32_t)(formattedAddressSize - 2), val) != NULL) {
        return 0;
      }

      return strlen((const char *)formattedAddress);
    }
    case ADDRESS_PROTOCOL_SECP256K1: {  // NOLINT(bugprone-branch-clone)
      // payload 20 bytes + 4 bytes checksum
      payloadSize = ADDRESS_PROTOCOL_SECP256K1_PAYLOAD_LEN;
      break;
    }
    case ADDRESS_PROTOCOL_ACTOR: {  // NOLINT(bugprone-branch-clone)
      // payload 20 bytes + 4 bytes checksum
      payloadSize = ADDRESS_PROTOCOL_ACTOR_PAYLOAD_LEN;
      break;
    }
    case ADDRESS_PROTOCOL_BLS: {
      // payload 20 bytes + 4 bytes checksum
      payloadSize = ADDRESS_PROTOCOL_BLS_PAYLOAD_LEN;
      break;
    }
    case ADDRESS_PROTOCOL_DELEGATED: {
      uint64_t actorId = 0;
      const uint16_t actorIdSize =
          decompressLEB128(addressBytes + 1, addressSize - 1, &actorId);

      // Check missing actor id or missing sub-address
      if (actorIdSize == 0 || (addressSize <= actorIdSize + 1)) {
        return 0;
      }

      char actorId_str[25] = {0};
      if (uint64_to_str(actorId_str, sizeof(actorId_str), actorId) != NULL) {
        return 0;
      }
      // Copy Actor ID
      snprintf((char *)formattedAddress + 2, formattedAddressSize - 2, "%sf",
               actorId_str);

      payloadSize = addressSize - 1 - actorIdSize;
      break;
    }
    default:
      return 0;
  }

  // Keep only one crc buffer using the biggest size
  uint8_t payload_crc[ADDRESS_PROTOCOL_DELEGATED_MAX_SUBADDRESS_LEN +
                      CHECKSUM_LENGTH] = {0};

  // f4 addresses contain actorID
  const uint16_t actorIdSize = (protocol == ADDRESS_PROTOCOL_DELEGATED)
                                   ? (addressSize - payloadSize - 1)
                                   : 0;
  if (addressSize != payloadSize + 1 + actorIdSize) {
    return 0;
  }
  memcpy(payload_crc, addressBytes + 1 + actorIdSize, payloadSize);

  blake2b(addressBytes, addressSize, payload_crc + payloadSize,
          CHECKSUM_LENGTH);

  const uint16_t offset =
      strnlen((char *)formattedAddress, formattedAddressSize);

  if (base32_encode(payload_crc, (uint32_t)(payloadSize + CHECKSUM_LENGTH),
                    (char *)(formattedAddress + offset),
                    (uint32_t)(formattedAddressSize - offset),
                    BASE32_ALPHABET_RFC4648_S) == NULL) {
    return 0;
  }

  return strnlen((char *)formattedAddress, formattedAddressSize);
}

bool get_filecoin_addr(uint8_t *pubkey, FilecoinAddress *address) {
  uint8_t addrBytes[21] = {0};
  addrBytes[0] = ADDRESS_PROTOCOL_SECP256K1;
  blake2b(pubkey, 65, addrBytes + 1, 20);

  int len = formatProtocol(addrBytes, 21, (uint8_t *)address->address, 45);
  if (len == 0) return false;

  address->has_address = true;
  return true;
}

static bool layoutFilSign(void) {
  bool result = false;
  int index = 0, len, y = 0;
  uint8_t key = KEY_NULL;
  uint8_t numItems = 0;
  uint8_t max_index = 0;
  char token_key[64];
  char token_val[64];
  char desc[64] = {0};
  char *p;
  uint8_t pageCount = 0;
  const char **tx_msg = format_tx_message("Filecoin");

  ButtonRequest resp = {0};
  memzero(&resp, sizeof(ButtonRequest));
  resp.has_code = true;
  resp.code = ButtonRequestType_ButtonRequest_SignTx;
  msg_write(MessageType_MessageType_ButtonRequest, &resp);

  fil_tx_getNumItems(&numItems);
  max_index = numItems;
refresh_menu:
  layoutSwipe();
  oledClear();
  y = 13;
  if (index == max_index) {
    layoutHeader(_("Sign Transaction"));
    oledDrawStringAdapter(0, y, tx_msg[1], FONT_STANDARD);
    layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_confirm);
  } else {
    fil_tx_getItem(index, token_key, sizeof(token_key), token_val,
                   sizeof(token_val), 0, &pageCount);
    memset(desc, 0, 64);
    token_key[strlen(token_key) - 1] = '\0';  // remove ':'
    strcat(desc, _(token_key));
    strcat(desc, ":");
    if ((0 == index) || (4 == index) || (5 == index)) {
      p = strchr(token_val, '.');
      if (p) {
        len = strlen(token_val);
        for (int i = len - 1; i > 0; i--) {
          if (token_val[i] == '0') {
            token_val[i] = '\0';
          } else if (token_val[i] == '.') {
            token_val[i] = '\0';
            break;
          } else {
            break;
          }
        }
      }
      memcpy(token_val + strlen(token_val), " FIL", 5);
    }
    layoutHeader(tx_msg[0]);
    oledDrawStringAdapter(0, y, desc, FONT_STANDARD);
    oledDrawStringAdapter(0, y + 10, token_val, FONT_STANDARD);
    if (index == 0) {
      layoutButtonNoAdapter(NULL, &bmp_bottom_left_close);
    } else {
      layoutButtonNoAdapter(NULL, &bmp_bottom_left_arrow);
    }
    layoutButtonYesAdapter(NULL, &bmp_bottom_right_arrow);
  }
  oledRefresh();

scan_key:
  key = protectWaitKey(0, 0);
  switch (key) {
    case KEY_UP:
      goto scan_key;
    case KEY_DOWN:
      goto scan_key;
    case KEY_CONFIRM:
      if (index == max_index) {
        result = true;
        break;
      }
      if (index < max_index) {
        index++;
      }
      goto refresh_menu;
    case KEY_CANCEL:
      if ((0 == index) || (index == max_index)) {
        result = false;
        break;
      }
      if (index > 0) {
        index--;
      }
      goto refresh_menu;
    default:
      break;
  }

  return result;
}

bool filecoin_sign_tx(const FilecoinSignTx *msg, const HDNode *node,
                      FilecoinSignedTx *resp) {
  const char *error_msg = fil_tx_parse(msg->raw_tx.bytes, msg->raw_tx.size);
  if (error_msg) {
    fsm_sendFailure(FailureType_Failure_DataError, "Tx invalid");
    layoutHome();
    return false;
  }
  if (!layoutFilSign()) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, "Signing cancelled");
    layoutHome();
    return false;
  }

  uint8_t tmp[32] = {0};
  uint8_t message_digest[32];
  uint8_t prefix[] = PREFIX;
  BLAKE2B_CTX ctx;
  blake2b(msg->raw_tx.bytes, msg->raw_tx.size, tmp, 32);
  blake2b_Init(&ctx, 32);
  blake2b_Update(&ctx, prefix, 6);
  blake2b_Update(&ctx, tmp, 32);
  blake2b_Final(&ctx, message_digest, 32);

  char buf[256] = {0};
  data2hexaddr(message_digest, 32, buf);

  uint8_t v;
  if (ecdsa_sign_digest(&secp256k1, node->private_key, message_digest,
                        resp->signature.bytes, &v, ethereum_is_canonic) != 0) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Signing failed"));
    return false;
  }
  resp->signature.bytes[64] = v;
  resp->signature.size = 65;

  return true;
}
