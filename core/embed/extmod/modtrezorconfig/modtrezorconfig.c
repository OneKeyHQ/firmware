/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include "py/mphal.h"
#include "py/objstr.h"
#include "py/runtime.h"

#if MICROPY_PY_TREZORCONFIG

#include "embed/extmod/trezorobj.h"

#include "common.h"
#include "memzero.h"
#include "storage.h"

#ifndef TREZOR_EMULATOR
#include "bip39.h"
#include "device.h"
#include "display.h"
#include "emmc.h"
#include "mini_printf.h"
#include "se_thd89.h"

#define MAX_MNEMONIC_LEN 240

#endif

static secbool wrapped_ui_wait_callback(uint32_t wait, uint32_t progress,
                                        const char *message) {
  if (mp_obj_is_callable(MP_STATE_VM(trezorconfig_ui_wait_callback))) {
    mp_obj_t args[3] = {0};
    args[0] = mp_obj_new_int(wait);
    args[1] = mp_obj_new_int(progress);
    args[2] = mp_obj_new_str(message, strlen(message));
    if (mp_call_function_n_kw(MP_STATE_VM(trezorconfig_ui_wait_callback), 3, 0,
                              args) == mp_const_true) {
      return sectrue;
    }
  }
  return secfalse;
}

#if USE_THD89
#ifdef TREZOR_EMULATOR
#error "Emulator not support SE_THD89"
#endif

/// def init(
///    ui_wait_callback: Callable[[int, int, str], bool] | None = None
/// ) -> None:
///     """
///     Initializes the storage.  Must be called before any other method is
///     called from this module!
///     """
STATIC mp_obj_t mod_trezorconfig_init(size_t n_args, const mp_obj_t *args) {
  if (n_args > 0) {
    MP_STATE_VM(trezorconfig_ui_wait_callback) = args[0];
    se_set_ui_callback(wrapped_ui_wait_callback);
  }
  return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_init_obj, 0, 1,
                                           mod_trezorconfig_init);

/// def is_initialized() -> bool:
///     """
///     Returns True if device is initialized.
///     """
STATIC mp_obj_t mod_trezorconfig_is_initialized(void) {
  if (sectrue != se_isInitialized()) {
    return mp_const_false;
  }

  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_is_initialized_obj,
                                 mod_trezorconfig_is_initialized);

/// def unlock(pin: str, ext_salt: bytes | None) -> bool:
///     """
///     Attempts to unlock the storage with the given PIN and external salt.
///     Returns True on success, False on failure.
///     """
STATIC mp_obj_t mod_trezorconfig_unlock(mp_obj_t pin, mp_obj_t ext_salt) {
  mp_buffer_info_t pin_b = {0};
  mp_get_buffer_raise(pin, &pin_b, MP_BUFFER_READ);

  mp_buffer_info_t ext_salt_b = {0};
  ext_salt_b.buf = NULL;
  if (ext_salt != mp_const_none) {
    mp_get_buffer_raise(ext_salt, &ext_salt_b, MP_BUFFER_READ);
    if (ext_salt_b.len != EXTERNAL_SALT_SIZE)
      mp_raise_msg(&mp_type_ValueError, "Invalid length of external salt.");
  }

  display_clear();
  display_loader_ex(0, false, 0, 0xFFFF, 0x0000, NULL, 0, 0);
  secbool ret = secfalse;

  // verify se pin first when not in emulator
  ret = se_verifyPin(pin_b.buf);
  if (ret != sectrue) {
    return mp_const_false;
  }

  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorconfig_unlock_obj,
                                 mod_trezorconfig_unlock);

/// def check_pin(pin: str, ext_salt: bytes | None) -> bool:
///     """
///     Check the given PIN with the given external salt.
///     Returns True on success, False on failure.
///     """
STATIC mp_obj_t mod_trezorconfig_check_pin(mp_obj_t pin, mp_obj_t ext_salt) {
  return mod_trezorconfig_unlock(pin, ext_salt);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorconfig_check_pin_obj,
                                 mod_trezorconfig_check_pin);

/// def lock() -> None:
///     """
///     Locks the storage.
///     """
STATIC mp_obj_t mod_trezorconfig_lock(void) {
  se_clearSecsta();
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_lock_obj,
                                 mod_trezorconfig_lock);

/// def is_unlocked() -> bool:
///     """
///     Returns True if storage is unlocked, False otherwise.
///     """
STATIC mp_obj_t mod_trezorconfig_is_unlocked(void) {
  if (sectrue != se_getSecsta()) {
    return mp_const_false;
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_is_unlocked_obj,
                                 mod_trezorconfig_is_unlocked);

/// def has_pin() -> bool:
///     """
///     Returns True if storage has a configured PIN, False otherwise.
///     """
STATIC mp_obj_t mod_trezorconfig_has_pin(void) {
  if (sectrue != se_hasPin()) {
    return mp_const_false;
  }

  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_has_pin_obj,
                                 mod_trezorconfig_has_pin);

/// def get_pin_rem() -> int:
///     """
///     Returns the number of remaining PIN entry attempts.
///     """
STATIC mp_obj_t mod_trezorconfig_get_pin_rem(void) {
  uint8_t retry_cnts = 0;
  if (sectrue != se_getRetryTimes(&retry_cnts)) {
    mp_raise_msg(&mp_type_RuntimeError, "Failed to get pin retry times.");
  }

  return mp_obj_new_int_from_uint(retry_cnts);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_get_pin_rem_obj,
                                 mod_trezorconfig_get_pin_rem);

/// def change_pin(
///     oldpin: str,
///     newpin: str,
///     old_ext_salt: bytes | None,
///     new_ext_salt: bytes | None,
/// ) -> bool:
///     """
///     Change PIN and external salt. Returns True on success, False on failure.
///     """
STATIC mp_obj_t mod_trezorconfig_change_pin(size_t n_args,
                                            const mp_obj_t *args) {
  mp_buffer_info_t oldpin = {0};
  mp_get_buffer_raise(args[0], &oldpin, MP_BUFFER_READ);

  mp_buffer_info_t newpin = {0};
  mp_get_buffer_raise(args[1], &newpin, MP_BUFFER_READ);

  if (!se_hasPin()) {
    if (sectrue != se_setPin(newpin.buf)) {
      return mp_const_false;
    }

  } else {
    if (sectrue != se_changePin(oldpin.buf, newpin.buf)) {
      return mp_const_false;
    }
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_change_pin_obj, 4,
                                           4, mod_trezorconfig_change_pin);

/// def ensure_not_wipe_code(pin: str) -> None:
///     """
///     Wipes the device if the entered PIN is the wipe code.
///     """
STATIC mp_obj_t mod_trezorconfig_ensure_not_wipe_code(mp_obj_t pin) {
  mp_buffer_info_t pin_b = {0};
  mp_get_buffer_raise(pin, &pin_b, MP_BUFFER_READ);
  // storage_ensure_not_wipe_code(pin_b.buf, pin_b.len);
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorconfig_ensure_not_wipe_code_obj,
                                 mod_trezorconfig_ensure_not_wipe_code);

/// def has_wipe_code() -> bool:
///     """
///     Returns True if storage has a configured wipe code, False otherwise.
///     """
STATIC mp_obj_t mod_trezorconfig_has_wipe_code(void) {
  if (sectrue != se_hasWipeCode()) {
    return mp_const_false;
  }

  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_has_wipe_code_obj,
                                 mod_trezorconfig_has_wipe_code);

/// def change_wipe_code(
///     pin: str,
///     ext_salt: bytes | None,
///     wipe_code: str,
/// ) -> bool:
///     """
///     Change wipe code. Returns True on success, False on failure.
///     """
STATIC mp_obj_t mod_trezorconfig_change_wipe_code(size_t n_args,
                                                  const mp_obj_t *args) {
  mp_buffer_info_t pin_b = {0};
  mp_get_buffer_raise(args[0], &pin_b, MP_BUFFER_READ);

  mp_buffer_info_t wipe_code_b = {0};
  mp_get_buffer_raise(args[2], &wipe_code_b, MP_BUFFER_READ);

  if (pin_b.len == wipe_code_b.len) {
    if (memcmp(pin_b.buf, wipe_code_b.buf, pin_b.len) == 0) {
      mp_raise_msg(&mp_type_ValueError,
                   "The new PIN must be different from your wipe code.");
    }
  }

  if (sectrue != se_changeWipeCode(pin_b.buf, wipe_code_b.buf)) {
    return mp_const_false;
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(
    mod_trezorconfig_change_wipe_code_obj, 3, 3,
    mod_trezorconfig_change_wipe_code);

/// def get_needs_backup() -> bool:
///     """
///     Returns needs_backup.
///     """
STATIC mp_obj_t mod_trezorconfig_get_needs_backup(void) {
  bool needs_backup = false;
  if (sectrue != se_get_needs_backup(&needs_backup)) {
    return mp_const_false;
  }

  return needs_backup ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_get_needs_backup_obj,
                                 mod_trezorconfig_get_needs_backup);

/// def set_needs_backup(needs_backup: bool = False) -> bool:
///     """
///     Set needs_backup.
///     """
STATIC mp_obj_t mod_trezorconfig_set_needs_backup(mp_obj_t needs_backup) {
  bool needs_backup_b = mp_obj_is_true(needs_backup);

  if (sectrue != se_set_needs_backup(needs_backup_b)) {
    return mp_const_false;
  }

  return mp_const_true;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorconfig_set_needs_backup_obj,
                                 mod_trezorconfig_set_needs_backup);

/// def get(app: int, key: int, public: bool = False) -> bytes | None:
///     """
///     Gets the value of the given key for the given app (or None if not set).
///     Raises a RuntimeError if decryption or authentication of the stored
///     value fails.
///     """
STATIC mp_obj_t mod_trezorconfig_get(size_t n_args, const mp_obj_t *args) {
  uint32_t key = trezor_obj_get_uint(args[1]);

  bool is_private = key & (1 << 31);

  secbool (*reader)(uint16_t, void *, uint16_t) =
      is_private ? se_get_private_region : se_get_public_region;

  // key is position
  key &= ~(1 << 31);

  uint8_t temp[4] = {0};
  if (sectrue != reader(key, temp, 3)) {
    return mp_const_none;
  }
  // has flag
  if (temp[0] != 1) {
    return mp_const_none;
  }

  uint16_t len = 0;
  len = (temp[1] << 8) + temp[2];

  if (len == 0) {
    return mp_const_empty_bytes;
  }
  vstr_t vstr = {0};
  vstr_init_len(&vstr, len);
  vstr.len = len;
  if (sectrue != reader(key + 3, vstr.buf, vstr.len)) {
    vstr_clear(&vstr);
    mp_raise_msg(&mp_type_RuntimeError, "Failed to get value from storage.");
  }
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_get_obj, 2, 3,
                                           mod_trezorconfig_get);

/// def set(app: int, key: int, value: bytes, public: bool = False) -> None:
///     """
///     Sets a value of given key for given app.
///     """
STATIC mp_obj_t mod_trezorconfig_set(size_t n_args, const mp_obj_t *args) {
  uint32_t key = trezor_obj_get_uint(args[1]);
  bool is_private = key & (1 << 31);
  secbool (*writer)(uint16_t, const void *, uint16_t) =
      is_private ? se_set_private_region : se_set_public_region;

  mp_buffer_info_t value;
  mp_get_buffer_raise(args[2], &value, MP_BUFFER_READ);
  if (value.len > UINT16_MAX) {
    mp_raise_msg(&mp_type_RuntimeError, "Could not save value");
  }
  uint8_t temp[4] = {0};
  temp[0] = 1;
  temp[1] = (value.len >> 8) & 0xff;
  temp[2] = value.len & 0xff;

  if (sectrue != writer(key, temp, 3)) {
    mp_raise_msg(&mp_type_RuntimeError, "Could not save value");
  }
  if (sectrue != writer(key + 3, value.buf, value.len)) {
    mp_raise_msg(&mp_type_RuntimeError, "Could not save value");
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_set_obj, 3, 4,
                                           mod_trezorconfig_set);

/// def delete(
///     app: int, key: int, public: bool = False, writable_locked: bool = False
/// ) -> bool:
///     """
///     Deletes the given key of the given app.
///     """
STATIC mp_obj_t mod_trezorconfig_delete(size_t n_args, const mp_obj_t *args) {
  uint32_t key = trezor_obj_get_uint(args[1]);
  bool is_private = key & (1 << 31);
  secbool (*writer)(uint16_t, const void *, uint16_t) =
      is_private ? se_set_private_region : se_set_public_region;

  uint8_t temp[1] = {0};
  temp[0] = 0;
  if (sectrue != writer(key, temp, 1)) {
    mp_raise_msg(&mp_type_RuntimeError, "Could not delete key");
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_delete_obj, 2, 4,
                                           mod_trezorconfig_delete);

/// def set_counter(
///     app: int, key: int, count: int, writable_locked: bool = False
/// ) -> None:
///     """
///     Sets the given key of the given app as a counter with the given value.
///     """
STATIC mp_obj_t mod_trezorconfig_set_counter(size_t n_args,
                                             const mp_obj_t *args) {
  mp_uint_t count = trezor_obj_get_uint(args[2]);
  if (count > UINT32_MAX || !se_set_u2f_counter(count)) {
    mp_raise_msg(&mp_type_RuntimeError, "Failed to set u2f counter.");
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_set_counter_obj, 3,
                                           4, mod_trezorconfig_set_counter);

/// def next_counter(
///    app: int, key: int, writable_locked: bool = False,
/// ) -> int:
///     """
///     Increments the counter stored under the given key of the given app and
///     returns the new value.
///     """
STATIC mp_obj_t mod_trezorconfig_next_counter(size_t n_args,
                                              const mp_obj_t *args) {
  uint32_t count = 0;
  if (sectrue != se_get_u2f_counter(&count)) {
    mp_raise_msg(&mp_type_RuntimeError, "Failed to get u2f counter.");
  }
  return mp_obj_new_int_from_uint(count);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_next_counter_obj, 2,
                                           3, mod_trezorconfig_next_counter);

/// def wipe() -> None:
///     """
///     Erases the whole config. Use with caution!
///     """
STATIC mp_obj_t mod_trezorconfig_wipe(void) {
  if (sectrue != se_reset_storage()) {
    mp_raise_msg(&mp_type_RuntimeError, "Failed to reset storage.");
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_wipe_obj,
                                 mod_trezorconfig_wipe);

#ifndef TREZOR_EMULATOR
STATIC mp_obj_t mod_trezorconfig_se_import_mnemonic(mp_obj_t mnemonic) {
  mp_buffer_info_t mnemo = {0};
  mp_get_buffer_raise(mnemonic, &mnemo, MP_BUFFER_READ);

  if (sectrue != se_set_mnemonic(mnemo.buf, mnemo.len)) {
    return mp_const_false;
  }

  return mp_const_true;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorconfig_se_import_mnemonic_obj,
                                 mod_trezorconfig_se_import_mnemonic);

STATIC mp_obj_t mod_trezorconfig_se_export_mnemonic(void) {
  char mnemonic[MAX_MNEMONIC_LEN + 1];

  if (sectrue != se_exportMnemonic(mnemonic, sizeof(mnemonic))) {
    mp_raise_ValueError("Get se mnemonic");
  }

  mp_obj_t res = mp_obj_new_str_copy(&mp_type_bytes, (const uint8_t *)mnemonic,
                                     strlen(mnemonic));
  memzero(mnemonic, sizeof(mnemonic));
  return res;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_se_export_mnemonic_obj,
                                 mod_trezorconfig_se_export_mnemonic);

#endif
#else
/// def init(
///    ui_wait_callback: Callable[[int, int, str], bool] | None = None
/// ) -> None:
///     """
///     Initializes the storage.  Must be called before any other method is
///     called from this module!
///     """
STATIC mp_obj_t mod_trezorconfig_init(size_t n_args, const mp_obj_t *args) {
#ifdef TREZOR_EMULATOR
  if (n_args > 0) {
    MP_STATE_VM(trezorconfig_ui_wait_callback) = args[0];
    storage_init(wrapped_ui_wait_callback, HW_ENTROPY_DATA, HW_ENTROPY_LEN);
  } else {
    storage_init(NULL, HW_ENTROPY_DATA, HW_ENTROPY_LEN);
  }
  memzero(HW_ENTROPY_DATA, sizeof(HW_ENTROPY_DATA));
#else

#endif
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_init_obj, 0, 1,
                                           mod_trezorconfig_init);

/// def unlock(pin: str, ext_salt: bytes | None) -> bool:
///     """
///     Attempts to unlock the storage with the given PIN and external salt.
///     Returns True on success, False on failure.
///     """
STATIC mp_obj_t mod_trezorconfig_unlock(mp_obj_t pin, mp_obj_t ext_salt) {
  mp_buffer_info_t pin_b = {0};
  mp_get_buffer_raise(pin, &pin_b, MP_BUFFER_READ);

  mp_buffer_info_t ext_salt_b = {0};
  ext_salt_b.buf = NULL;
  if (ext_salt != mp_const_none) {
    mp_get_buffer_raise(ext_salt, &ext_salt_b, MP_BUFFER_READ);
    if (ext_salt_b.len != EXTERNAL_SALT_SIZE)
      mp_raise_msg(&mp_type_ValueError, "Invalid length of external salt.");
  }

#ifdef TREZOR_EMULATOR
  if (sectrue != storage_unlock(pin_b.buf, pin_b.len, ext_salt_b.buf)) {
    return mp_const_false;
  }
#else
  display_clear();
  display_loader_ex(0, false, 0, 0xFFFF, 0x0000, NULL, 0, 0);
  secbool ret = secfalse;
  // verify se pin first when not in emulator
  bool verified = se_verifyPin(pin_b.buf);
  ret = storage_unlock(pin_b.buf, pin_b.len, ext_salt_b.buf);
  if (ret != sectrue) {
    return mp_const_false;
  } else {
    if (!verified) {
      if (se_hasPin()) {
        se_reset_pin();
      }
      if (sectrue == storage_has_pin()) {
        if (!se_changePin((char *)PIN_EMPTY, pin_b.buf)) {
          return mp_const_false;
        }
      }
    }
  }

#endif

  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorconfig_unlock_obj,
                                 mod_trezorconfig_unlock);

/// def check_pin(pin: str, ext_salt: bytes | None) -> bool:
///     """
///     Check the given PIN with the given external salt.
///     Returns True on success, False on failure.
///     """
STATIC mp_obj_t mod_trezorconfig_check_pin(mp_obj_t pin, mp_obj_t ext_salt) {
  return mod_trezorconfig_unlock(pin, ext_salt);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_trezorconfig_check_pin_obj,
                                 mod_trezorconfig_check_pin);

/// def lock() -> None:
///     """
///     Locks the storage.
///     """
STATIC mp_obj_t mod_trezorconfig_lock(void) {
  storage_lock();
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_lock_obj,
                                 mod_trezorconfig_lock);

/// def is_unlocked() -> bool:
///     """
///     Returns True if storage is unlocked, False otherwise.
///     """
STATIC mp_obj_t mod_trezorconfig_is_unlocked(void) {
  if (sectrue != storage_is_unlocked()) {
    return mp_const_false;
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_is_unlocked_obj,
                                 mod_trezorconfig_is_unlocked);

/// def has_pin() -> bool:
///     """
///     Returns True if storage has a configured PIN, False otherwise.
///     """
STATIC mp_obj_t mod_trezorconfig_has_pin(void) {
  if (sectrue != storage_has_pin()) {
    return mp_const_false;
  }

  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_has_pin_obj,
                                 mod_trezorconfig_has_pin);

/// def get_pin_rem() -> int:
///     """
///     Returns the number of remaining PIN entry attempts.
///     """
STATIC mp_obj_t mod_trezorconfig_get_pin_rem(void) {
#ifdef TREZOR_EMULATOR
  return mp_obj_new_int_from_uint(storage_get_pin_rem());
#else
  int remain = storage_get_pin_rem();
  int fail_count = se_pinFailedCounter();
  int remain_se = 0;
  if (fail_count < PIN_MAX_TRIES) {
    remain_se = PIN_MAX_TRIES - fail_count;
  } else {
    remain_se = 0;
  }
  return mp_obj_new_int_from_uint(remain < remain_se ? remain : remain_se);
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_get_pin_rem_obj,
                                 mod_trezorconfig_get_pin_rem);

/// def change_pin(
///     oldpin: str,
///     newpin: str,
///     old_ext_salt: bytes | None,
///     new_ext_salt: bytes | None,
/// ) -> bool:
///     """
///     Change PIN and external salt. Returns True on success, False on failure.
///     """
STATIC mp_obj_t mod_trezorconfig_change_pin(size_t n_args,
                                            const mp_obj_t *args) {
  mp_buffer_info_t oldpin = {0};
  mp_get_buffer_raise(args[0], &oldpin, MP_BUFFER_READ);

  mp_buffer_info_t newpin = {0};
  mp_get_buffer_raise(args[1], &newpin, MP_BUFFER_READ);

  mp_buffer_info_t ext_salt_b = {0};
  const uint8_t *old_ext_salt = NULL;
  if (args[2] != mp_const_none) {
    mp_get_buffer_raise(args[2], &ext_salt_b, MP_BUFFER_READ);
    if (ext_salt_b.len != EXTERNAL_SALT_SIZE)
      mp_raise_msg(&mp_type_ValueError, "Invalid length of external salt.");
    old_ext_salt = ext_salt_b.buf;
  }
  const uint8_t *new_ext_salt = NULL;
  if (args[3] != mp_const_none) {
    mp_get_buffer_raise(args[3], &ext_salt_b, MP_BUFFER_READ);
    if (ext_salt_b.len != EXTERNAL_SALT_SIZE)
      mp_raise_msg(&mp_type_ValueError, "Invalid length of external salt.");
    new_ext_salt = ext_salt_b.buf;
  }
#ifdef TREZOR_EMULATOR
  if (sectrue != storage_change_pin(oldpin.buf, oldpin.len, newpin.buf,
                                    newpin.len, old_ext_salt, new_ext_salt)) {
    return mp_const_false;
  }
#else
  display_clear();
  display_loader_ex(0, false, 0, 0xFFFF, 0x0000, NULL, 0, 0);
  if (sectrue != storage_change_pin(oldpin.buf, oldpin.len, newpin.buf,
                                    newpin.len, old_ext_salt, new_ext_salt)) {
    return mp_const_false;
  }
  display_loader_ex(1000, false, 0, 0xFFFF, 0x0000, NULL, 0, 0);
  if (!se_setPin(newpin.buf)) {
    return mp_const_false;
  }
#endif
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_change_pin_obj, 4,
                                           4, mod_trezorconfig_change_pin);

/// def ensure_not_wipe_code(pin: str) -> None:
///     """
///     Wipes the device if the entered PIN is the wipe code.
///     """
STATIC mp_obj_t mod_trezorconfig_ensure_not_wipe_code(mp_obj_t pin) {
  mp_buffer_info_t pin_b = {0};
  mp_get_buffer_raise(pin, &pin_b, MP_BUFFER_READ);
  storage_ensure_not_wipe_code(pin_b.buf, pin_b.len);
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorconfig_ensure_not_wipe_code_obj,
                                 mod_trezorconfig_ensure_not_wipe_code);

/// def has_wipe_code() -> bool:
///     """
///     Returns True if storage has a configured wipe code, False otherwise.
///     """
STATIC mp_obj_t mod_trezorconfig_has_wipe_code(void) {
  if (sectrue != storage_has_wipe_code()) {
    return mp_const_false;
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_has_wipe_code_obj,
                                 mod_trezorconfig_has_wipe_code);

/// def change_wipe_code(
///     pin: str,
///     ext_salt: bytes | None,
///     wipe_code: str,
/// ) -> bool:
///     """
///     Change wipe code. Returns True on success, False on failure.
///     """
STATIC mp_obj_t mod_trezorconfig_change_wipe_code(size_t n_args,
                                                  const mp_obj_t *args) {
  mp_buffer_info_t pin_b = {0};
  mp_get_buffer_raise(args[0], &pin_b, MP_BUFFER_READ);

  mp_buffer_info_t wipe_code_b = {0};
  mp_get_buffer_raise(args[2], &wipe_code_b, MP_BUFFER_READ);

  mp_buffer_info_t ext_salt_b = {0};
  const uint8_t *ext_salt = NULL;
  if (args[1] != mp_const_none) {
    mp_get_buffer_raise(args[1], &ext_salt_b, MP_BUFFER_READ);
    if (ext_salt_b.len != EXTERNAL_SALT_SIZE)
      mp_raise_msg(&mp_type_ValueError, "Invalid length of external salt.");
    ext_salt = ext_salt_b.buf;
  }

  if (sectrue != storage_change_wipe_code(pin_b.buf, pin_b.len, ext_salt,
                                          wipe_code_b.buf, wipe_code_b.len)) {
    return mp_const_false;
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(
    mod_trezorconfig_change_wipe_code_obj, 3, 3,
    mod_trezorconfig_change_wipe_code);

/// def get(app: int, key: int, public: bool = False) -> bytes | None:
///     """
///     Gets the value of the given key for the given app (or None if not set).
///     Raises a RuntimeError if decryption or authentication of the stored
///     value fails.
///     """
STATIC mp_obj_t mod_trezorconfig_get(size_t n_args, const mp_obj_t *args) {
  uint8_t app = trezor_obj_get_uint8(args[0]);
  if (app == 0 || app > MAX_APPID) {
    mp_raise_msg(&mp_type_ValueError, "Invalid app ID.");
  }
  uint8_t key = trezor_obj_get_uint8(args[1]);
  if (n_args > 2 && args[2] == mp_const_true) {
    app |= FLAG_PUBLIC;
  }
  uint16_t appkey = (app << 8) | key;
  uint16_t len = 0;
  if (sectrue != storage_get(appkey, NULL, 0, &len)) {
    return mp_const_none;
  }
  if (len == 0) {
    return mp_const_empty_bytes;
  }
  vstr_t vstr = {0};
  vstr_init_len(&vstr, len);
  if (sectrue != storage_get(appkey, vstr.buf, vstr.len, &len)) {
    vstr_clear(&vstr);
    mp_raise_msg(&mp_type_RuntimeError, "Failed to get value from storage.");
  }
  return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_get_obj, 2, 3,
                                           mod_trezorconfig_get);

/// def set(app: int, key: int, value: bytes, public: bool = False) -> None:
///     """
///     Sets a value of given key for given app.
///     """
STATIC mp_obj_t mod_trezorconfig_set(size_t n_args, const mp_obj_t *args) {
  uint8_t app = trezor_obj_get_uint8(args[0]);
  if (app == 0 || app > MAX_APPID) {
    mp_raise_msg(&mp_type_ValueError, "Invalid app ID.");
  }
  uint8_t key = trezor_obj_get_uint8(args[1]);
  if (n_args > 3 && args[3] == mp_const_true) {
    app |= FLAG_PUBLIC;
  }
  uint16_t appkey = (app << 8) | key;
  mp_buffer_info_t value;
  mp_get_buffer_raise(args[2], &value, MP_BUFFER_READ);
  if (value.len > UINT16_MAX ||
      sectrue != storage_set(appkey, value.buf, value.len)) {
    mp_raise_msg(&mp_type_RuntimeError, "Could not save value");
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_set_obj, 3, 4,
                                           mod_trezorconfig_set);

/// def delete(
///     app: int, key: int, public: bool = False, writable_locked: bool = False
/// ) -> bool:
///     """
///     Deletes the given key of the given app.
///     """
STATIC mp_obj_t mod_trezorconfig_delete(size_t n_args, const mp_obj_t *args) {
  uint8_t app = trezor_obj_get_uint8(args[0]);
  if (app == 0 || app > MAX_APPID) {
    mp_raise_msg(&mp_type_ValueError, "Invalid app ID.");
  }
  uint8_t key = trezor_obj_get_uint8(args[1]);
  if (n_args > 2 && args[2] == mp_const_true) {
    app |= FLAG_PUBLIC;
  }
  if (n_args > 3 && args[3] == mp_const_true) {
    app |= FLAGS_WRITE;
    if (args[2] != mp_const_true) {
      mp_raise_msg(&mp_type_ValueError, "Writable entry must be public.");
    }
  }
  uint16_t appkey = (app << 8) | key;
  if (sectrue != storage_delete(appkey)) {
    return mp_const_false;
  }
  return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_delete_obj, 2, 4,
                                           mod_trezorconfig_delete);

/// def set_counter(
///     app: int, key: int, count: int, writable_locked: bool = False
/// ) -> None:
///     """
///     Sets the given key of the given app as a counter with the given value.
///     """
STATIC mp_obj_t mod_trezorconfig_set_counter(size_t n_args,
                                             const mp_obj_t *args) {
  uint8_t app = trezor_obj_get_uint8(args[0]);
  if (app == 0 || app > MAX_APPID) {
    mp_raise_msg(&mp_type_ValueError, "Invalid app ID.");
  }
  uint8_t key = trezor_obj_get_uint8(args[1]);
  if (n_args > 3 && args[3] == mp_const_true) {
    app |= FLAGS_WRITE;
  } else {
    app |= FLAG_PUBLIC;
  }
  uint16_t appkey = (app << 8) | key;
  mp_uint_t count = trezor_obj_get_uint(args[2]);
  if (count > UINT32_MAX || sectrue != storage_set_counter(appkey, count)) {
    mp_raise_msg(&mp_type_RuntimeError, "Failed to set value in storage.");
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_set_counter_obj, 3,
                                           4, mod_trezorconfig_set_counter);

/// def next_counter(
///    app: int, key: int, writable_locked: bool = False,
/// ) -> int:
///     """
///     Increments the counter stored under the given key of the given app and
///     returns the new value.
///     """
STATIC mp_obj_t mod_trezorconfig_next_counter(size_t n_args,
                                              const mp_obj_t *args) {
  uint8_t app = trezor_obj_get_uint8(args[0]);
  if (app == 0 || app > MAX_APPID) {
    mp_raise_msg(&mp_type_ValueError, "Invalid app ID.");
  }
  uint8_t key = trezor_obj_get_uint8(args[1]);
  if (n_args > 2 && args[2] == mp_const_true) {
    app |= FLAGS_WRITE;
  } else {
    app |= FLAG_PUBLIC;
  }
  uint16_t appkey = (app << 8) | key;
  uint32_t count = 0;
  if (sectrue != storage_next_counter(appkey, &count)) {
    mp_raise_msg(&mp_type_RuntimeError, "Failed to set value in storage.");
  }
  return mp_obj_new_int_from_uint(count);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_trezorconfig_next_counter_obj, 2,
                                           3, mod_trezorconfig_next_counter);

/// def wipe() -> None:
///     """
///     Erases the whole config. Use with caution!
///     """
STATIC mp_obj_t mod_trezorconfig_wipe(void) {
  storage_wipe();
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_wipe_obj,
                                 mod_trezorconfig_wipe);

#ifndef TREZOR_EMULATOR
STATIC mp_obj_t mod_trezorconfig_se_import_mnemonic(mp_obj_t mnemonic) {
  mp_buffer_info_t mnemo = {0};
  mp_get_buffer_raise(mnemonic, &mnemo, MP_BUFFER_READ);
  const char *pmnemonic = mnemo.len > 0 ? mnemo.buf : "";

  uint8_t mnemonic_bits[64] = {0};
  int mnemonic_bits_len = mnemonic_to_bits(pmnemonic, mnemonic_bits);
  if (mnemonic_bits_len == 0 || mnemonic_bits_len % 33 != 0) {
    mp_raise_ValueError("Invalid mnemonic");
  }
  int strength = (mnemonic_bits_len / 11) * 8 * 4 / 3;
  se_setSeedStrength(strength);
  if (!se_importSeed(mnemonic_bits)) {
    return mp_const_false;
  }
  return mp_const_true;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_trezorconfig_se_import_mnemonic_obj,
                                 mod_trezorconfig_se_import_mnemonic);

STATIC mp_obj_t mod_trezorconfig_se_export_mnemonic(void) {
  uint8_t seed[64] = {0};
  uint32_t strength = 0;

  if (!se_isInitialized()) {
    // mp_raise_ValueError("Device not initialized");
    return mp_const_none;
  }
  if (!se_getSeedStrength(&strength)) {
    mp_raise_ValueError("Get mnemonic strength");
  }

  if (!se_export_seed(seed)) {
    mp_raise_ValueError("Get mnemonic seed");
  }
  const char *mnemonic = mnemonic_from_data(seed, strength / 8);
  mp_obj_t res = mp_obj_new_str_copy(&mp_type_bytes, (const uint8_t *)mnemonic,
                                     strlen(mnemonic));
  mnemonic_clear();
  return res;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_se_export_mnemonic_obj,
                                 mod_trezorconfig_se_export_mnemonic);

#endif
#endif

#ifndef TREZOR_EMULATOR
/// def get_serial() -> str:
///     """
///     get device serial
///     """
STATIC mp_obj_t mod_trezorconfig_get_serial(void) {
  mp_obj_t res;

  char *dev_serial;
  if (device_get_serial(&dev_serial)) {
    res = mp_obj_new_str_copy(&mp_type_bytes, (const uint8_t *)dev_serial,
                              strlen(dev_serial));
  } else {
    res = mp_obj_new_str_copy(&mp_type_bytes, (const uint8_t *)"NULL",
                              strlen("NULL"));
  }

  return res;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_get_serial_obj,
                                 mod_trezorconfig_get_serial);

/// def get_capacity() -> str:
///     """
///     get emmc capacity
///     """
STATIC mp_obj_t mod_trezorconfig_get_capacity(void) {
  char cap_info[32] = {0};
  uint64_t cap = emmc_get_capacity_in_bytes();

  if (cap > (1024 * 1024 * 1024)) {
    mini_snprintf(cap_info, sizeof(cap_info), "%d GB",
                  (unsigned int)(cap >> 30));
  } else if (cap > (1024 * 1024)) {
    mini_snprintf(cap_info, sizeof(cap_info), "%d MB",
                  (unsigned int)(cap >> 20));
  } else {
    mini_snprintf(cap_info, sizeof(cap_info), "%d Bytes", (unsigned int)cap);
  }
  return mp_obj_new_str_copy(&mp_type_bytes, (const uint8_t *)cap_info,
                             strlen(cap_info));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_trezorconfig_get_capacity_obj,
                                 mod_trezorconfig_get_capacity);
#endif

STATIC const mp_rom_map_elem_t mp_module_trezorconfig_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_trezorconfig)},
    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mod_trezorconfig_init_obj)},
    {MP_ROM_QSTR(MP_QSTR_check_pin),
     MP_ROM_PTR(&mod_trezorconfig_check_pin_obj)},
    {MP_ROM_QSTR(MP_QSTR_unlock), MP_ROM_PTR(&mod_trezorconfig_unlock_obj)},
    {MP_ROM_QSTR(MP_QSTR_lock), MP_ROM_PTR(&mod_trezorconfig_lock_obj)},
    {MP_ROM_QSTR(MP_QSTR_is_unlocked),
     MP_ROM_PTR(&mod_trezorconfig_is_unlocked_obj)},
    {MP_ROM_QSTR(MP_QSTR_has_pin), MP_ROM_PTR(&mod_trezorconfig_has_pin_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_pin_rem),
     MP_ROM_PTR(&mod_trezorconfig_get_pin_rem_obj)},
    {MP_ROM_QSTR(MP_QSTR_change_pin),
     MP_ROM_PTR(&mod_trezorconfig_change_pin_obj)},
    {MP_ROM_QSTR(MP_QSTR_ensure_not_wipe_code),
     MP_ROM_PTR(&mod_trezorconfig_ensure_not_wipe_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_has_wipe_code),
     MP_ROM_PTR(&mod_trezorconfig_has_wipe_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_change_wipe_code),
     MP_ROM_PTR(&mod_trezorconfig_change_wipe_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&mod_trezorconfig_get_obj)},
    {MP_ROM_QSTR(MP_QSTR_set), MP_ROM_PTR(&mod_trezorconfig_set_obj)},
    {MP_ROM_QSTR(MP_QSTR_delete), MP_ROM_PTR(&mod_trezorconfig_delete_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_counter),
     MP_ROM_PTR(&mod_trezorconfig_set_counter_obj)},
    {MP_ROM_QSTR(MP_QSTR_next_counter),
     MP_ROM_PTR(&mod_trezorconfig_next_counter_obj)},
    {MP_ROM_QSTR(MP_QSTR_wipe), MP_ROM_PTR(&mod_trezorconfig_wipe_obj)},
#ifndef TREZOR_EMULATOR
    {MP_ROM_QSTR(MP_QSTR_se_import_mnemonic),
     MP_ROM_PTR(&mod_trezorconfig_se_import_mnemonic_obj)},
    {MP_ROM_QSTR(MP_QSTR_se_export_mnemonic),
     MP_ROM_PTR(&mod_trezorconfig_se_export_mnemonic_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_serial),
     MP_ROM_PTR(&mod_trezorconfig_get_serial_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_capacity),
     MP_ROM_PTR(&mod_trezorconfig_get_capacity_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_needs_backup),
     MP_ROM_PTR(&mod_trezorconfig_get_needs_backup_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_needs_backup),
     MP_ROM_PTR(&mod_trezorconfig_set_needs_backup_obj)},
#endif
#if USE_THD89
    {MP_ROM_QSTR(MP_QSTR_is_initialized),
     MP_ROM_PTR(&mod_trezorconfig_is_initialized_obj)},

#endif
};
STATIC MP_DEFINE_CONST_DICT(mp_module_trezorconfig_globals,
                            mp_module_trezorconfig_globals_table);

const mp_obj_module_t mp_module_trezorconfig = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mp_module_trezorconfig_globals,
};

MP_REGISTER_MODULE(MP_QSTR_trezorconfig, mp_module_trezorconfig,
                   MICROPY_PY_TREZORCONFIG);

#endif  // MICROPY_PY_TREZORCONFIG
