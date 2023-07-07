#ifndef _SE_THD89_H_
#define _SE_THD89_H_

#include "secbool.h"

#include "bip32.h"

#define IN
#define OUT

#define SESSION_KEYLEN (16)
#define DEFAULT_SECKEYADDR (0x0800F000UL)

#define PUBLIC_REGION_SIZE (0x600)   // 1.5KB
#define PRIVATE_REGION_SIZE (0x200)  // 0.5KB
#define SE_PRIVATE_REGION_BASE PUBLIC_REGION_SIZE

#define SE_WRFLG_RESET (0x00)                     // se reset on device
#define SE_WRFLG_SETPIN (0x00)                    // se set pin
#define SE_WRFLG_CHGPIN (0x01)                    // se change pin
#define SE_WRFLG_GENSEED (0x00)                   // se generate seed
#define SE_WRFLG_GENMINISECRET (0x01)             // se generate minisecret
#define SE_WRFLG_MNEMONIC SE_WRFLG_GENMINISECRET  // se set mnemonic
#define SE_WRFLG_ENTROPY (0x02)                   // se set entropy
#define SE_GENSEDMNISEC_FIRST (0xff)              // for first generate
#define SE_GENSEDMNISEC_OTHER (0x01)

#define SE_GENERATE_SEED_MAX_STEPS 100  // [1, 100] // total 100 steps

// pin timeout disable
#define SE_PIN_TIMEOUT_MAX 0xFF

typedef enum {
  TYPE_SEED = 0x00,               /* BIP32 seed */
  TYPE_MINI_SECRET = 0x01,        /* polkadot mini secret */
  TYPE_ICARUS_MAIN_SECRET = 0x02, /* cardano icarus main secret */
  TYPE_ICARUS_EXT_SECRET = 0x03,  /* cardano icarus extension main secret */
  // CARDANO_LEDGER_SECRET, /* ledger cardano secret, does this need? */
} se_generate_type_t;

typedef struct {
  bool se_seed_status;
  bool se_minisecret_status;
  bool se_icarus_status;
} se_session_cached_status;

typedef enum {
  PROCESS_BEGIN = 0xFF,
  PROCESS_GENERATING = 0x01,
} se_generate_process_t;

typedef enum {
  STATE_FAILD,
  STATE_GENERATING,
  STATE_COMPLETE,
} se_generate_state_t;

typedef struct {
  se_generate_type_t type;
  se_generate_process_t processing;
} se_generate_session_t;

typedef secbool (*UI_WAIT_CALLBACK)(uint32_t wait, uint32_t progress,
                                    const char *message);
void se_set_ui_callback(UI_WAIT_CALLBACK callback);

secbool generate_seed_steps(void);

secbool se_sync_session_key(void);
secbool se_device_init(uint8_t mode, const char *passphrase);
secbool se_ecdsa_get_pubkey(uint32_t *address, uint8_t count, uint8_t *pubkey);
secbool se_set_value(uint16_t key, const void *val_dest, uint16_t len);
secbool se_get_value(uint16_t key, void *val_dest, uint16_t max_len,
                     uint16_t *len);
void se_reset_storage(void);
secbool se_get_sn(char **serial, uint16_t len);
char *se_get_version(void);
secbool se_isInitialized(void);
secbool se_hasPin(void);
secbool se_setPin(const char *pin);
secbool se_verifyPin(const char *pin);
secbool se_changePin(const char *oldpin, const char *newpin);
uint32_t se_pinFailedCounter(void);
secbool se_getRetryTimes(uint8_t *ptimes);
secbool se_clearSecsta(void);
secbool se_setPinValidtime(uint8_t data);
secbool se_getPinValidtime(uint8_t *data_buf);
secbool se_applyPinValidtime(void);
secbool se_getSecsta(void);
secbool se_isFactoryMode(void);
secbool se_isLifecyComSta(void);
secbool se_set_u2f_counter(uint32_t u2fcounter);
secbool se_get_u2f_counter(uint32_t *u2fcounter);
secbool se_get_entropy(uint8_t entropy[32]);
secbool se_set_entropy(const void *entropy, uint16_t len);
secbool se_set_mnemonic(const void *mnemonic, uint16_t len);
secbool se_sessionStart(OUT uint8_t *session_id_bytes);
secbool se_sessionOpen(IN uint8_t *session_id_bytes);
// generateing secret when create/recover wallet
se_generate_state_t se_beginGenerate(se_generate_type_t type,
                                     se_generate_session_t *session);
se_generate_state_t se_generating(se_generate_session_t *session);

// generateing secret when use passprase session
se_generate_state_t se_sessionBeginGenerate(const uint8_t *passphase,
                                            uint16_t len,
                                            se_generate_type_t type,
                                            se_generate_session_t *session);
se_generate_state_t se_sessionGenerating(se_generate_session_t *session);

secbool se_getSessionCachedState(se_session_cached_status *status);

secbool se_sessionClose(void);
secbool se_sessionClear(void);
secbool se_set_public_region(uint16_t offset, const void *val_dest,
                             uint16_t len);
secbool se_get_public_region(uint16_t offset, void *val_dest, uint16_t len);
secbool se_set_private_region(uint16_t offset, const void *val_dest,
                              uint16_t len);
secbool se_get_private_region(uint16_t offset, void *val_dest, uint16_t len);
secbool se_schnoor_sign_plain(const uint8_t *data, uint16_t data_len,
                              uint8_t *sig);
secbool se_aes_128_encrypt(uint8_t mode, uint8_t *key, uint8_t *iv,
                           uint8_t *send, uint16_t send_len, uint8_t *recv,
                           uint16_t *recv_len);
secbool se_aes_128_decrypt(uint8_t mode, uint8_t *key, uint8_t *iv,
                           uint8_t *send, uint16_t send_len, uint8_t *recv,
                           uint16_t *recv_len);

secbool se_get_pubkey(uint8_t pubkey[64]);
secbool se_write_certificate(const uint8_t *cert, uint32_t cert_len);
secbool se_read_certificate(uint8_t *cert, uint32_t *cert_len);
secbool se_has_cerrificate(void);
secbool se_sign_message(uint8_t *msg, uint32_t msg_len, uint8_t *signature);

secbool se_containsMnemonic(const char *mnemonic);
secbool se_hasWipeCode(void);
secbool se_changeWipeCode(const char *wipe_code);

uint8_t *se_session_startSession(const uint8_t *received_session_id);
secbool se_gen_session_seed(const char *passphrase);
secbool se_derive_keys(HDNode *out, const char *curve,
                       const uint32_t *address_n, size_t address_n_count,
                       uint32_t *fingerprint);
int se_ecdsa_sign_digest(const char *curve, const uint8_t *digest, uint8_t *sig,
                         uint8_t *pby, int (*is_canonical)(uint8_t, uint8_t *));

#include "curves.h"

#define se_secp256k1_sign_digest(hash, sig, v, is_canonical) \
  se_ecdsa_sign_digest(SECP256K1_NAME, hash, sig, v, is_canonical)
#define se_nist256p1_sign_digest(hash, sig, v, is_canonical) \
  se_ecdsa_sign_digest(NIST256P1_NAME, hash, sig, v, is_canonical)

int se_ed25519_sign(const uint8_t *msg, uint16_t msg_len, uint8_t *sig);

int se_get_shared_key(const char *curve, const uint8_t *peer_public_key,
                      uint8_t *session_key);

secbool se_derive_tweak_private_keys(void);
int se_bip340_sign_digest(const uint8_t *digest, uint8_t sig[64]);
int se_bip340_ecdh(const uint8_t *peer_public_key, uint8_t session_key[65]);

secbool se_setCoinJoinAuthorization(const uint8_t *authorization, uint16_t len);
secbool se_getCoinJoinAuthorization(uint8_t *authorization, uint16_t *len);

#endif