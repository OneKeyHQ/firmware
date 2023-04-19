
#ifndef __KASPA_H__
#define __KASPA_H__
#include "bip32.h"
#include "messages-kaspa.pb.h"

void kaspa_get_address(uint8_t *pubkey, const uint8_t pubkey_len,
                       const char *prefix, char *addr);
void kaspa_sign_sighash(HDNode *node, const uint8_t *raw_message,
                        uint32_t raw_message_len, uint8_t *signature,
                        pb_size_t *signature_len);
void kaspa_signing_init(const KaspaSignTx *msg);
void kaspa_signing_abort(void);

extern uint16_t input_count;
extern uint16_t input_index;
#endif  // __KASPA_H__
