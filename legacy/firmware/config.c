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

#include <libopencm3/stm32/flash.h>
#include <stdint.h>
#include <string.h>

#include "messages-common.pb.h"
#include "messages.pb.h"

#include "aes/aes.h"
#include "bip32.h"
#include "bip39.h"
#include "ble.h"
#include "common.h"
#include "config.h"
#include "curves.h"
#include "debug.h"
#include "font.h"
#include "fsm.h"
#include "gettext.h"
#include "hmac.h"
#include "layout2.h"
#include "memory.h"
#include "memzero.h"
#include "menu_list.h"
#include "mi2c.h"
#include "pbkdf2.h"
#include "protect.h"
#include "rng.h"
#include "se_chip.h"
#include "sha2.h"
#include "storage.h"
#include "storage_ex.h"
#include "supervise.h"
#include "timer.h"
#include "trezor.h"
#include "u2f.h"
#include "usb.h"
#include "util.h"

/* Magic constants to check validity of storage block for storage versions 1
 * to 10. */
static const uint32_t CONFIG_MAGIC_V10 = 0x726f7473;  // 'stor' as uint32_t

#if !EMULATOR
static const uint32_t META_MAGIC_V10 = 0x525a5254;  // 'TRZR' as uint32_t
#else
static const uint32_t META_MAGIC_V10 = 0xFFFFFFFF;
#endif

#define APP (0x01 << 8)

#define FLAG_PUBLIC_SHIFTED (FLAG_PUBLIC << 8)
#define FLAGS_WRITE_SHIFTED (FLAGS_WRITE << 8)

// clang-format off
#define KEY_UUID (0 | APP | FLAG_PUBLIC_SHIFTED)                        // bytes(12)
#define KEY_VERSION (1 | APP)                                           // uint32
#define KEY_MNEMONIC (2 | APP)                                          // string(241)
#define KEY_LANGUAGE (3 | APP | FLAG_PUBLIC_SHIFTED)                    // string(17)
#define KEY_LABEL (4 | APP | FLAG_PUBLIC_SHIFTED)                       // string(33)
#define KEY_PASSPHRASE_PROTECTION (5 | APP | FLAG_PUBLIC_SHIFTED)       // bool
#define KEY_HOMESCREEN (6 | APP | FLAG_PUBLIC_SHIFTED)                  // bytes(1024)
#define KEY_NEEDS_BACKUP (7 | APP)                                      // bool
#define KEY_FLAGS (8 | APP)                                             // uint32
#define KEY_U2F_COUNTER (9 | APP | FLAGS_WRITE_SHIFTED)                 // uint32
#define KEY_UNFINISHED_BACKUP (11 | APP)                                // bool
#define KEY_AUTO_LOCK_DELAY_MS (12 | APP | FLAG_PUBLIC_SHIFTED)         // uint32
#define KEY_NO_BACKUP (13 | APP)                                        // bool
#define KEY_INITIALIZED (14 | APP | FLAG_PUBLIC_SHIFTED)                // uint32
#define KEY_NODE (15 | APP)                                             // node
#define KEY_IMPORTED (16 | APP)                                         // bool
#define KEY_U2F_ROOT (17 | APP | FLAG_PUBLIC_SHIFTED)                   // node
#define KEY_SEEDS (18 | APP)                                            // bytes
#define KEY_SEEDSFLAG (19 | APP | FLAG_PUBLIC_SHIFTED)                  // uint32
//#define KEY_PIN (20| APP_PIN )                                        // uint32
//#define KEY_PINFLAG (21| APP_PIN )                                    // uint32
//#define KEY_VERIFYPIN (22| APP_PIN)                                   // uint32

#define KEY_TRANSBLEMODE (23 | APP | FLAG_PUBLIC_SHIFTED)               // bool
#define KEY_FREEPAYPINFLAG (24 | APP)                                   // bool
#define KEY_SEFLAG (25 | APP | FLAG_PUBLIC_SHIFTED)                     // bool
//#define MNEMONIC_INDEX_TOSEED(26)
#define KEY_RESET (27 | APP)                                            // bool
#define KEY_FREEPAYCONFIRMFLAG (28 | APP)                               // bool
#define KEY_FREEPAYMONEYLIMT (29 | APP)                                 // uint64
#define KEY_FREEPAYPTIMES (30 | APP)                                    // uint32

#define KEY_SE_SESSIONKEY  (31 | APP | FLAG_PUBLIC_SHIFTED)             // bytes(16)
#define KEY_DEVICE_STATE (32 | APP | FLAG_PUBLIC_SHIFTED)               // uint32
#define KEY_SEED_PASSPHRASE (33 | APP)                                  // string
#define KEY_SEED_ST (34 | APP)                                          // string
#define KEY_ST_SEED_EXCHANGE (35 | APP)                                 // bytes, only used in se

#define KEY_MNEMONICS_IMPORTED (36 | APP | FLAG_PUBLIC_SHIFTED)         // bool
#define KEY_SLEEP_DELAY_MS (37 | APP | FLAG_PUBLIC_SHIFTED)             // uint32

#define KEY_DEBUG_LINK_PIN (255 | APP | FLAG_PUBLIC_SHIFTED)            // string(10)
// clang-format on

#define MAX_SESSIONS_COUNT 10

// The PIN value corresponding to an empty PIN.
static const uint32_t PIN_EMPTY = 1;

static secbool se_unlocked = secfalse;

static uint32_t config_uuid[UUID_SIZE / sizeof(uint32_t)];
_Static_assert(sizeof(config_uuid) == UUID_SIZE, "config_uuid has wrong size");

static char config_language[MAX_LANGUAGE_LEN];
_Static_assert(sizeof(config_language) == MAX_LANGUAGE_LEN,
               "config_language has wrong size");

char config_uuid_str[2 * UUID_SIZE + 1] = {0};

/*
 Old storage layout:

 offset |  type/length |  description
--------+--------------+-------------------------------
 0x0000 |     4 bytes  |  magic = 'stor'
 0x0004 |    12 bytes  |  uuid
 0x0010 |     ? bytes  |  Storage structure
--------+--------------+-------------------------------
 0x4000 |     4 kbytes |  area for pin failures
 0x5000 |   256 bytes  |  area for u2f counter updates
 0x5100 | 11.75 kbytes |  reserved

The area for pin failures looks like this:
0 ... 0 pinfail 0xffffffff .. 0xffffffff
The pinfail is a binary number of the form 1...10...0,
the number of zeros is the number of pin failures.
This layout is used because we can only clear bits without
erasing the flash.

The area for u2f counter updates is just a sequence of zero-bits
followed by a sequence of one-bits.  The bits in a byte are numbered
from LSB to MSB.  The number of zero bits is the offset that should
be added to the storage u2f_counter to get the real counter value.

 */

/* Current u2f offset, i.e. u2f counter is
 * storage.u2f_counter + config_u2f_offset.
 * This corresponds to the number of cleared bits in the U2FAREA.
 */

// Session management
typedef struct {
  uint8_t id[32];
  uint32_t last_use;
  uint8_t seed[64];
  secbool seedCached;
} Session;

static void session_clearCache(Session *session);
static uint8_t session_findLeastRecent(void);
static uint8_t session_findSession(const uint8_t *sessionId);

static CONFIDENTIAL Session sessionsCache[MAX_SESSIONS_COUNT];
static Session *activeSessionCache;

static uint32_t sessionUseCounter = 0;

#if !EMULATOR
#define autoLockDelayMsDefault (30 * 60 * 1000U)  // 30 minutes
#else
#define autoLockDelayMsDefault (10 * 60 * 1000U)  // 10 minutes
#endif
#define sleepDelayMsDefault (5 * 60 * 1000U)  // 5 minutes

static secbool autoLockDelayMsCached = secfalse;
static secbool sleepDelayMsCached = secfalse;
static uint32_t autoLockDelayMs = autoLockDelayMsDefault;
static uint32_t autoSleepDelayMs = sleepDelayMsDefault;

static uint32_t deviceState = 0;

static const uint32_t CONFIG_VERSION = 11;

static const uint8_t FALSE_BYTE = '\x00';
static const uint8_t TRUE_BYTE = '\x01';

static uint32_t pin_to_int(const char *pin) {
  uint32_t val = 1;
  size_t i = 0;
  for (i = 0; i < MAX_PIN_LEN && pin[i] != '\0'; ++i) {
    if (pin[i] < '0' || pin[i] > '9') {
      return 0;
    }
    val = 10 * val + pin[i] - '0';
  }

  if (pin[i] != '\0') {
    return 0;
  }

  return val;
}

static secbool config_set_bool(uint16_t key, bool value) {
  if (value) {
    return storage_set(key, &TRUE_BYTE, sizeof(TRUE_BYTE));
  } else {
    return storage_set(key, &FALSE_BYTE, sizeof(FALSE_BYTE));
  }
}

static secbool config_get_bool(uint16_t key, bool *value) {
  uint8_t val = 0;
  uint16_t len = 0;
  if (sectrue == storage_get(key, &val, sizeof(val), &len) &&
      len == sizeof(TRUE_BYTE)) {
    *value = (val == TRUE_BYTE);
    return sectrue;
  } else {
    *value = false;
    return secfalse;
  }
}

static secbool config_get_bytes(uint16_t key, uint8_t *dest, uint16_t dest_size,
                                uint16_t *real_size) {
  if (dest_size == 0) {
    return secfalse;
  }

  if (sectrue != storage_get(key, dest, dest_size, real_size)) {
    return secfalse;
  }
  return sectrue;
}

static secbool config_get_string(uint16_t key, char *dest, uint16_t dest_size) {
  if (dest_size == 0) {
    return secfalse;
  }

  uint16_t len = 0;
  if (sectrue != storage_get(key, dest, dest_size - 1, &len)) {
    dest[0] = '\0';
    return secfalse;
  }
  dest[len] = '\0';
  return sectrue;
}

static secbool config_get_uint32(uint16_t key, uint32_t *value) {
  uint16_t len = 0;
  if (sectrue != storage_get(key, value, sizeof(uint32_t), &len) ||
      len != sizeof(uint32_t)) {
    *value = 0;
    return secfalse;
  }
  return sectrue;
}

#define FLASH_META_START 0x08008000
#define FLASH_META_LEN 0x100

static secbool config_upgrade_v10(void) {
#define OLD_STORAGE_SIZE(last_member)                                        \
  (((offsetof(Storage, last_member) + pb_membersize(Storage, last_member)) + \
    3) &                                                                     \
   ~3)

  if (memcmp(FLASH_PTR(FLASH_META_START), &META_MAGIC_V10,
             sizeof(META_MAGIC_V10)) != 0 ||
      memcmp(FLASH_PTR(FLASH_META_START + FLASH_META_LEN), &CONFIG_MAGIC_V10,
             sizeof(CONFIG_MAGIC_V10)) != 0) {
    // wrong magic
    return secfalse;
  }

  Storage config __attribute__((aligned(4)));
  _Static_assert((sizeof(config) & 3) == 0, "storage unaligned");

  memcpy(
      config_uuid,
      FLASH_PTR(FLASH_META_START + FLASH_META_LEN + sizeof(CONFIG_MAGIC_V10)),
      sizeof(config_uuid));
  memcpy(&config,
         FLASH_PTR(FLASH_META_START + FLASH_META_LEN +
                   sizeof(CONFIG_MAGIC_V10) + sizeof(config_uuid)),
         sizeof(config));

  // version 1: since 1.0.0
  // version 2: since 1.2.1
  // version 3: since 1.3.1
  // version 4: since 1.3.2
  // version 5: since 1.3.3
  // version 6: since 1.3.6
  // version 7: since 1.5.1
  // version 8: since 1.5.2
  // version 9: since 1.6.1
  // version 10: since 1.7.2
  if (config.version > CONFIG_VERSION) {
    // downgrade -> clear storage
    config_wipe();
    return secfalse;
  }

  size_t old_config_size = 0;
  if (config.version == 0) {
  } else if (config.version <= 2) {
    old_config_size = OLD_STORAGE_SIZE(imported);
  } else if (config.version <= 5) {
    // added homescreen
    old_config_size = OLD_STORAGE_SIZE(homescreen);
  } else if (config.version <= 7) {
    // added u2fcounter
    old_config_size = OLD_STORAGE_SIZE(u2f_counter);
  } else if (config.version <= 8) {
    // added flags and needsBackup
    old_config_size = OLD_STORAGE_SIZE(flags);
  } else if (config.version <= 9) {
    // added u2froot, unfinished_backup and auto_lock_delay_ms
    old_config_size = OLD_STORAGE_SIZE(auto_lock_delay_ms);
  } else if (config.version <= 10) {
    // added no_backup
    old_config_size = OLD_STORAGE_SIZE(no_backup);
  }

  // Erase newly added fields.
  if (old_config_size != sizeof(Storage)) {
    memzero((char *)&config + old_config_size,
            sizeof(Storage) - old_config_size);
  }

  const uint32_t FLASH_STORAGE_PINAREA = FLASH_META_START + 0x4000;
  uint32_t pin_wait = 0;
  if (config.version <= 5) {
    // Get PIN failure counter from version 5 format.
    uint32_t pinctr =
        config.has_pin_failed_attempts ? config.pin_failed_attempts : 0;
    if (pinctr > 31) {
      pinctr = 31;
    }

    pin_wait = (1 << pinctr) - 1;
  } else {
    // Get PIN failure counter from version 10 format.
    uint32_t flash_pinfails = FLASH_STORAGE_PINAREA;
    while (*(const uint32_t *)FLASH_PTR(flash_pinfails) == 0) {
      flash_pinfails += sizeof(uint32_t);
    }
    pin_wait = ~*(const uint32_t *)FLASH_PTR(flash_pinfails);
  }

  uint32_t u2f_offset = 0;
  if (config.has_u2f_counter) {
    const uint32_t FLASH_STORAGE_U2FAREA = FLASH_STORAGE_PINAREA + 0x1000;
    const uint32_t *u2fptr = (const uint32_t *)FLASH_PTR(FLASH_STORAGE_U2FAREA);
    while (*u2fptr == 0) {
      u2fptr++;
    }
    u2f_offset =
        32 * (u2fptr - (const uint32_t *)FLASH_PTR(FLASH_STORAGE_U2FAREA));
    uint32_t u2fword = *u2fptr;
    while ((u2fword & 1) == 0) {
      u2f_offset++;
      u2fword >>= 1;
    }
  }

  storage_init(NULL, HW_ENTROPY_DATA, HW_ENTROPY_LEN);
  storage_unlock(PIN_EMPTY, NULL);
  if (config.has_pin) {
    storage_change_pin(PIN_EMPTY, pin_to_int(config.pin), NULL, NULL);
  }

  while (pin_wait != 0) {
    storage_pin_fails_increase();
    pin_wait >>= 1;
  }

  storage_set(KEY_UUID, config_uuid, sizeof(config_uuid));
  storage_set(KEY_VERSION, &CONFIG_VERSION, sizeof(CONFIG_VERSION));
  if (config.has_node) {
    if (sectrue == storage_set(KEY_NODE, &config.node, sizeof(config.node))) {
      config_set_bool(KEY_INITIALIZED, true);
    }
  }
  if (config.has_mnemonic) {
    config_setMnemonic(config.mnemonic, false);
  }
  if (config.has_passphrase_protection) {
    config_setPassphraseProtection(config.passphrase_protection);
  }
  if (config.has_language) {
    config_setLanguage(config.language);
  }
  if (config.has_label) {
    config_setLabel(config.label);
  }
  if (config.has_imported) {
    config_setImported(config.imported);
  }
  if (config.has_homescreen) {
    config_setHomescreen(config.homescreen.bytes, config.homescreen.size);
  }
  if (config.has_u2f_counter) {
    config_setU2FCounter(config.u2f_counter + u2f_offset);
  }
  if (config.has_needs_backup) {
    config_setNeedsBackup(config.needs_backup);
  }
  if (config.has_flags) {
    config_applyFlags(config.flags);
  }
  if (config.has_unfinished_backup) {
    config_setUnfinishedBackup(config.unfinished_backup);
  }
  if (config.has_auto_lock_delay_ms) {
    config_setAutoLockDelayMs(config.auto_lock_delay_ms);
  }
  if (config.has_no_backup && config.no_backup) {
    config_setNoBackup();
  }
  memzero(&config, sizeof(config));

  session_clear(true);

  return sectrue;
}

void config_init(void) {
  char *se_version = NULL;
  char oldTiny = usbTiny(1);

  config_upgrade_v10();

  storage_init(&protectPinUiCallback, HW_ENTROPY_DATA, HW_ENTROPY_LEN);
  memzero(HW_ENTROPY_DATA, sizeof(HW_ENTROPY_DATA));

  // get whether use se flag
  se_version = se_get_version();
  for (uint8_t i = 0; i < 5; i++) {
    if (se_version == NULL) {
      se_power_off();
      delay_ms(100);
      se_power_on();
      se_version = se_get_version();
    } else {
      break;
    }
  }
  if (se_version) {
    if (strcmp(se_version, "1.1.0.0") > 0) {
      g_bSelectSEFlag = true;
    }
  } else {
    layoutDialog(&bmp_icon_error, NULL, NULL, NULL, _("Get SE version"),
                 _("failed."), NULL, NULL, _("Contact our support."), NULL);
    delay_ms(1000);
    shutdown();
  }

  config_getLanguage(config_language, sizeof(config_language));

  // imported xprv is not supported anymore so we set initialized to false
  // if no mnemonic is present
  if (config_isInitialized() && !config_hasMnemonic()) {
    config_set_bool(KEY_INITIALIZED, false);
  }

  // Auto-unlock storage if no PIN is set.
  if (storage_is_unlocked() == secfalse && storage_has_pin() == secfalse) {
    storage_unlock(PIN_EMPTY, NULL);
  }
#if !EMULATOR
  se_sync_session_key();
#endif

  uint16_t len = 0;
  // If UUID is not set, then the config is uninitialized.
  if (sectrue !=
          storage_get(KEY_UUID, config_uuid, sizeof(config_uuid), &len) ||
      len != sizeof(config_uuid)) {
    random_buffer((uint8_t *)config_uuid, sizeof(config_uuid));
    storage_set(KEY_UUID, config_uuid, sizeof(config_uuid));
    storage_set(KEY_VERSION, &CONFIG_VERSION, sizeof(CONFIG_VERSION));
  }

  data2hex(config_uuid, sizeof(config_uuid), config_uuid_str);

  session_clear(false);
#if !EMULATOR
  user_data_init();
#endif
  usbTiny(oldTiny);
}

void session_clear(bool lock) {
  for (uint8_t i = 0; i < MAX_SESSIONS_COUNT; i++) {
    session_clearCache(sessionsCache + i);
  }
  activeSessionCache = NULL;
  if (lock) {
    config_lockDevice();
  }
}

void session_clearCache(Session *session) {
  session->last_use = 0;
  memzero(session->id, sizeof(session->id));
  memzero(session->seed, sizeof(session->seed));
  session->seedCached = false;
}

void config_lockDevice(void) {
  if (g_bSelectSEFlag) {
    se_unlocked = secfalse;
  } else {
    storage_lock();
  }
}

static void get_u2froot_callback(uint32_t iter, uint32_t total) {
  layoutProgressAdapter(_("Updating"), 1000 * iter / total);
}

static void config_compute_u2froot(const char *mnemonic,
                                   StorageHDNode *u2froot) {
  static CONFIDENTIAL HDNode node;
  static CONFIDENTIAL uint8_t seed[64];
  char oldTiny = usbTiny(1);
  mnemonic_to_seed(mnemonic, "", seed,
                   get_u2froot_callback);  // BIP-0039
  usbTiny(oldTiny);
  hdnode_from_seed(seed, 64, NIST256P1_NAME, &node);
  hdnode_private_ckd(&node, U2F_KEY_PATH);
  u2froot->depth = node.depth;
  u2froot->child_num = U2F_KEY_PATH;
  u2froot->chain_code.size = sizeof(node.chain_code);
  memcpy(u2froot->chain_code.bytes, node.chain_code, sizeof(node.chain_code));
  u2froot->has_private_key = true;
  u2froot->private_key.size = sizeof(node.private_key);
  memcpy(u2froot->private_key.bytes, node.private_key,
         sizeof(node.private_key));
  memzero(&node, sizeof(node));
  memzero(&seed, sizeof(seed));
  session_clear(false);  // invalidate seed cache
}

#if DEBUG_LINK

bool config_dumpNode(HDNodeType *node) {
  memzero(node, sizeof(HDNodeType));

  StorageHDNode storageNode = {0};
  uint16_t len = 0;
  if (sectrue !=
          storage_get(KEY_NODE, &storageNode, sizeof(storageNode), &len) ||
      len != sizeof(StorageHDNode)) {
    memzero(&storageNode, sizeof(storageNode));
    return false;
  }

  node->depth = storageNode.depth;
  node->fingerprint = storageNode.fingerprint;
  node->child_num = storageNode.child_num;

  node->chain_code.size = 32;
  memcpy(node->chain_code.bytes, storageNode.chain_code.bytes, 32);

  if (storageNode.has_private_key) {
    node->has_private_key = true;
    node->private_key.size = 32;
    memcpy(node->private_key.bytes, storageNode.private_key.bytes, 32);
  }

  memzero(&storageNode, sizeof(storageNode));
  return true;
}

void config_loadDevice(const LoadDevice *msg) {
  session_clear(false);
  config_set_bool(KEY_IMPORTED, true);
  config_setPassphraseProtection(msg->has_passphrase_protection &&
                                 msg->passphrase_protection);

  if (msg->has_pin) {
    config_changePin("", msg->pin);
  }

  if (msg->mnemonics_count) {
    storage_delete(KEY_NODE);
    config_setMnemonic(msg->mnemonics[0], true);
  }

  if (msg->has_language) {
    config_setLanguage(msg->language);
  }

  config_setLabel(msg->has_label ? msg->label : "");

  if (msg->has_u2f_counter) {
    config_setU2FCounter(msg->u2f_counter);
  }

  if (msg->has_needs_backup) {
    config_setNeedsBackup(msg->needs_backup);
  }

  if (msg->has_no_backup && msg->no_backup) {
    config_setNoBackup();
  }
}

#endif

void config_loadDevice_ex(const BixinLoadDevice *msg) {
  session_clear(false);
  config_set_bool(KEY_MNEMONICS_IMPORTED, true);

  config_setMnemonic(msg->mnemonics, true);
  config_set_bool(KEY_INITIALIZED, true);

  if (msg->has_language) {
    config_setLanguage(msg->language);
  }

  config_setLabel(msg->has_label ? msg->label : "");
}

void config_setLabel(const char *label) {
  if (label == NULL || label[0] == '\0') {
    storage_delete(KEY_LABEL);
  } else {
    storage_set(KEY_LABEL, label, strnlen(label, MAX_LABEL_LEN));
  }
}

void config_setLanguage(const char *lang) {
  if (lang == NULL) {
    return;
  }
  // Sanity check.
  if (strcmp(lang, "en-US") == 0 || strcmp(lang, "english") == 0) {
    ui_language = 0;
  } else if (strcmp(lang, "zh-CN") == 0 || strcmp(lang, "chinese") == 0) {
    ui_language = 1;
  } else {
    return;
  }

  storage_set(KEY_LANGUAGE, lang, strnlen(lang, MAX_LANGUAGE_LEN));
  font_set(ui_language ? "dingmao_9x9" : "english");
}

void config_setPassphraseProtection(bool passphrase_protection) {
  config_set_bool(KEY_PASSPHRASE_PROTECTION, passphrase_protection);
}

bool config_getPassphraseProtection(bool *passphrase_protection) {
  return sectrue ==
         config_get_bool(KEY_PASSPHRASE_PROTECTION, passphrase_protection);
}

void config_setHomescreen(const uint8_t *data, uint32_t size) {
  if (data != NULL && size == HOMESCREEN_SIZE) {
    storage_set(KEY_HOMESCREEN, data, size);
  } else {
    storage_delete(KEY_HOMESCREEN);
  }
}

static void get_root_node_callback(uint32_t iter, uint32_t total) {
  usbSleep(1);
  layoutProgressAdapter(_("Waking up"), 1000 * iter / total);
}

const uint8_t *config_getSeed(void) {
  if (activeSessionCache == NULL) {
    fsm_sendFailure(FailureType_Failure_InvalidSession, "Invalid session");
    return NULL;
  }

  // root node is properly cached
  if (activeSessionCache->seedCached == sectrue) {
    return activeSessionCache->seed;
  }

  // if storage has mnemonic, convert it to node and use it
  char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
  if (config_getMnemonic(mnemonic, sizeof(mnemonic))) {
    char passphrase[MAX_PASSPHRASE_LEN + 1] = {0};
    if (!protectPassphrase(passphrase)) {
      memzero(mnemonic, sizeof(mnemonic));
      memzero(passphrase, sizeof(passphrase));
      return NULL;
    }
    // passphrase is used - confirm on the display
    if (passphrase[0] != 0) {
      layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                        _("Access hidden wallet?"), NULL,
                        _("Next screen will show"), _("the passphrase!"), NULL,
                        NULL);
      if (!protectButton(ButtonRequestType_ButtonRequest_Other, false)) {
        memzero(mnemonic, sizeof(mnemonic));
        memzero(passphrase, sizeof(passphrase));
        fsm_sendFailure(FailureType_Failure_ActionCancelled,
                        _("Passphrase dismissed"));
        layoutHome();
        return NULL;
      }
      layoutShowPassphrase(passphrase);
      if (!protectButton(ButtonRequestType_ButtonRequest_Other, false)) {
        memzero(mnemonic, sizeof(mnemonic));
        memzero(passphrase, sizeof(passphrase));
        fsm_sendFailure(FailureType_Failure_ActionCancelled,
                        _("Passphrase dismissed"));
        layoutHome();
        return NULL;
      }
    }
    // if storage was not imported (i.e. it was properly generated or
    // recovered)
    bool imported = false;
    config_get_bool(KEY_IMPORTED, &imported);
    if (!imported) {
      // test whether mnemonic is a valid BIP-0039 mnemonic
      if (!mnemonic_check(mnemonic)) {
        // and if not then halt the device
        error_shutdown(_("Storage failure"), _("detected."), NULL, NULL);
      }
    }
    char oldTiny = usbTiny(1);
    mnemonic_to_seed(mnemonic, passphrase, activeSessionCache->seed,
                     get_root_node_callback);  // BIP-0039
    memzero(mnemonic, sizeof(mnemonic));
    memzero(passphrase, sizeof(passphrase));
    usbTiny(oldTiny);
    activeSessionCache->seedCached = sectrue;
    return activeSessionCache->seed;
  } else {
    fsm_sendFailure(FailureType_Failure_NotInitialized,
                    _("Device not initialized"));
  }

  return NULL;
}

static bool config_loadNode(const StorageHDNode *node, const char *curve,
                            HDNode *out) {
  return hdnode_from_xprv(node->depth, node->child_num, node->chain_code.bytes,
                          node->private_key.bytes, curve, out);
}

bool config_getU2FRoot(HDNode *node) {
  StorageHDNode u2fNode = {0};
  uint16_t len = 0;
  if (sectrue != storage_get(KEY_U2F_ROOT, &u2fNode, sizeof(u2fNode), &len) ||
      len != sizeof(StorageHDNode)) {
    memzero(&u2fNode, sizeof(u2fNode));
    return false;
  }
  bool ret = config_loadNode(&u2fNode, NIST256P1_NAME, node);
  memzero(&u2fNode, sizeof(u2fNode));
  return ret;
}

bool config_getRootNode(HDNode *node, const char *curve) {
  const uint8_t *seed = config_getSeed();
  if (seed == NULL) {
    return false;
  }
  int result = hdnode_from_seed(seed, 64, curve, node);
  if (result == 0) {
    fsm_sendFailure(FailureType_Failure_NotInitialized, _("Unsupported curve"));
  }
  return result;
}

bool config_getLabel(char *dest, uint16_t dest_size) {
  return sectrue == config_get_string(KEY_LABEL, dest, dest_size);
}

bool config_getLanguage(char *dest, uint16_t dest_size) {
  if (sectrue == config_get_string(KEY_LANGUAGE, dest, dest_size)) {
    if (strcmp(dest, "en-US") == 0 || strcmp(dest, "english") == 0) {
      ui_language = 0;
    } else if (strcmp(dest, "zh-CN") == 0 || strcmp(dest, "chinese") == 0) {
      ui_language = 1;
    }
  } else {
    ui_language = 0;
    strcpy(dest, "en-US");
    dest_size = 5;
  }

  font_set(ui_language ? "dingmao_9x9" : "english");

  return true;
}

bool config_isLanguageSet(void) {
  char language[MAX_LANGUAGE_LEN];
  if (sectrue == config_get_string(KEY_LANGUAGE, language, sizeof(language))) {
    return true;
  }
  return false;
}

bool config_getHomescreen(uint8_t *dest, uint16_t dest_size) {
  uint16_t len = 0;
  secbool ret = storage_get(KEY_HOMESCREEN, dest, dest_size, &len);
  if (sectrue != ret || len != HOMESCREEN_SIZE) {
    return false;
  }
  return true;
}

bool config_setMnemonic(const char *mnemonic, bool import) {
  if (mnemonic == NULL) {
    return false;
  }

  if (g_bSelectSEFlag) {
    se_setNeedsBackup(false);
    if (import) {
      uint8_t seed[64] = {0};
      uint32_t strength = 0;
      (void)strength;
      random_buffer(seed, sizeof(seed));
      strength = (mnemonic_to_bits(mnemonic, seed) / 11) * 8 * 4 / 3;
      se_setSeedStrength(strength);
      return se_importSeed(seed);
    }
  }

  if (sectrue != storage_set(KEY_MNEMONIC, mnemonic,
                             strnlen(mnemonic, MAX_MNEMONIC_LEN))) {
    return false;
  }

  StorageHDNode u2fNode = {0};
  memzero(&u2fNode, sizeof(u2fNode));
  config_compute_u2froot(mnemonic, &u2fNode);
  secbool ret = storage_set(KEY_U2F_ROOT, &u2fNode, sizeof(u2fNode));
  memzero(&u2fNode, sizeof(u2fNode));

  if (sectrue != ret) {
    storage_delete(KEY_MNEMONIC);
    return false;
  }
  config_set_bool(KEY_INITIALIZED, true);

  return true;
}

bool config_setSeedsBytes(const uint8_t *seeds, uint8_t len) {
  if (seeds == NULL) {
    return false;
  }

  if (sectrue != storage_set(KEY_SEEDS, seeds, len)) {
    return false;
  }
  if (!g_bSelectSEFlag) {
    config_set_bool(KEY_INITIALIZED, true);
    config_set_bool(KEY_SEEDSFLAG, true);
  }

  return true;
}
bool config_SeedsEncExportBytes(BixinOutMessageSE_outmessage_t *get_msg) {
  if (!g_bSelectSEFlag) {
    return false;
  }
  if (sectrue != se_transmit(MI2C_CMD_WR_PIN, (KEY_SEEDS & 0xFF), NULL, 0,
                             get_msg->bytes, &get_msg->size, FLAG_PUBLIC,
                             GET_SESTORE_DATA)) {
    return false;
  }
  get_msg->bytes[get_msg->size] = '\0';
  return true;
}
bool config_SeedsEncImportBytes(BixinSeedOperate_seed_importData_t *input_msg) {
  if (!g_bSelectSEFlag) {
    return false;
  }
  if (sectrue != se_transmit(MI2C_CMD_WR_PIN, (KEY_SEEDS & 0xFF),
                             input_msg->bytes, input_msg->size, NULL, NULL,
                             FLAG_PUBLIC, DELETE_SESTORE_DATA)) {
    return false;
  }
  return true;
}

bool config_getMnemonicBytes(uint8_t *dest, uint16_t dest_size,
                             uint16_t *real_size) {
  return sectrue == config_get_bytes(KEY_MNEMONIC, dest, dest_size, real_size);
}

bool config_getMnemonic(char *dest, uint16_t dest_size) {
  if (g_bSelectSEFlag) {
    uint8_t seed[64];
    uint32_t strength = 0;
    if (!se_getSeedStrength(&strength)) return false;
    if (!se_export_seed(seed)) return false;
    const char *mnemonic = mnemonic_from_data(seed, strength / 8);
    strlcpy(dest, mnemonic, dest_size);
    return true;
  } else {
    return sectrue == config_get_string(KEY_MNEMONIC, dest, dest_size);
  }
}

bool config_hasMnemonic(void) {
  if (g_bSelectSEFlag)
    return true;
  else
    return sectrue == storage_has(KEY_MNEMONIC);
}

/* Check whether mnemonic matches storage. The mnemonic must be
 * a null-terminated string.
 */
bool config_containsMnemonic(const char *mnemonic) {
  uint16_t len = 0;
  uint8_t stored_mnemonic[MAX_MNEMONIC_LEN] = {0};
  if (sectrue != storage_get(KEY_MNEMONIC, stored_mnemonic,
                             sizeof(stored_mnemonic), &len)) {
    return false;
  }

  // Compare the digests to mitigate side-channel attacks.
  uint8_t digest_stored[SHA256_DIGEST_LENGTH] = {0};
  sha256_Raw(stored_mnemonic, len, digest_stored);
  memzero(stored_mnemonic, sizeof(stored_mnemonic));

  uint8_t digest_input[SHA256_DIGEST_LENGTH] = {0};
  sha256_Raw((const uint8_t *)mnemonic, strnlen(mnemonic, MAX_MNEMONIC_LEN),
             digest_input);

  uint8_t diff = 0;
  for (size_t i = 0; i < sizeof(digest_input); i++) {
    diff |= (digest_stored[i] - digest_input[i]);
  }
  memzero(digest_stored, sizeof(digest_stored));
  memzero(digest_input, sizeof(digest_input));
  return diff == 0;
}

/* Check whether pin matches storage.  The pin must be
 * a null-terminated string with at most 9 characters.
 */
bool config_unlock(const char *pin) {
  if (g_bSelectSEFlag) {
    if (se_verifyPin((pin_to_int(pin)))) {
      se_unlocked = sectrue;
      return true;
    } else {
      se_unlocked = secfalse;
      return false;
    }

  } else {
    char oldTiny = usbTiny(1);
    secbool ret = storage_unlock(pin_to_int(pin), NULL);
    usbTiny(oldTiny);
    return sectrue == ret;
  }
}

bool config_hasPin(void) {
  if (g_bSelectSEFlag) {
    return se_hasPin();
  } else {
    return sectrue == storage_has_pin();
  }
}

bool config_changePin(const char *old_pin, const char *new_pin) {
  uint32_t new_pin_int = pin_to_int(new_pin);
  if (new_pin_int == 0) {
    return false;
  }
  if (g_bSelectSEFlag) {
    if (se_changePin(pin_to_int(old_pin), new_pin_int)) {
      se_unlocked = sectrue;
      return true;
    } else {
      se_unlocked = secfalse;
      return false;
    }
  } else {
    char oldTiny = usbTiny(1);
    secbool ret =
        storage_change_pin(pin_to_int(old_pin), new_pin_int, NULL, NULL);
    usbTiny(oldTiny);

#if DEBUG_LINK
    if (sectrue == ret) {
      if (new_pin_int != PIN_EMPTY) {
        storage_set(KEY_DEBUG_LINK_PIN, new_pin, strnlen(new_pin, MAX_PIN_LEN));
      } else {
        storage_delete(KEY_DEBUG_LINK_PIN);
      }
    }
#endif

    memzero(&new_pin_int, sizeof(new_pin_int));

    return sectrue == ret;
  }
}

#if DEBUG_LINK
bool config_getPin(char *dest, uint16_t dest_size) {
  return sectrue == config_get_string(KEY_DEBUG_LINK_PIN, dest, dest_size);
}
#endif

bool config_hasWipeCode(void) { return sectrue == storage_has_wipe_code(); }

bool config_changeWipeCode(const char *pin, const char *wipe_code) {
  uint32_t wipe_code_int = pin_to_int(wipe_code);
  if (wipe_code_int == 0) {
    return false;
  }

  char oldTiny = usbTiny(1);
  secbool ret = storage_change_wipe_code(pin_to_int(pin), NULL, wipe_code_int);
  usbTiny(oldTiny);

  memzero(&wipe_code_int, sizeof(wipe_code_int));
  return sectrue == ret;
}

uint8_t session_findLeastRecent(void) {
  uint8_t least_recent_index = MAX_SESSIONS_COUNT;
  uint32_t least_recent_use = sessionUseCounter;
  for (uint8_t i = 0; i < MAX_SESSIONS_COUNT; i++) {
    if (sessionsCache[i].last_use == 0) {
      return i;
    }
    if (sessionsCache[i].last_use <= least_recent_use) {
      least_recent_use = sessionsCache[i].last_use;
      least_recent_index = i;
    }
  }
  ensure(sectrue * (least_recent_index < MAX_SESSIONS_COUNT), NULL);
  return least_recent_index;
}

uint8_t session_findSession(const uint8_t *sessionId) {
  for (uint8_t i = 0; i < MAX_SESSIONS_COUNT; i++) {
    if (sessionsCache[i].last_use != 0) {
      if (memcmp(sessionsCache[i].id, sessionId, 32) == 0) {  // session found
        return i;
      }
    }
  }
  return MAX_SESSIONS_COUNT;
}

uint8_t *session_startSession(const uint8_t *received_session_id) {
  int session_index = MAX_SESSIONS_COUNT;

  if (received_session_id != NULL) {
    session_index = session_findSession(received_session_id);
  }

  if (session_index == MAX_SESSIONS_COUNT) {
    // Session not found in cache. Use an empty one or the least recently
    // used.
    session_index = session_findLeastRecent();
    session_clearCache(sessionsCache + session_index);
    random_buffer(sessionsCache[session_index].id, 32);
  }

  sessionUseCounter++;
  sessionsCache[session_index].last_use = sessionUseCounter;
  activeSessionCache = sessionsCache + session_index;
  return activeSessionCache->id;
}

void session_endCurrentSession(void) {
  if (activeSessionCache == NULL) return;
  session_clearCache(activeSessionCache);
  activeSessionCache = NULL;
}

bool session_isUnlocked(void) {
  if (g_bSelectSEFlag) {
    if (se_hasPin()) {
      return sectrue == se_unlocked;
    } else {
      return true;
    }

  } else {
    return sectrue == storage_is_unlocked();
  }
}

bool config_isInitialized(void) {
  bool initialized = false;
  if (g_bSelectSEFlag) {
    initialized = se_isInitialized();
  } else {
    config_get_bool(KEY_INITIALIZED, &initialized);
  }
  return initialized;
}
bool config_isInitializedSeeds(void) {
  bool initialized = false;
  config_get_bool(KEY_SEEDSFLAG, &initialized);
  return initialized;
}

bool config_getImported(bool *imported) {
  return sectrue == config_get_bool(KEY_IMPORTED, imported);
}

void config_setImported(bool imported) {
  config_set_bool(KEY_IMPORTED, imported);
}

bool config_getMnemonicsImported(void) {
  bool mnemonic_imported = false;
  config_get_bool(KEY_MNEMONICS_IMPORTED, &mnemonic_imported);
  return mnemonic_imported;
}

bool config_getNeedsBackup(bool *needs_backup) {
  return sectrue == config_get_bool(KEY_NEEDS_BACKUP, needs_backup);
}

void config_setNeedsBackup(bool needs_backup) {
  config_set_bool(KEY_NEEDS_BACKUP, needs_backup);
}

bool config_getUnfinishedBackup(bool *unfinished_backup) {
  return sectrue == config_get_bool(KEY_UNFINISHED_BACKUP, unfinished_backup);
}

void config_setUnfinishedBackup(bool unfinished_backup) {
  config_set_bool(KEY_UNFINISHED_BACKUP, unfinished_backup);
}

bool config_getNoBackup(bool *no_backup) {
  return sectrue == config_get_bool(KEY_NO_BACKUP, no_backup);
}

void config_setNoBackup(void) { config_set_bool(KEY_NO_BACKUP, true); }

void config_applyFlags(uint32_t flags) {
  uint32_t old_flags = 0;
  config_get_uint32(KEY_FLAGS, &old_flags);
  flags |= old_flags;
  if (flags == old_flags) {
    return;  // no new flags
  }
  storage_set(KEY_FLAGS, &flags, sizeof(flags));
}

bool config_getFlags(uint32_t *flags) {
  return sectrue == config_get_uint32(KEY_FLAGS, flags);
}

uint32_t config_nextU2FCounter(void) {
  uint32_t u2fcounter = 0;
  storage_next_counter(KEY_U2F_COUNTER, &u2fcounter);
  return u2fcounter;
}

void config_setU2FCounter(uint32_t u2fcounter) {
  storage_set_counter(KEY_U2F_COUNTER, u2fcounter);
}

uint32_t config_getAutoLockDelayMs() {
  if (sectrue == autoLockDelayMsCached) {
    return autoLockDelayMs;
  }
#if EMULATOR
  if (sectrue != storage_is_unlocked()) {
    return autoLockDelayMsDefault;
  }
#endif
  if (sectrue != config_get_uint32(KEY_AUTO_LOCK_DELAY_MS, &autoLockDelayMs)) {
    autoLockDelayMs = autoLockDelayMsDefault;
  }
  if (autoLockDelayMs) {
    autoLockDelayMs = MAX(autoLockDelayMs, MIN_AUTOLOCK_DELAY_MS);
  }
  autoLockDelayMsCached = sectrue;
  return autoLockDelayMs;
}

void config_setAutoLockDelayMs(uint32_t auto_lock_delay_ms) {
  if (auto_lock_delay_ms != 0)
    auto_lock_delay_ms = MAX(auto_lock_delay_ms, MIN_AUTOLOCK_DELAY_MS);
  if (sectrue == storage_set(KEY_AUTO_LOCK_DELAY_MS, &auto_lock_delay_ms,
                             sizeof(auto_lock_delay_ms))) {
    autoLockDelayMs = auto_lock_delay_ms;
    autoLockDelayMsCached = sectrue;
  }
}

uint32_t config_getSleepDelayMs(void) {
  if (sectrue == sleepDelayMsCached) {
    return autoSleepDelayMs;
  }

  if (sectrue != config_get_uint32(KEY_SLEEP_DELAY_MS, &autoSleepDelayMs)) {
    autoSleepDelayMs = sleepDelayMsDefault;
  }
  sleepDelayMsCached = sectrue;
  return autoSleepDelayMs;
}

void config_setSleepDelayMs(uint32_t auto_sleep_ms) {
  if (auto_sleep_ms != 0)
    auto_sleep_ms = MAX(auto_sleep_ms, MIN_AUTOLOCK_DELAY_MS);
  if (sectrue ==
      storage_set(KEY_SLEEP_DELAY_MS, &auto_sleep_ms, sizeof(auto_sleep_ms))) {
    autoSleepDelayMs = auto_sleep_ms;
    sleepDelayMsCached = sectrue;
  }
}

void config_wipe(void) {
  uint8_t session_key[16];

  if (g_bSelectSEFlag) {
    se_reset_storage();
    config_getSeSessionKey(session_key, sizeof(session_key));
    se_unlocked = secfalse;
  }

  char oldTiny = usbTiny(1);
  storage_wipe();
  usbTiny(oldTiny);
  random_buffer((uint8_t *)config_uuid, sizeof(config_uuid));
  data2hex(config_uuid, sizeof(config_uuid), config_uuid_str);
  autoLockDelayMsCached = secfalse;
  storage_set(KEY_UUID, config_uuid, sizeof(config_uuid));
  storage_set(KEY_VERSION, &CONFIG_VERSION, sizeof(CONFIG_VERSION));
  config_setSeSessionKey(session_key, 16);
  config_getLanguage(config_language, sizeof(config_language));
}

void config_setFastPayPinFlag(bool flag) {
  config_set_bool(KEY_FREEPAYPINFLAG, flag);
}

bool config_getFastPayPinFlag(void) {
  bool flag = false;
  config_get_bool(KEY_FREEPAYPINFLAG, &flag);
  return flag;
}

void config_setFastPayConfirmFlag(bool flag) {
  config_set_bool(KEY_FREEPAYCONFIRMFLAG, flag);
}
bool config_getFastPayConfirmFlag(void) {
  bool flag = false;
  config_get_bool(KEY_FREEPAYCONFIRMFLAG, &flag);
  return flag;
}

void config_setFastPayMoneyLimt(uint64_t MoneyLimt) {
  storage_set(KEY_FREEPAYMONEYLIMT, &MoneyLimt, sizeof(uint64_t));
}

uint64_t config_getFastPayMoneyLimt(void) {
  uint64_t MoneyLimt = 0;
  uint16_t len = sizeof(MoneyLimt);
  config_get_bytes(KEY_FREEPAYMONEYLIMT, (uint8_t *)&MoneyLimt,
                   sizeof(uint64_t), &len);
  return MoneyLimt;
}

void config_setFastPayTimes(uint32_t times) {
  storage_set(KEY_FREEPAYPTIMES, &times, sizeof(uint32_t));
}

uint32_t config_getFastPayTimes(void) {
  uint32_t times = 0;
  config_get_uint32(KEY_FREEPAYPTIMES, &times);
  return times;
}

void config_setBleTrans(bool mode) {
  ble_set_switch(mode);
  change_ble_sta(mode);
}

void config_setWhetherUseSE(bool flag) {
  config_set_bool(KEY_SEFLAG, flag);
  if (g_bSelectSEFlag != flag) {
    session_clear(true);
    g_bSelectSEFlag = flag;
  }
  // Auto-unlock storage if no PIN is set.
  if (storage_has_pin() == secfalse) {
    storage_unlock(PIN_EMPTY, NULL);
  }
}

bool config_getWhetherUseSE(void) { return g_bSelectSEFlag; }

ExportType config_setSeedsExportFlag(ExportType flag) { return flag; }

bool config_getMessageSE(BixinMessageSE_inputmessage_t *input_msg,
                         BixinOutMessageSE_outmessage_t *get_msg) {
  if (false == bMI2CDRV_SendData(input_msg->bytes, input_msg->size)) {
    return false;
  }
  get_msg->size = 1024;
  if (false == bMI2CDRV_ReceiveData(get_msg->bytes, &get_msg->size)) {
    return false;
  }
  get_msg->bytes[get_msg->size] = '\0';
  return true;
}

void config_setIsBixinAPP(void) { g_bIsBixinAPP = true; }

void config_setSeSessionKey(uint8_t *data, uint32_t size) {
  if (data != NULL && size == SE_SESSION_KEY) {
    storage_set(KEY_SE_SESSIONKEY, data, size);
  } else {
    storage_delete(KEY_SE_SESSIONKEY);
  }
}

bool config_getSeSessionKey(uint8_t *dest, uint16_t dest_size) {
  uint16_t len = 0;
  secbool ret = storage_get(KEY_SE_SESSIONKEY, dest, dest_size, &len);
  if (sectrue != ret || len != SE_SESSION_KEY) {
    return false;
  }
  return true;
}

uint32_t config_getDeviceState() {
  if (sectrue != config_get_uint32(KEY_DEVICE_STATE, &deviceState)) {
    deviceState = 0;
  }
  return deviceState;
}

void config_setDeviceState(uint32_t device_state) {
  if (sectrue ==
      storage_set(KEY_DEVICE_STATE, &device_state, sizeof(device_state))) {
    deviceState = device_state;
  }
}

bool config_setSeedPin(const char *pin) {
  uint32_t seedpin;
  seedpin = pin_to_int(pin);
  if (0x00 == seedpin) {
    return false;
  }
  return se_set_value(KEY_SEED_PASSPHRASE, &seedpin, sizeof(uint32_t));
}

bool config_STSeedBackUp(void *plain_data, uint16_t plain_len,
                         void *cipher_data, uint16_t *cipher_len) {
  return se_st_seed_en(KEY_SEED_ST, plain_data, plain_len, cipher_data,
                       cipher_len);
}
bool config_STSeedRestore(void *cipher_data, uint16_t cipher_len,
                          void *plain_data, uint16_t *plain_len) {
  return se_st_seed_de(KEY_SEED_ST, cipher_data, cipher_len, plain_data,
                       plain_len);
}

bool config_stBackUpEntoryToSe(uint8_t *seed, uint8_t seed_len) {
  return st_backup_entory_to_se(KEY_ST_SEED_EXCHANGE, seed, seed_len);
}

bool config_stRestoreEntoryFromSe(uint8_t *seed, uint8_t *seed_len) {
  return st_restore_entory_from_se(KEY_ST_SEED_EXCHANGE, seed, seed_len);
}

uint32_t config_getPinFails(void) {
  uint32_t count = 0;
  if (g_bSelectSEFlag) {
    count = se_pinFailedCounter();
  } else {
    pin_get_fails(&count);
  }

  return count;
}
