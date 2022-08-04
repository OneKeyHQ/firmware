#include "se_atca.h"
#include "atca_api.h"
#include "atca_command.h"
#include "atca_config.h"
#include "device.h"
#include "memory.h"
#include "rng.h"
#include "sha2.h"
#include "util.h"

typedef struct {
  bool cached;
  uint8_t cache_pin[ATCA_KEY_SIZE];
} PINCache;

static bool se_has_pin = false;
static bool se_is_init = false;

extern ATCAPairingInfo *pair_info;

static PINCache pin_cache = {0};

static void pin_cacheSave(uint8_t pin[ATCA_KEY_SIZE]) {
  pin_cache.cached = true;
  memcpy(pin_cache.cache_pin, pin, ATCA_KEY_SIZE);
}

static void pin_cacheGet(uint8_t pin[ATCA_KEY_SIZE]) {
  pin_cache.cached = true;
  memcpy(pin, pin_cache.cache_pin, ATCA_KEY_SIZE);
}

static void pin_hash(const char *pin, uint32_t pin_len, uint8_t result[32]) {
  SHA256_CTX ctx = {0};

  sha256_Init(&ctx);
  sha256_Update(&ctx, (uint8_t *)pin, pin_len);
  sha256_Update(&ctx, pair_info->hash_mix, sizeof(pair_info->hash_mix));
  sha256_Final(&ctx, result);

  atca_sha_hmac(result, 32, SLOT_PIN_ATTEMPT, result);

  sha256_Init(&ctx);
  sha256_Update(&ctx, result, 32);
  sha256_Update(&ctx, pair_info->hash_mix, sizeof(pair_info->hash_mix));
  sha256_Final(&ctx, result);
}

char *se_get_version(void) { return device_get_se_config_version(); }

bool se_get_sn(char **serial) {
  (void)serial;
  return false;
}

bool se_setSeedStrength(uint32_t strength) {
  if (strength != 128 && strength != 192 && strength != 256) return false;
  ATCAUserState state = {0};

  atca_pair_unlock();
  atca_read_slot_data(SLOT_USER_SATATE, (uint8_t *)&state);
  state.strength = strength;
  atca_pair_unlock();
  if (ATCA_SUCCESS == atca_write_enc(SLOT_USER_SATATE, 0, (uint8_t *)&state,
                                     pair_info->protect_key,
                                     SLOT_IO_PROTECT_KEY)) {
    return true;
  }
  return false;
}

bool se_getSeedStrength(uint32_t *strength) {
  ATCAUserState state = {0};

  atca_pair_unlock();
  atca_read_slot_data(SLOT_USER_SATATE, (uint8_t *)&state);
  *strength = state.strength;
  if (*strength == 128 || *strength == 192 || *strength == 256) {
    return true;
  } else {
    return false;
  }
}

void pin_updateCounter(void) { atca_update_counter(); }

void se_get_status(void) {
  ATCAUserState state = {0};

  atca_pair_unlock();

  atca_read_slot_data(SLOT_USER_SATATE, (uint8_t *)&state);
  if (state.pin_set) {
    se_has_pin = true;
  } else {
    se_has_pin = false;
    pin_cacheSave(pair_info->init_pin);
  }

  if (state.initialized) {
    se_is_init = true;
  }
}

bool se_hasPin(void) {
  if (!se_has_pin) {
    pin_cacheSave(pair_info->init_pin);
  }
  return se_has_pin;
}

bool se_verifyPin(const char *pin) {
  uint8_t hash_pin[32] = {0};

  if (!se_has_pin) {
    return true;
  } else {
    pin_hash(pin, strlen(pin), hash_pin);
    atca_pair_unlock();
    if (ATCA_SUCCESS == atca_mac_slot(SLOT_USER_PIN, hash_pin)) {
      pin_cacheSave(hash_pin);
      pin_updateCounter();
      return true;
    } else {
      return false;
    }
  }
}

bool se_reset_pin(void) {
  ATCAUserState state = {0};

  atca_read_slot_data(SLOT_USER_SATATE, (uint8_t *)&state);
  atca_pair_unlock();
  if (ATCA_SUCCESS == atca_write_enc(SLOT_USER_PIN, 0, pair_info->init_pin,
                                     pair_info->protect_key,
                                     SLOT_IO_PROTECT_KEY)) {
    pin_cacheSave(pair_info->init_pin);
    pin_updateCounter();
    if (state.pin_set) {
      state.pin_set = false;
      atca_pair_unlock();
      if (ATCA_SUCCESS == atca_write_enc(SLOT_USER_SATATE, 0, (uint8_t *)&state,
                                         pair_info->protect_key,
                                         SLOT_IO_PROTECT_KEY)) {
        se_has_pin = false;
        return true;
      }
    } else {
      se_has_pin = false;
      return true;
    }
  }
  return false;
}

bool se_setPin(const char *pin) {
  uint8_t hash_pin[32] = {0};
  ATCAUserState state = {0};

  atca_read_slot_data(SLOT_USER_SATATE, (uint8_t *)&state);
  if (strlen(pin) == 0) {
    return se_reset_pin();

  } else {
    pin_hash(pin, strlen(pin), hash_pin);
    atca_pair_unlock();
    if (ATCA_SUCCESS == atca_write_enc(SLOT_USER_PIN, 0, hash_pin,
                                       pair_info->protect_key,
                                       SLOT_IO_PROTECT_KEY)) {
      pin_cacheSave(hash_pin);
      pin_updateCounter();
      if (!state.pin_set) {
        state.pin_set = true;
        atca_pair_unlock();
        if (ATCA_SUCCESS ==
            atca_write_enc(SLOT_USER_SATATE, 0, (uint8_t *)&state,
                           pair_info->protect_key, SLOT_IO_PROTECT_KEY)) {
          se_has_pin = true;
          return true;
        }
      } else {
        return true;
      }
    }
  }

  return false;
}

bool se_changePin(const char *old_pin, const char *new_pin) {
  if (se_verifyPin(old_pin)) {
    return se_setPin(new_pin);
  }
  return false;
}

bool se_isInitialized(void) {
  // ATCAUserState state = {0};

  // atca_pair_unlock();

  // atca_read_slot_data(SLOT_USER_SATATE, (uint8_t *)&state);

  // return state.initialized;
  return se_is_init;
}

bool se_importSeed(uint8_t *seed) {
  uint8_t pin[32] = {0};
  ATCAUserState state = {0};

  atca_read_slot_data(SLOT_USER_SATATE, (uint8_t *)&state);
  pin_cacheGet(pin);
  atca_pair_unlock();
  if (ATCA_SUCCESS == atca_mac_slot(SLOT_USER_PIN, pin)) {
    if (ATCA_SUCCESS == atca_write_enc(SLOT_USER_SECRET, 0, seed,
                                       pair_info->protect_key,
                                       SLOT_IO_PROTECT_KEY)) {
      if (!state.initialized) {
        state.initialized = true;
        se_is_init = true;
        atca_pair_unlock();
        if (ATCA_SUCCESS ==
            atca_write_enc(SLOT_USER_SATATE, 0, (uint8_t *)&state,
                           pair_info->protect_key, SLOT_IO_PROTECT_KEY)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool se_export_seed(uint8_t *seed) {
  uint8_t pin[32] = {0};
  pin_cacheGet(pin);

  atca_pair_unlock();
  if (ATCA_SUCCESS == atca_mac_slot(SLOT_USER_PIN, pin)) {
    if (ATCA_SUCCESS == atca_read_enc(SLOT_USER_SECRET, 0, seed,
                                      pair_info->protect_key,
                                      SLOT_IO_PROTECT_KEY)) {
      return true;
    }
  }
  return false;
}

bool se_is_wiping(void) {
  ATCAUserState state = {0};

  atca_pair_unlock();

  atca_read_slot_data(SLOT_USER_SATATE, (uint8_t *)&state);

  return state.wiping;
}

void se_set_wiping(bool flag) {
  ATCAUserState state = {0};

  atca_pair_unlock();
  atca_read_slot_data(SLOT_USER_SATATE, (uint8_t *)&state);
  if (flag != state.wiping) {
    state.wiping = flag;
    atca_pair_unlock();
    atca_write_enc(SLOT_USER_SATATE, 0, (uint8_t *)&state,
                   pair_info->protect_key, SLOT_IO_PROTECT_KEY);
  }
}

void se_reset_state(void) {
  uint8_t zeros[32] = {0};

  atca_pair_unlock();
  atca_write_enc(SLOT_USER_SATATE, 0, (uint8_t *)&zeros, pair_info->protect_key,
                 SLOT_IO_PROTECT_KEY);
  se_has_pin = false;
  se_is_init = false;
}

void se_reset_storage(void) {
  uint8_t zeros[32] = {0};

  se_reset_pin();
  se_importSeed(zeros);
}

uint32_t se_pinFailedCounter(void) { return atca_get_failed_counter(); }

bool se_device_init(uint8_t mode, const char *passphrase) {
  (void)mode;
  (void)passphrase;
  return true;
}

bool se_get_pubkey(uint8_t pubkey[64]) { return atca_cal_pubkey(pubkey); }

bool se_write_certificate(const uint8_t *cert, uint32_t cert_len) {
  return atca_write_certificate(cert, cert_len);
}

bool se_get_certificate_len(uint32_t *cert_len) {
  return atca_get_certificate_len(cert_len);
}

bool se_read_certificate(uint8_t *cert, uint32_t *cert_len) {
  return atca_read_certificate(cert, cert_len);
}

bool se_sign_message(uint8_t *msg, uint32_t msg_len, uint8_t *signature) {
  SHA256_CTX ctx = {0};
  uint8_t result[32] = {0};

  atca_pair_unlock();

  sha256_Init(&ctx);
  sha256_Update(&ctx, msg, msg_len);
  sha256_Final(&ctx, result);

  if (ATCA_SUCCESS ==
      atca_sign_extern(SLOT_PRIMARY_PRIVATE_KEY, result, signature)) {
    return true;
  } else {
    return false;
  }
}

void se_init(void) { atca_config_init(); }
