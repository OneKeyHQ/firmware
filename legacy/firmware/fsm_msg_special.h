#include "ed25519-donna/ed25519.h"
#include "ed25519-donna/ed25519-donna.h"
#include "ed25519-donna/ed25519-hash-custom.h"

void fsm_msgSignDigest(const SignDigest *msg) {
  RESP_INIT(DigestSignature);

  CHECK_INITIALIZED

  layoutSignMessage(msg->digest.bytes, msg->digest.size);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  CHECK_PIN

  layoutProgressSwipe(_("Singing"), 0);

  const CoinInfo *coin = fsm_getCoin(msg->has_coin_name, msg->coin_name);
  HDNode *node = fsm_getDerivedNode(coin->curve_name, msg->address_n,
                                    msg->address_n_count, NULL);

  hdnode_sign_digest(node, msg->digest.bytes, resp->signature.bytes, NULL, NULL);
  resp->signature.size = 64;
  resp->has_signature = true;

  ecdsa_get_public_key65(node->curve->params, node->private_key, resp->pubkey.bytes);
  resp->pubkey.size = 65;

  msg_write(MessageType_MessageType_DigestSignature, resp);

  layoutHome();
}


void fsm_msgSignData(const SignData *msg) {
  RESP_INIT(DataSignature);

  CHECK_INITIALIZED

  sha256_Raw(msg->data.bytes, msg->data.size, resp->digest.bytes);
  resp->digest.size = 32;
  resp->has_digest = true;

  layoutSignMessage(resp->digest.bytes, resp->digest.size);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  CHECK_PIN

  layoutProgressSwipe(_("Singing"), 0);

  const CoinInfo *coin = fsm_getCoin(msg->has_coin_name, msg->coin_name);
  HDNode *node = fsm_getDerivedNode(coin->curve_name, msg->address_n,
                                    msg->address_n_count, NULL);

  hdnode_sign_digest(node, resp->digest.bytes, resp->signature.bytes, NULL, NULL);
  resp->signature.size = 64;
  resp->has_signature = true;

  ecdsa_get_public_key65(node->curve->params, node->private_key, resp->pubkey.bytes);
  resp->pubkey.size = 65;

  msg_write(MessageType_MessageType_DataSignature, resp);

  layoutHome();
}

void fsm_msgExportEd25519PublicKey(const ExportEd25519PublicKey *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(Ed25519PublicKey);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);

  if (!node) return;

  hdnode_fill_public_key(node);

  resp->pubkey.size = 32;

  memcpy(&resp->pubkey.bytes, &node->public_key[1],
         sizeof(resp->pubkey.bytes));

  resp->privkey.size = 32;
  memcpy(&resp->privkey.bytes, &node->private_key[0],
         sizeof(resp->privkey.bytes));

  msg_write(MessageType_MessageType_Ed25519PublicKey, resp);

  layoutHome();
}

void get_nonce_r(const uint8_t * privkey, const uint8_t * data, const size_t data_len, const uint32_t ctr,
    uint8_t * r) {
  uint8_t h[64];
  uint8_t d[256];
  size_t d_len;
  uint8_t ctr_be[4];

  sha512_Raw(privkey, 32, h);
  memcpy(d, &h[sizeof(h)/2], sizeof(h)/2);
  d_len = sizeof(h)/2;

  memcpy(d + d_len, data, data_len);
  d_len += data_len;

  ctr_be[0] = (uint8_t)(ctr >> 24);
  ctr_be[1] = (uint8_t)(ctr >> 16);
  ctr_be[2] = (uint8_t)(ctr >> 8);
  ctr_be[3] = (uint8_t)(ctr);
  memcpy(d + d_len, ctr_be, sizeof(ctr_be));
  d_len += sizeof(ctr_be);

  sha512_Raw(d, d_len, r);
}

void get_nonce_R_from_r(const uint8_t * r, uint8_t * R) {
  bignum256modm a = {0};
  ge25519 ALIGN(16) A;
  ed25519_public_key pk;
  expand256_modm(a, r, 64);

  ge25519_scalarmult_base_niels(&A, ge25519_niels_base_multiples, a);
  ge25519_pack(pk, &A);
  memcpy(R, &pk, sizeof(pk));
}

void fsm_msgGetEd25519Nonce(const GetEd25519Nonce *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(Ed25519Nonce);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  uint8_t r[64];
  uint8_t R[32];

  get_nonce_r(node->private_key, msg->data.bytes, msg->data.size, msg->ctr, r);
  get_nonce_R_from_r(r, R);

  resp->r.size = sizeof(r);
  memcpy(&resp->r.bytes, r, resp->r.size);

  resp->R.size = sizeof(R);
  memcpy(&resp->R.bytes, R, resp->R.size);

  resp->r_src.size = sizeof(r)/2 + msg->data.size + sizeof(msg->ctr);
  //memcpy(&resp->r_src.bytes, temp, resp->r_src.size);

  msg_write(MessageType_MessageType_Ed25519Nonce, resp);
}

inline static void
ed25519_extsk(hash_512bits extsk, const ed25519_secret_key sk) {
	ed25519_hash(extsk, sk, 32);
	extsk[0] &= 248;
	extsk[31] &= 127;
	extsk[31] |= 64;
}

inline static void
ed25519_hram(hash_512bits hram, const ed25519_signature RS, const ed25519_public_key pk, const unsigned char *m, size_t mlen) {
	ed25519_hash_context ctx;
	ed25519_hash_init(&ctx);
	ed25519_hash_update(&ctx, RS, 32);
	ed25519_hash_update(&ctx, pk, 32);
	ed25519_hash_update(&ctx, m, mlen);
	ed25519_hash_final(&ctx, hram);
}

static void
ed25519_cosign(const uint8_t * digest, const size_t digest_len,
    const uint8_t * privkey, const uint32_t ctr,
    const uint8_t * global_pubkey, const uint8_t * global_commit,
    uint8_t * sig) {
  bignum256modm r = {0}, S = {0}, a = {0};
  hash_512bits extsk = {0}, hram = {0};

  ed25519_extsk(extsk, privkey);

  /* r = nonce
     in order to cross-verify with trezorlib.cosi for conveniece,
     the calculation of r value follows the trezorlib.cosi method,
     it differs from ed25519_cosi_sign in ed25519-donna
  */
  uint8_t nonce[64];
  get_nonce_r(privkey, digest, digest_len, ctr, nonce);
  expand256_modm(r, nonce, 64);

  /* S = H(R,A,m).. */
  ed25519_hram(hram, global_commit, global_pubkey, digest, digest_len);
  expand256_modm(S, hram, 64);

  /* S = H(R,A,m)a */
  expand256_modm(a, extsk, 32);
  mul256_modm(S, S, a);

  /* S = (r + H(R,A,m)a) */
  add256_modm(S, S, r);

  /* S = (r + H(R,A,m)a) mod L */
  contract256_modm(sig, S);
}

void fsm_msgCosignEd25519(const CosignEd25519 *msg) {
  CHECK_INITIALIZED

  CHECK_PIN

  RESP_INIT(Ed25519Signature);

  HDNode *node = fsm_getDerivedNode(ED25519_NAME, msg->address_n,
                                    msg->address_n_count, NULL);
  if (!node) return;

  resp->sig.size = 64;
  ed25519_cosign(msg->digest.bytes, msg->digest.size,
    node->private_key, msg->ctr,
    msg->global_pubkey.bytes, msg->global_commit.bytes,
    resp->sig.bytes);

  msg_write(MessageType_MessageType_Ed25519Signature, resp);
}

void fsm_msgEd25519Verify(const Ed25519Verify *msg) {
  if (0 == ed25519_sign_open(msg->digest.bytes, msg->digest.size,
    msg->pubkey.bytes, msg->sig.bytes)) {
      fsm_sendSuccess(_("Verification passed"));
  } else {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Verification failed"));
  }
}
