#ifndef _SE_THD89_H_
#define _SE_THD89_H_

#include "bip32.h"
#include "secbool.h"

#define SESSION_KEYLEN (16)

#define PUBLIC_REGION_SIZE (0x800)   // 2KB
#define PRIVATE_REGION_SIZE (0x800)  // 2KB

#define MAX_AUTHORIZATION_LEN 128

typedef secbool (*UI_WAIT_CALLBACK)(uint32_t wait, uint32_t progress,
                                    const char *message);
void se_set_ui_callback(UI_WAIT_CALLBACK callback);

secbool se_transmit_mac(uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data,
                        uint16_t data_len, uint8_t *recv, uint16_t *recv_len);

secbool se_get_rand(uint8_t *rand, uint8_t rand_len);
secbool se_reset_se(void);
secbool se_random_encrypted(uint8_t *rand, uint16_t len);
secbool se_sync_session_key(void);
secbool se_device_init(uint8_t mode, const char *passphrase);
secbool se_ecdsa_get_pubkey(uint32_t *address, uint8_t count, uint8_t *pubkey);

secbool se_reset_storage(void);
secbool se_set_sn(const char *serial, uint8_t len);
secbool se_get_sn(char **serial);
char *se_get_version(void);
secbool se_isInitialized(void);
secbool se_hasPin(void);
secbool se_setPin(const char *pin);
secbool se_verifyPin(const char *pin);
secbool se_changePin(const char *oldpin, const char *newpin);
uint32_t se_pinFailedCounter(void);
secbool se_getRetryTimes(uint8_t *ptimes);
secbool se_clearSecsta(void);
secbool se_getSecsta(void);
secbool se_set_u2f_counter(uint32_t u2fcounter);
secbool se_get_u2f_counter(uint32_t *u2fcounter);
secbool se_set_mnemonic(const char *mnemonic, uint16_t len);
secbool se_sessionStart(uint8_t *session_id_bytes);
secbool se_sessionOpen(uint8_t *session_id_bytes);

secbool se_get_session_seed_state(uint8_t *state);
secbool se_session_is_open(void);

secbool se_sessionClose(void);
secbool se_sessionClear(void);
secbool se_set_public_region(uint16_t offset, const void *val_dest,
                             uint16_t len);
secbool se_get_public_region(uint16_t offset, void *val_dest, uint16_t len);
secbool se_set_private_region(uint16_t offset, const void *val_dest,
                              uint16_t len);
secbool se_get_private_region(uint16_t offset, void *val_dest, uint16_t len);

secbool se_get_pubkey(uint8_t *pubkey);
secbool se_write_certificate(const uint8_t *cert, uint16_t cert_len);
secbool se_read_certificate(uint8_t *cert, uint16_t *cert_len);
secbool se_has_cerrificate(void);
secbool se_sign_message(uint8_t *msg, uint32_t msg_len, uint8_t *signature);
secbool se_set_session_key(const uint8_t *session_key);

secbool se_containsMnemonic(const char *mnemonic);
secbool se_exportMnemonic(char *mnemonic, uint16_t dest_size);
secbool se_set_needs_backup(bool needs_backup);
secbool se_get_needs_backup(bool *needs_backup);
secbool se_hasWipeCode(void);
secbool se_changeWipeCode(const char *pin, const char *wipe_code);

uint8_t *se_session_startSession(const uint8_t *received_session_id);
secbool se_gen_session_seed(const char *passphrase, bool cardano);
secbool se_derive_keys(HDNode *out, const char *curve,
                       const uint32_t *address_n, size_t address_n_count,
                       uint32_t *fingerprint);
secbool se_node_sign_digest(const uint8_t *hash, uint8_t *sig, uint8_t *by);
int se_ecdsa_sign_digest(const uint8_t curve, const uint8_t canonical,
                         const uint8_t *digest, uint8_t *sig, uint8_t *pby);

int se_secp256k1_sign_digest(const uint8_t canonical, const uint8_t *digest,
                             uint8_t *sig, uint8_t *pby);
int se_nist256p1_sign_digest(const uint8_t *digest, uint8_t *sig, uint8_t *pby);

int se_ed25519_sign(const uint8_t *msg, uint16_t msg_len, uint8_t *sig);
int se_ed25519_sign_ext(const uint8_t *msg, uint16_t msg_len, uint8_t *sig);
int se_ed25519_sign_keccak(const uint8_t *msg, uint16_t msg_len, uint8_t *sig);

int se_get_shared_key(const char *curve, const uint8_t *peer_public_key,
                      uint8_t *session_key);

secbool se_derive_tweak_private_keys(const uint8_t *root_hash);
int se_bip340_sign_digest(const uint8_t *digest, uint8_t sig[64]);

int se_aes256_encrypt(const uint8_t *data, uint16_t data_len, const uint8_t *iv,
                      uint8_t *value, uint16_t value_len, uint8_t *out);
int se_aes256_decrypt(const uint8_t *data, uint16_t data_len, const uint8_t *iv,
                      uint8_t *value, uint16_t value_len, uint8_t *out);

int se_nem_aes256_encrypt(const uint8_t *ed25519_public_key, const uint8_t *iv,
                          const uint8_t *salt, uint8_t *payload, uint16_t size,
                          uint8_t *out);
int se_nem_aes256_decrypt(const uint8_t *ed25519_public_key, const uint8_t *iv,
                          const uint8_t *salt, uint8_t *payload, uint16_t size,
                          uint8_t *out);
int se_slip21_node(uint8_t *data);

secbool se_authorization_set(const uint32_t authorization_type,
                             const uint8_t *authorization,
                             uint32_t authorization_len);
secbool se_authorization_get_type(uint32_t *authorization_type);
secbool se_authorization_get_data(uint8_t *authorization_data,
                                  uint32_t *authorization_len);
void se_authorization_clear(void);

#endif