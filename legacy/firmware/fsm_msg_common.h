/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2018 Pavol Rusnak <stick@satoshilabs.com>
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
#include "flash.h"
#include "menu_list.h"
#include "mi2c.h"
#include "se_chip.h"
#include "storage.h"

extern char bootloader_version[8];

bool get_features(Features *resp) {
  char *sn_version = NULL;
  char *serial = NULL;
  resp->has_fw_vendor = true;
#if EMULATOR
  strlcpy(resp->fw_vendor, "EMULATOR", sizeof(resp->fw_vendor));
#else
  const image_header *hdr =
      (const image_header *)FLASH_PTR(FLASH_FWHEADER_START);
  // allow both v2 and v3 signatures
  if (SIG_OK == signatures_match(hdr, NULL)) {
    strlcpy(resp->fw_vendor, "OneKey", sizeof(resp->fw_vendor));
  } else {
    strlcpy(resp->fw_vendor, "UNSAFE, DO NOT USE!", sizeof(resp->fw_vendor));
  }
#endif
  resp->has_vendor = true;
  strlcpy(resp->vendor, "trezor.io", sizeof(resp->vendor));
  resp->major_version = VERSION_MAJOR;
  resp->minor_version = VERSION_MINOR;
  resp->patch_version = VERSION_PATCH;
  resp->has_device_id = true;
  strlcpy(resp->device_id, config_uuid_str, sizeof(resp->device_id));
  resp->has_pin_protection = true;
  resp->pin_protection = config_hasPin();
  resp->has_passphrase_protection = true;
  config_getPassphraseProtection(&(resp->passphrase_protection));
#ifdef SCM_REVISION
  int len = sizeof(SCM_REVISION) - 1;
  resp->has_revision = true;
  memcpy(resp->revision.bytes, SCM_REVISION, len);
  resp->revision.size = len;
#endif
  resp->has_bootloader_hash = true;
  resp->bootloader_hash.size =
      memory_bootloader_hash(resp->bootloader_hash.bytes);

  resp->has_language =
      config_getLanguage(resp->language, sizeof(resp->language));
  resp->has_label = config_getLabel(resp->label, sizeof(resp->label));
  resp->has_initialized = true;
  resp->initialized = config_isInitialized();
  resp->has_imported = config_getImported(&(resp->imported));
  resp->has_unlocked = true;
  resp->unlocked = session_isUnlocked();
  resp->has_needs_backup = true;
  config_getNeedsBackup(&(resp->needs_backup));
  resp->has_unfinished_backup = true;
  config_getUnfinishedBackup(&(resp->unfinished_backup));
  resp->has_no_backup = true;
  config_getNoBackup(&(resp->no_backup));
  resp->has_flags = config_getFlags(&(resp->flags));
  resp->has_model = true;
  strlcpy(resp->model, "1", sizeof(resp->model));
  resp->has_safety_checks = true;
  resp->safety_checks = config_getSafetyCheckLevel();
  resp->has_busy = true;
  resp->busy = (system_millis_busy_deadline > timer_ms());
  if (session_isUnlocked()) {
    resp->has_wipe_code_protection = true;
    resp->wipe_code_protection = config_hasWipeCode();
    resp->has_auto_lock_delay_ms = true;
    resp->auto_lock_delay_ms = config_getAutoLockDelayMs();
  }

#if BITCOIN_ONLY
  resp->capabilities_count = 2;
  resp->capabilities[0] = Capability_Capability_Bitcoin;
  resp->capabilities[1] = Capability_Capability_Crypto;
#else
  resp->capabilities_count = 7;
  resp->capabilities[0] = Capability_Capability_Bitcoin;
  resp->capabilities[1] = Capability_Capability_Bitcoin_like;
  resp->capabilities[2] = Capability_Capability_Crypto;
  resp->capabilities[3] = Capability_Capability_Ethereum;
  resp->capabilities[4] = Capability_Capability_NEM;
  resp->capabilities[5] = Capability_Capability_Stellar;
  resp->capabilities[6] = Capability_Capability_U2F;
#endif
  if (ble_name_state()) {
    resp->has_ble_name = true;
    strlcpy(resp->ble_name, ble_get_name(), sizeof(resp->ble_name));
  }
  if (ble_ver_state()) {
    resp->has_ble_ver = true;
    strlcpy(resp->ble_ver, ble_get_ver(), sizeof(resp->ble_ver));
  }
  if (ble_switch_state()) {
    resp->has_ble_enable = true;
    resp->ble_enable = ble_get_switch();
  }
  resp->has_se_enable = true;
  resp->se_enable = config_getWhetherUseSE();
  sn_version = se_get_version();
  if (sn_version) {
    resp->has_se_ver = true;
    memcpy(resp->se_ver, sn_version, strlen(sn_version));
  }

  resp->has_backup_only = true;
  resp->backup_only = config_getMnemonicsImported();

  resp->has_onekey_version = true;
  strlcpy(resp->onekey_version, ONEKEY_VERSION, sizeof(resp->onekey_version));

  if (se_get_sn(&serial)) {
    resp->has_onekey_serial = true;
    strlcpy(resp->onekey_serial, serial, sizeof(resp->onekey_serial));
  }

#ifdef BUILD_ID
  resp->has_build_id = true;
  strlcpy(resp->build_id, BUILD_ID, sizeof(resp->build_id));
#endif
#if !EMULATOR
  resp->has_bootloader_version = true;
  strlcpy(resp->bootloader_version, bootloader_version,
          sizeof(resp->bootloader_version));
#endif

  resp->has_coin_switch = true;
  resp->coin_switch |=
      config_getCoinSwitch(COIN_SWITCH_ETH_EIP712) ? COIN_SWITCH_ETH_EIP712 : 0;
  resp->coin_switch |=
      config_getCoinSwitch(COIN_SWITCH_SOLANA) ? COIN_SWITCH_SOLANA : 0;

#if !EMULATOR
  if (battery_cap != 0xff) {
    resp->has_battery_level = true;
    resp->battery_level = battery_cap;
  }
#endif

  return resp;
}

void fsm_msgInitialize(const Initialize *msg) {
  fsm_abortWorkflows();

  uint8_t *session_id;
  if (msg && msg->has_session_id) {
    session_id = session_startSession(msg->session_id.bytes);
  } else {
    session_id = session_startSession(NULL);
  }

  RESP_INIT(Features);
  get_features(resp);

  resp->has_session_id = true;
  memcpy(resp->session_id.bytes, session_id, sizeof(resp->session_id.bytes));
  resp->session_id.size = sizeof(resp->session_id.bytes);

  layoutHome();
  msg_write(MessageType_MessageType_Features, resp);
}

void fsm_msgGetFeatures(const GetFeatures *msg) {
  (void)msg;
  RESP_INIT(Features);
  get_features(resp);
  msg_write(MessageType_MessageType_Features, resp);
}

void fsm_msgPing(const Ping *msg) {
  RESP_INIT(Success);

  if (msg->has_button_protection && msg->button_protection) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("answer to ping?"), NULL,
                      NULL, NULL, NULL);
    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }

  if (msg->has_message) {
    resp->has_message = true;
    memcpy(&(resp->message), &(msg->message), sizeof(resp->message));
  }
  msg_write(MessageType_MessageType_Success, resp);
  layoutHome();
}

void fsm_msgChangePin(const ChangePin *msg) {
  // CHECK_INITIALIZED
  if (!config_isInitialized() && !config_getMnemonicsImported()) {
    fsm_sendFailure(FailureType_Failure_NotInitialized, NULL);
    return;
  }

  bool removal = msg->has_remove && msg->remove;
  bool button_confirm = true;
  if (removal) {
    if (config_hasPin()) {
      layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                        _("Do you really want to"), _("remove current PIN?"),
                        NULL, NULL, NULL, NULL);
    } else {
      fsm_sendSuccess(_("PIN removed"));
      return;
    }
  } else {
    if (config_hasPin()) {
      layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                        _("Do you really want to"), _("change current PIN?"),
                        NULL, NULL, NULL, NULL);
    } else {
      if (g_bIsBixinAPP) {
        button_confirm = false;
      } else {
        layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                          _("Do you really want to"), _("set new PIN?"), NULL,
                          NULL, NULL, NULL);
      }
    }
  }
  if (button_confirm &&
      !protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }
  if (protectChangePin(removal)) {
    i2c_set_wait(false);
    if (removal) {
      fsm_sendSuccess(_("PIN removed"));
    } else {
      fsm_sendSuccess(_("PIN changed"));
    }
  }

  layoutHome();
}

void fsm_msgChangeWipeCode(const ChangeWipeCode *msg) {
  CHECK_INITIALIZED
  if (g_bIsBixinAPP) {
    CHECK_PIN
  }

  bool removal = msg->has_remove && msg->remove;
  bool has_wipe_code = config_hasWipeCode();

  if (removal) {
    // Note that if storage is locked, then config_hasWipeCode() returns false.
    if (has_wipe_code || !session_isUnlocked()) {
      layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                        _("Do you really want to"), _("disable wipe code"),
                        _("protection?"), NULL, NULL, NULL);
    } else {
      fsm_sendSuccess(_("Wipe code removed"));
      return;
    }
  } else {
    if (has_wipe_code) {
      layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                        _("Do you really want to"), _("change the current"),
                        _("wipe code?"), NULL, NULL, NULL);
    } else {
      layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                        _("Do you really want to"), _("set a new wipe code?"),
                        NULL, NULL, NULL, NULL);
    }
  }
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  if (protectChangeWipeCode(removal)) {
    if (removal) {
      fsm_sendSuccess(_("Wipe code removed"));
    } else if (has_wipe_code) {
      fsm_sendSuccess(_("Wipe code changed"));
    } else {
      fsm_sendSuccess(_("Wipe code set"));
    }
  }

  layoutHome();
}

void fsm_msgWipeDevice(const WipeDevice *msg) {
  (void)msg;
  uint8_t key = KEY_NULL;

  if (!layoutEraseDevice()) {
    i2c_set_wait(false);
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }
  if (!protectPinOnDevice(false, true)) {
    i2c_set_wait(false);
    fsm_sendFailure(FailureType_Failure_PinInvalid, NULL);
    layoutHome();
    return;
  }
  layoutDialogAdapterEx(
      _("Erase Device"), &bmp_bottom_left_delete, _("Back"),
      &bmp_bottom_right_confirm, _("Reset "),
      _("Are you sure to reset this \ndevice? This action can not be undo!"),
      NULL, NULL, NULL, NULL);
  key = protectWaitKey(0, 1);
  if (key != KEY_CONFIRM) {
    i2c_set_wait(false);
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  uint8_t ui_language_bak = ui_language;

  config_wipe();
  if (ui_language_bak) {
    ui_language = ui_language_bak;
  }
  layoutDialogAdapterEx(
      _("Reset Complete"), NULL, NULL, &bmp_bottom_right_confirm, _("Reset "),
      _("The device is reset, restart now."), NULL, NULL, NULL, NULL);
  protectWaitKey(0, 0);

  // the following does not work on Mac anyway :-/ Linux/Windows are fine, so it
  // is not needed usbReconnect(); // force re-enumeration because of the serial
  // number change
  i2c_set_wait(false);
  fsm_sendSuccess(_("Device wiped"));
  layoutHome();
#if !EMULATOR
  // svc_system_reset();
  reset_to_firmware();
#endif
}

void fsm_msgGetEntropy(const GetEntropy *msg) {
  CHECK_PIN

#if !DEBUG_RNG
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Do you really want to"), _("send entropy?"), NULL, NULL,
                    NULL, NULL);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }
#endif
  RESP_INIT(Entropy);
  uint32_t len = msg->size;
  if (len > 1024) {
    len = 1024;
  }
  resp->entropy.size = len;
  random_buffer(resp->entropy.bytes, len);
  msg_write(MessageType_MessageType_Entropy, resp);
  layoutHome();
}

#if DEBUG_LINK

void fsm_msgLoadDevice(const LoadDevice *msg) {
  CHECK_PIN

  CHECK_NOT_INITIALIZED

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("I take the risk"), NULL,
                    _("Loading private seed"), _("is not recommended."),
                    _("Continue only if you"), _("know what you are"),
                    _("doing!"), NULL);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  if (msg->mnemonics_count && !(msg->has_skip_checksum && msg->skip_checksum)) {
    if (!mnemonic_check(msg->mnemonics[0])) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Mnemonic with wrong checksum provided"));
      layoutHome();
      return;
    }
  }

  config_loadDevice(msg);
  fsm_sendSuccess(_("Device loaded"));
  layoutHome();
}

#endif

void fsm_msgResetDevice(const ResetDevice *msg) {
  CHECK_PIN
  CHECK_NOT_INITIALIZED

  CHECK_PARAM(!msg->has_strength || msg->strength == 128 ||
                  msg->strength == 192 || msg->strength == 256,
              _("Invalid seed strength"));

  reset_init(msg->has_display_random && msg->display_random,
             msg->has_strength ? msg->strength : 128,
             msg->has_passphrase_protection && msg->passphrase_protection,
             msg->has_pin_protection && msg->pin_protection,
             msg->has_language ? msg->language : 0,
             msg->has_label ? msg->label : 0,
             msg->has_u2f_counter ? msg->u2f_counter : 0,
             msg->has_skip_backup ? msg->skip_backup : false,
             msg->has_no_backup ? msg->no_backup : false);
}

void fsm_msgEntropyAck(const EntropyAck *msg) {
  reset_entropy(msg->entropy.bytes, msg->entropy.size);
}

void fsm_msgBackupDevice(const BackupDevice *msg) {
  (void)msg;

  CHECK_INITIALIZED

  CHECK_PIN_UNCACHED

  char mnemonic[MAX_MNEMONIC_LEN + 1];
  if (config_getMnemonic(mnemonic, sizeof(mnemonic))) {
    reset_backup(true, mnemonic);
  }
  memzero(mnemonic, sizeof(mnemonic));
}

void fsm_msgCancel(const Cancel *msg) {
  (void)msg;
  fsm_abortWorkflows();
  fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
}

void fsm_msgLockDevice(const LockDevice *msg) {
  (void)msg;
  config_lockDevice();
  layoutScreensaver();
  fsm_sendSuccess(_("Session cleared"));
}

bool fsm_getLang(const ApplySettings *msg) {
  if (!strcmp(msg->language, "zh-CN") || !strcmp(msg->language, "chinese"))
    return true;
  else
    return false;
}

void fsm_msgEndSession(const EndSession *msg) {
  (void)msg;
  session_endCurrentSession();
  fsm_sendSuccess(_("Session ended"));
}

void fsm_msgApplySettings(const ApplySettings *msg) {
  CHECK_PARAM(
      !msg->has_passphrase_always_on_device,
      _("This firmware is incapable of passphrase entry on the device."));

  CHECK_PARAM(msg->has_label || msg->has_language || msg->has_use_passphrase ||
                  msg->has_homescreen || msg->has_auto_lock_delay_ms ||
                  msg->has_safety_checks || msg->has_use_ble ||
                  msg->has_is_bixinapp,
              _("No setting provided"));

  if (!msg->has_is_bixinapp) CHECK_PIN

  if (msg->has_is_bixinapp) {
    if (msg->has_label || msg->has_language || msg->has_use_passphrase ||
        msg->has_homescreen || msg->has_auto_lock_delay_ms ||
        msg->has_use_ble) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled,
                      "you should set bixin_app flag only");
      layoutHome();
      return;
    }
  }

  if (msg->has_label) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("change name to"),
                      msg->label, "?", NULL, NULL);
    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }
  if (msg->has_language) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("change language to"),
                      (fsm_getLang(msg) ? "中文" : "English"), "?", NULL, NULL);
    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }
  if (msg->has_use_passphrase) {
    layoutDialogSwipe(
        &bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
        _("Do you really want to"),
        msg->use_passphrase ? _("enable passphrase") : _("disable passphrase"),
        _("protection?"), NULL, NULL, NULL);
    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }
  if (msg->has_homescreen) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("change the home screen"),
                      NULL, NULL, NULL, NULL);
    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }

  if (msg->has_auto_lock_delay_ms) {
    if (msg->auto_lock_delay_ms < MIN_AUTOLOCK_DELAY_MS) {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Auto-lock delay too short"));
      layoutHome();
      return;
    }
    if (msg->auto_lock_delay_ms > MAX_AUTOLOCK_DELAY_MS) {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Auto-lock delay too long"));
      layoutHome();
      return;
    }
    layoutConfirmAutoLockDelay(msg->auto_lock_delay_ms);

    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }
  if ((msg->has_fastpay_pin) || (msg->has_fastpay_confirm) ||
      (msg->has_fastpay_money_limit) || (msg->has_fastpay_times)) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("change fastpay settings"),
                      NULL, NULL, NULL, NULL);
    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }
  if (msg->has_use_ble) {
    layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                      _("Do you really want to"), _("change bluetooth"),
                      _("status always?"), NULL, NULL, NULL);

    if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
      fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
      layoutHome();
      return;
    }
  }
  if ((msg->has_use_se) && (config_isInitialized())) {
    fsm_sendSuccess(_("Can't change se setting after device initialized"));
    layoutHome();
    return;
  }
  if (msg->has_safety_checks) {
    if (msg->safety_checks == SafetyCheckLevel_Strict ||
        msg->safety_checks == SafetyCheckLevel_PromptTemporarily) {
      layoutConfirmSafetyChecks(msg->safety_checks);
      if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
        fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
        layoutHome();
        return;
      }
    } else {
      fsm_sendFailure(FailureType_Failure_ProcessError,
                      _("Unsupported safety-checks setting"));
      layoutHome();
      return;
    }
  }

  if (msg->has_label) {
    config_setLabel(msg->label);
    i2c_set_wait(false);
  }
  if (msg->has_language) {
    config_setLanguage(msg->language);
  }
  if (msg->has_use_passphrase) {
    config_setPassphraseProtection(msg->use_passphrase);
  }
  if (msg->has_homescreen) {
    config_setHomescreen(msg->homescreen.bytes, msg->homescreen.size);
  }
  if (msg->has_auto_lock_delay_ms) {
    config_setSleepDelayMs(msg->auto_lock_delay_ms);
    menu_autolock_added_custom();
  }
  if (msg->has_use_ble) {
    config_setBleTrans(msg->use_ble);
  }
  if (msg->has_is_bixinapp) {
    config_setIsBixinAPP();
  }
  if (msg->has_safety_checks) {
    config_setSafetyCheckLevel(msg->safety_checks);
  }
  fsm_sendSuccess(_("Settings applied"));
  layoutHome();
}

void fsm_msgApplyFlags(const ApplyFlags *msg) {
  CHECK_PIN

  config_applyFlags(msg->flags);
  fsm_sendSuccess(_("Flags applied"));
}

void fsm_msgRecoveryDevice(const RecoveryDevice *msg) {
  CHECK_PIN_UNCACHED

  const bool dry_run = msg->has_dry_run ? msg->dry_run : false;
  if (!dry_run) {
    CHECK_NOT_INITIALIZED
  } else {
    CHECK_INITIALIZED
    CHECK_PARAM(!msg->has_passphrase_protection && !msg->has_pin_protection &&
                    !msg->has_language && !msg->has_label &&
                    !msg->has_u2f_counter,
                _("Forbidden field set in dry-run"))
  }

  CHECK_PARAM(!msg->has_word_count || msg->word_count == 12 ||
                  msg->word_count == 18 || msg->word_count == 24,
              _("Invalid word count"));

  recovery_init(msg->has_word_count ? msg->word_count : 12,
                msg->has_passphrase_protection && msg->passphrase_protection,
                msg->has_pin_protection && msg->pin_protection,
                msg->has_language ? msg->language : 0,
                msg->has_label ? msg->label : 0,
                msg->has_enforce_wordlist && msg->enforce_wordlist,
                msg->has_type ? msg->type : 0,
                msg->has_u2f_counter ? msg->u2f_counter : 0, dry_run);
}

void fsm_msgWordAck(const WordAck *msg) {
  CHECK_UNLOCKED

  recovery_word(msg->word);
}

void fsm_msgSetU2FCounter(const SetU2FCounter *msg) {
  CHECK_PIN

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Do you want to set"), _("the U2F counter?"), NULL, NULL,
                    NULL, NULL);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }
  config_setU2FCounter(msg->u2f_counter);
  fsm_sendSuccess(_("U2F counter set"));
  layoutHome();
}

void fsm_msgGetNextU2FCounter() {
  CHECK_PIN

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Do you want to"), _("increase and retrieve"),
                    _("the U2F counter?"), NULL, NULL, NULL);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }
  uint32_t counter = config_nextU2FCounter();

  RESP_INIT(NextU2FCounter);
  resp->u2f_counter = counter;
  msg_write(MessageType_MessageType_NextU2FCounter, resp);
  layoutHome();
}

static void progress_callback(uint32_t iter, uint32_t total) {
  layoutProgress(_("Please wait"), 1000 * iter / total);
}

void fsm_msgGetFirmwareHash(const GetFirmwareHash *msg) {
  RESP_INIT(FirmwareHash);
  layoutProgressSwipe(_("Please wait"), 0);
  if (memory_firmware_hash(msg->challenge.bytes, msg->challenge.size,
                           progress_callback, resp->hash.bytes) != 0) {
    fsm_sendFailure(FailureType_Failure_FirmwareError, NULL);
    return;
  }

  resp->hash.size = sizeof(resp->hash.bytes);
  msg_write(MessageType_MessageType_FirmwareHash, resp);
  layoutHome();
}

void fsm_msgSetBusy(const SetBusy *msg) {
  if (msg->has_expiry_ms) {
    system_millis_busy_deadline = timer_ms() + msg->expiry_ms;
  } else {
    system_millis_busy_deadline = 0;
  }
  fsm_sendSuccess(NULL);
  layoutHome();
  return;
}

void fsm_msgBixinReboot(const BixinReboot *msg) {
  (void)msg;

#if !EMULATOR
  if (sys_usbState() == false && battery_cap < 2) {
    layoutDialogCenterAdapterEx(
        &bmp_icon_warning, NULL, &bmp_bottom_right_confirm, NULL,
        _("Low Battery!Use cable or"), _("Charge to 25% before"),
        _("updating the bootloader"), NULL);
    while (1) {
      uint8_t key = keyScan();
      if (key == KEY_CONFIRM) {
        fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
        layoutHome();
        return;
      }
      if (sys_usbState() == true) {
        break;
      }
    }
  }
#endif

  layoutDialogCenterAdapter(&bmp_icon_warning, &bmp_bottom_left_close, NULL,
                            &bmp_bottom_right_arrow, NULL, NULL, NULL, NULL,
                            NULL, _("Do you want to restart"),
                            _("device in update mode?"), NULL);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }
  CHECK_PIN_UNCACHED
  fsm_sendSuccess(_("reboot start"));
  usbFlush(500);  // send response before reboot
#if !EMULATOR
  svc_reboot_to_bootloader();
#endif
}

void fsm_msgBixinMessageSE(const BixinMessageSE *msg) {
  bool request_restore = false;
  bool request_backup = false;
  RESP_INIT(BixinOutMessageSE);

  if (msg->inputmessage.bytes[0] == 0x00 &&
      msg->inputmessage.bytes[1] == 0xf9 &&
      msg->inputmessage.bytes[2] == 0x00) {
    CHECK_INITIALIZED
    if (config_hasPin()) {
      CHECK_PIN_UNCACHED
    } else if (!protectChangePin(false)) {
      layoutHome();
      return;
    }
    if (config_hasMnemonic()) {
      char mnemonic[MAX_MNEMONIC_LEN + 1] = {0};
      uint8_t entropy[64] = {0};
      config_getMnemonic(mnemonic, sizeof(mnemonic));
      entropy[0] = mnemonic_to_bits(mnemonic, entropy + 1) / 11;
      if (!config_stBackUpEntoryToSe(entropy, sizeof(entropy))) {
        fsm_sendFailure(FailureType_Failure_ProcessError,
                        _("seed import failed"));
        layoutHome();
        return;
      }
    }
    request_backup = true;
  } else if (msg->inputmessage.bytes[0] == 0x00 &&
             msg->inputmessage.bytes[1] == 0xf9 &&
             msg->inputmessage.bytes[2] == 0x01) {
    CHECK_NOT_INITIALIZED
    request_restore = true;
  }

  if (false == config_getMessageSE(
                   (BixinMessageSE_inputmessage_t *)(&msg->inputmessage),
                   (BixinOutMessageSE_outmessage_t *)(&resp->outmessage))) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage, NULL);
    layoutHome();
    return;
  }
  if (request_restore) {
    // restore to st
    if (resp->outmessage.bytes[0] == 0x55) {
      if (!protectChangePin(false)) {
        layoutHome();
        return;
      }
      uint8_t entropy[64] = {0};
      uint8_t len = sizeof(entropy);
      if (config_stRestoreEntoryFromSe(entropy, &len)) {
        if (len != 64) {
          fsm_sendFailure(FailureType_Failure_DataError,
                          _("Entory data error"));
        } else if (entropy[0] == 12 || entropy[0] == 18 || entropy[0] == 24) {
          const char *mnemonic =
              mnemonic_from_data(entropy + 1, entropy[0] * 4 / 3);
          if (config_setMnemonic(mnemonic, true)) {
          } else {
            fsm_sendFailure(FailureType_Failure_ProcessError,
                            _("Failed to store mnemonic"));
            layoutHome();
            return;
          }
        }
      }
    } else if (resp->outmessage.bytes[0] == 0xaa) {
      if (!protectChangePin(false)) {
        layoutHome();
        return;
      }
      if (!config_getWhetherUseSE()) {
        config_setWhetherUseSE(true);
      }
    } else {
      fsm_sendFailure(FailureType_Failure_DataError, _("restore data err"));
      layoutHome();
      return;
    }
  }
  resp->has_outmessage = true;
  if (request_backup) {
    if (msg_write(MessageType_MessageType_BixinOutMessageSE, resp)) {
      config_setNeedsBackup(false);
    }
  } else if (request_restore) {
    i2c_set_wait(false);
    msg_write(MessageType_MessageType_BixinOutMessageSE, resp);
  } else {
    msg_write(MessageType_MessageType_BixinOutMessageSE, resp);
  }
  layoutHome();
  return;
}

void fsm_msgBixinVerifyDeviceRequest(const BixinVerifyDeviceRequest *msg) {
  if (config_hasPin()) {
    CHECK_PIN
  }

  layoutDialogSwipe(NULL, _("Cancel"), _("Confirm"), _("SECURITY CHECK"), NULL,
                    _("Check this device with\nOneKey secure server?"), NULL,
                    NULL, NULL, NULL);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  RESP_INIT(BixinVerifyDeviceAck);
  resp->cert.size = 1024;
  resp->signature.size = 512;
  if (false == se_verify((uint8_t *)msg->data.bytes, msg->data.size, 1024,
                         resp->cert.bytes, &resp->cert.size,
                         resp->signature.bytes, &resp->signature.size)) {
    fsm_sendFailure(FailureType_Failure_UnexpectedMessage, NULL);
    layoutHome();
    return;
  }
  msg_write(MessageType_MessageType_BixinVerifyDeviceAck, resp);
  layoutHome();
  return;
}

void fsm_msgBixinLoadDevice(const BixinLoadDevice *msg) {
  //   CHECK_PIN

  CHECK_NOT_INITIALIZED

  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("OK"), NULL,
                    _("If import seed,"), _("device is used for"),
                    _("backup only"), _("know what you are"), _("doing!"),
                    NULL);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }
  if (!(msg->has_skip_checksum && msg->skip_checksum)) {
    if (!mnemonic_check(msg->mnemonics)) {
      fsm_sendFailure(FailureType_Failure_DataError,
                      _("Mnemonic with wrong checksum provided"));
      layoutHome();
      return;
    }
  }
  if (config_hasPin()) {
    CHECK_PIN
  } else if (!protectChangePin(false)) {
    layoutHome();
    return;
  }

  config_loadDevice_ex(msg);
  fsm_sendSuccess(_("Device loaded"));
  layoutHome();
}

void fsm_msgBixinBackupDevice(void) {
  if (!config_getMnemonicsImported()) {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    "device is not supported");
    layoutHome();
    return;
  }
  CHECK_PIN_UNCACHED

  RESP_INIT(BixinBackupDeviceAck);

  config_getMnemonic(resp->mnemonics, sizeof(resp->mnemonics));

  msg_write(MessageType_MessageType_BixinBackupDeviceAck, resp);
  layoutHome();
  return;
}
