#ifndef __FILECOIN_COIN_H__
#define __FILECOIN_COIN_H__

#include <stddef.h>
#include <stdint.h>

#define COIN_AMOUNT_DECIMAL_PLACES 18

#define COIN_SUPPORTED_TX_VERSION 0

#define CHECKSUM_LENGTH 4

#define PREFIX \
  { 0x01, 0x71, 0xa0, 0xe4, 0x02, 0x20 }

#define ADDRESS_PROTOCOL_ID 0x00
#define ADDRESS_PROTOCOL_SECP256K1 0x01
#define ADDRESS_PROTOCOL_ACTOR 0x02
#define ADDRESS_PROTOCOL_BLS 0x03
#define ADDRESS_PROTOCOL_DELEGATED 0x04

#define ADDRESS_PROTOCOL_ID_PAYLOAD_LEN 0x00
#define ADDRESS_PROTOCOL_SECP256K1_PAYLOAD_LEN 20
#define ADDRESS_PROTOCOL_ACTOR_PAYLOAD_LEN 20
#define ADDRESS_PROTOCOL_BLS_PAYLOAD_LEN 48
#define ADDRESS_PROTOCOL_DELEGATED_MAX_SUBADDRESS_LEN 54

extern uint16_t decompressLEB128(const uint8_t *input, uint16_t inputSize,
                                 uint64_t *v);
extern uint16_t formatProtocol(const uint8_t *addressBytes,
                               uint16_t addressSize, uint8_t *formattedAddress,
                               uint16_t formattedAddressSize);

#endif
