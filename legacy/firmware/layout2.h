/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LAYOUT2_H__
#define __LAYOUT2_H__

#include "bignum.h"
#include "bitmaps.h"
#include "chinese.h"
#include "coins.h"
#include "layout.h"
#include "oled.h"
#include "trezor.h"

#include "messages-bitcoin.pb.h"
#include "messages-crypto.pb.h"
#include "messages-management.pb.h"
#include "messages-nem.pb.h"

#define DISP_BUFSIZE (2048)
#define DISP_PAGESIZE (96)

extern void *layoutLast;

void layoutDialogSwipe(const BITMAP *icon, const char *btnNo,
                       const char *btnYes, const char *desc, const char *line1,
                       const char *line2, const char *line3, const char *line4,
                       const char *line5, const char *line6);
void layoutDialogSwipeEx(const BITMAP *icon, const char *btnNo,
                         const char *btnYes, const char *desc,
                         const char *line1, const char *line2,
                         const char *line3, const char *line4,
                         const char *line5, const char *line6, uint8_t font);
void layoutDialogSwipeWrapping(const BITMAP *icon, const char *btnNo,
                               const char *btnYes, const char *heading,
                               const char *description, const char *wrap_text);
void layoutProgressSwipe(const char *desc, int permil);

void layoutScreensaver(void);
void layoutHomescreen(void);
void layoutBusyscreen(void);
void layoutHome(void);
void layoutHomeEx(void);
bool layoutConfirmOutput(const CoinInfo *coin, AmountUnit amount_unit,
                         const TxOutputType *out);
void layoutConfirmOmni(const uint8_t *data, uint32_t size);
void layoutConfirmOpReturn(const uint8_t *data, uint32_t size);
bool layoutConfirmTx(const CoinInfo *coin, AmountUnit amount_unit,
                     uint64_t total_in, uint64_t external_in,
                     uint64_t total_out, uint64_t change_out,
                     uint64_t tx_weight);
void layoutConfirmReplacement(const char *description, uint8_t txid[32]);
void layoutConfirmModifyOutput(const CoinInfo *coin, AmountUnit amount_unit,
                               TxOutputType *out, TxOutputType *orig_out,
                               int page);
void layoutConfirmModifyFee(const CoinInfo *coin, AmountUnit amount_unit,
                            uint64_t fee_old, uint64_t fee_new,
                            uint64_t tx_weight);
void layoutFeeOverThreshold(const CoinInfo *coin, AmountUnit amount_unit,
                            uint64_t fee);
void layoutFeeRateOverThreshold(const CoinInfo *coin, uint32_t fee_per_kvbyte);
void layoutChangeCountOverThreshold(uint32_t change_count);
void layoutConfirmUnverifiedExternalInputs(void);
void layoutConfirmNondefaultLockTime(uint32_t lock_time,
                                     bool lock_time_disabled);
void layoutAuthorizeCoinJoin(const CoinInfo *coin, uint64_t max_rounds,
                             uint32_t max_fee_per_kvbyte);
void layoutConfirmCoinjoinAccess(void);
void layoutVerifyAddress(const CoinInfo *coin, const char *address);
void layoutCipherKeyValue(bool encrypt, const char *key);
void layoutEncryptMessage(const uint8_t *msg, uint32_t len, bool signing);
void layoutDecryptMessage(const uint8_t *msg, uint32_t len,
                          const char *address);
void layoutResetWord(const char *word, int pass, int word_pos, bool last);
uint8_t layoutAddress(const char *address, const char *address_type,
                      const char *desc, bool qrcode, bool path, bool ignorecase,
                      const uint32_t *address_n, size_t address_n_count,
                      bool address_is_account, bool is_multisig);
void layoutPublicKey(const uint8_t *pubkey);
bool layoutXPUB(const char *coin_name, const char *xpub,
                const uint32_t *address_n, size_t address_n_count);
uint8_t layoutXPUBMultisig(const char *header, const char *xpub, int index,
                           int page, bool ours, bool last_page);
void layoutSignIdentity(const IdentityType *identity, const char *challenge);
void layoutDecryptIdentity(const IdentityType *identity);
void layoutU2FDialog(const char *verb, const char *appname);
void layoutShowPassphrase(const char *passphrase);

void layoutNEMDialog(const BITMAP *icon, const char *btnNo, const char *btnYes,
                     const char *desc, const char *line1, const char *address);
void layoutNEMTransferXEM(const char *desc, uint64_t quantity,
                          const bignum256 *multiplier, uint64_t fee);
void layoutNEMNetworkFee(const char *desc, bool confirm, const char *fee1_desc,
                         uint64_t fee1, const char *fee2_desc, uint64_t fee2);
void layoutNEMTransferMosaic(const NEMMosaicDefinition *definition,
                             uint64_t quantity, const bignum256 *multiplier,
                             uint8_t network);
void layoutNEMTransferUnknownMosaic(const char *namespace, const char *mosaic,
                                    uint64_t quantity,
                                    const bignum256 *multiplier);
void layoutNEMTransferPayload(const uint8_t *payload, size_t length,
                              bool encrypted);
void layoutNEMMosaicDescription(const char *description);
void layoutNEMLevy(const NEMMosaicDefinition *definition, uint8_t network);

void layoutCosiSign(const uint32_t *address_n, size_t address_n_count,
                    const uint8_t *data, uint32_t len);

void layoutConfirmAutoLockDelay(uint32_t delay_ms);
bool layoutConfirmSafetyChecks(SafetyCheckLevel safety_checks_level);

void layoutConfirmHash(const BITMAP *icon, const char *description,
                       const uint8_t *hash, uint32_t len);

void layoutConfirmOwnershipProof(void);

const char **split_message(const uint8_t *msg, uint32_t len, uint32_t rowlen);
const char **split_message_hex(const uint8_t *msg, uint32_t len);
const char **format_tx_message(const char *chain_name);

bool is_valid_ascii(const uint8_t *data, uint32_t size);

void layoutQRCode(const char *index, const BITMAP *bmp_up,
                  const BITMAP *bmp_down, const char *title, const char *text);

void layoutHomeInfo(void);

void layoutButtonNoAdapter(const char *btnNo, const BITMAP *icon);
void layoutButtonYesAdapter(const char *btnYes, const BITMAP *icon);
void layoutScroollbarButtonYesAdapter(const char *btnYes, const BITMAP *icon);
void layoutDialogAdapter(const BITMAP *icon, const char *btnNo,
                         const char *btnYes, const char *desc,
                         const char *line1, const char *line2,
                         const char *line3, const char *line4,
                         const char *line5, const char *line6);
void layoutDialogAdapter_ex(const BITMAP *icon, const BITMAP *bmp_no,
                            const char *btnNo, const BITMAP *bmp_yes,
                            const char *btnYes, const char *desc,
                            const char *line1, const char *line2,
                            const char *line3, const char *line4,
                            const char *line5, const char *line6);
void layoutDialogCenterAdapter(const BITMAP *icon, const BITMAP *bmp_no,
                               const char *btnNo, const BITMAP *bmp_yes,
                               const char *btnYes, const char *desc,
                               const char *line1, const char *line2,
                               const char *line3, const char *line4,
                               const char *line5, const char *line6);
void layoutDialogAdapterEx(const char *title, const BITMAP *bmp_no,
                           const char *btnNo, const BITMAP *bmp_yes,
                           const char *btnYes, const char *desc,
                           const char *line1, const char *line2,
                           const char *line3, const char *line4);
void layoutDialogCenterAdapterV2(const char *title, const BITMAP *icon,
                                 const BITMAP *bmp_no, const BITMAP *bmp_yes,
                                 const BITMAP *bmp_up, const BITMAP *bmp_down,
                                 const char *line1, const char *line2,
                                 const char *line3, const char *line4,
                                 const char *desc);
void layoutProgressAdapter(const char *desc, int permil);

void layoutDialogSwipeCenterAdapter(const BITMAP *icon, const BITMAP *bmp_no,
                                    const char *btnNo, const BITMAP *bmp_yes,
                                    const char *btnYes, const char *desc,
                                    const char *line1, const char *line2,
                                    const char *line3, const char *line4,
                                    const char *line5, const char *line6);

void layoutItemsSelect(int x, int y, const char *text, uint8_t font);
void layoutItemsSelect_ex(int x, int y, const char *text, uint8_t font,
                          bool vert);

void layoutBmpSelect(int x, int y, const BITMAP *bmp);
void layoutItemsSelectAdapter(const BITMAP *bmp_up, const BITMAP *bmp_down,
                              const BITMAP *bmp_no, const BITMAP *bmp_yes,
                              const char *btnNo, const char *btnYes,
                              uint32_t index, uint32_t count, const char *title,
                              const char *prefex, const char *current,
                              const char *previous, const char *next);

void layoutItemsSelectAdapterEx(
    const BITMAP *bmp_up, const BITMAP *bmp_down, const BITMAP *bmp_no,
    const BITMAP *bmp_yes, const char *btnNo, const char *btnYes,
    uint32_t index, uint32_t count, const char *title, const char *input_desc,
    const char *current, const char *name2, const char *param,
    const char *previous, const char *pre_previous,
    const char *pre_pre_previous, const char *next, const char *next_next,
    const char *next_next_next, bool show_index);
void layoutItemsSelectAdapterWords(
    const BITMAP *bmp_up, const BITMAP *bmp_down, const BITMAP *bmp_no,
    const BITMAP *bmp_yes, const char *btnNo, const char *btnYes,
    uint32_t index, uint32_t count, const char *title, const char *input_desc,
    const char *current, const char *previous, const char *pre_previous,
    const char *pre_pre_previous, const char *next, const char *next_next,
    const char *next_next_next, bool show_index, bool is_select);
void layoutWords(const char *title, const BITMAP *bmp_up,
                 const BITMAP *bmp_down, const BITMAP *bmp_no,
                 const BITMAP *bmp_yes, uint32_t index, uint32_t count,
                 const char *word1, const char *word2, const char *word3,
                 const char *word4, const char *word5, const char *word6);

void layoutHeader(const char *text);

void layoutInputPin(uint8_t pos, const char *text, int index,
                    bool cancel_allowed);

void layoutInputWord(const char *text, uint8_t prefix_len, const char *prefix,
                     const char *letter);

void layoutInputMethod(uint8_t index);
void layoutInputPassphrase(const char *text, uint8_t prefix_len,
                           const char *prefix, uint8_t char_index,
                           uint8_t input_type);

bool layoutEraseDevice(void);
void layoutDeviceParameters(int num);
void layoutAboutCertifications(int num);
bool layoutEnterSleep(int mode);
bool layoutInputDirection(int index);

#define layoutMenuItems(btn_yes, bmp_yes, index, count, title, current,        \
                        previous, next)                                        \
  layoutItemsSelectAdapter(&bmp_btn_up, &bmp_btn_down, &bmp_btn_back, bmp_yes, \
                           _("Back"), btn_yes, index, count, title, NULL,      \
                           current, previous, next)

#define layoutMenuItemsEx(btn_yes, bmp_yes, index, count, title, desc,       \
                          current, name2, param, previous, pre_previous,     \
                          pre_pre_previous, next, next_next, next_next_next) \
  layoutItemsSelectAdapterEx(                                                \
      &bmp_bottom_middle_arrow_up, &bmp_bottom_middle_arrow_down,            \
      &bmp_bottom_left_arrow, bmp_yes, _("Back"), btn_yes, index, count,     \
      title, desc, current, name2, param, previous, pre_previous,            \
      pre_pre_previous, next, next_next, next_next_next, true)

uint8_t layoutStatusLogoEx(bool need_fresh, bool force_fresh);

static inline void oledClear_ex(void) {
  oledClear();
#if !EMULATOR
  layoutStatusLogoEx(false, true);
#endif
}

#if DEBUG_LINK
#define layoutSwipe oledClear
#else
static inline void layoutSwipe(void) {
  oledClear();
#if !EMULATOR
  layoutStatusLogoEx(false, true);
#endif
}
#endif

const char *address_n_str(const uint32_t *address_n, size_t address_n_count,
                          bool address_is_account);
bool layoutTransactionSign(const char *chain_name, uint64_t chain_id,
                           bool token_transfer, const char *amount,
                           const char *to_str, const char *signer,
                           const char *recipient, const char *token_id,
                           const uint8_t *data, uint16_t len, const char *key1,
                           const char *value1, const char *key2,
                           const char *value2, const char *key3,
                           const char *value3, const char *key4,
                           const char *value4);
bool layoutTransactionSignEVM(const char *chain_name, uint64_t chain_id,
                              bool token_transfer, const char *amount,
                              const char *to_str, const char *signer,
                              const char *recipient, const char *token_id,
                              const uint8_t *data, uint16_t len,
                              const char *key1, const char *value1,
                              const char *key2, const char *value2,
                              const char *key3, const char *value3,
                              const char *key4, const char *value4);
bool layoutBlindSign(const char *chain_name, bool is_contract,
                     const char *contract_addr, const char *from_str,
                     const uint8_t *data, uint16_t len, const char *key1,
                     const char *value1, const char *key2, const char *value2,
                     const char *key3, const char *value3);
bool layoutSignMessage(const char *chain_name, bool verify, const char *signer,
                       const uint8_t *data, uint16_t len, bool is_ascii);
bool layoutSignHash(const char *chain_name, bool verify, const char *signer,
                    const char *domain_hash, const char *message_hash,
                    const char *tips);
bool layoutPaginated(const char *title, const uint8_t *data, uint16_t len);

void onboarding(uint8_t key);
void hide_icons(bool hide);

#endif
