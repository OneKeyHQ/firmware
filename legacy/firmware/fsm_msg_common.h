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
#include "menu_list.h"
#include "mi2c.h"
#include "se_hal.h"
#include "storage.h"

#if ONEKEY_MINI
#include "device.h"
#include "flash_enc.h"
#include "font_ex.h"
#include "nft.h"
#include "w25qxx.h"
#endif

extern char bootloader_version[8];

bool get_features(Features *resp) {
#if ONEKEY_MINI
  if (device_is_factory_mode()) {
    uint32_t cert_len = 0;
    resp->has_vendor = true;
    strlcpy(resp->vendor, "onekey.so", sizeof(resp->vendor));
    resp->has_model = true;
    strlcpy(resp->model, "factory", sizeof(resp->model));
    resp->major_version = VERSION_MAJOR;
    resp->minor_version = VERSION_MINOR;
    resp->patch_version = VERSION_PATCH;
    resp->has_initstates = true;
    resp->initstates = 0;
    resp->initstates |= device_serial_set() ? 1 : 0;
    resp->initstates |= font_imported() ? (1 << 1) : 0;
    resp->initstates |= se_get_certificate_len(&cert_len) ? (1 << 2) : 0;
    resp->initstates |= device_cpu_firmware_set() ? (1 << 3) : 0;

  } else
#endif
  {
    resp->has_vendor = true;
    strlcpy(resp->vendor, "onekey.so", sizeof(resp->vendor));
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

#if !ONEKEY_MINI
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
#endif

#if !EMULATOR
    char *se_version = NULL;
    char *serial = NULL;
    resp->has_se_enable = true;
    resp->se_enable = config_getWhetherUseSE();
    se_version = se_get_version();
    if (se_version) {
      resp->has_se_ver = true;
      memcpy(resp->se_ver, se_version, strlen(se_version));
    }

    if (se_get_sn(&serial)) {
      resp->has_onekey_serial = true;
      strlcpy(resp->onekey_serial, serial, sizeof(resp->onekey_serial));
    }
#endif

    resp->has_backup_only = true;
    resp->backup_only = config_getMnemonicsImported();

    resp->has_onekey_version = true;
    strlcpy(resp->onekey_version, ONEKEY_VERSION, sizeof(resp->onekey_version));

#ifdef BUILD_ID
    resp->has_build_id = true;
    strlcpy(resp->build_id, BUILD_ID, sizeof(resp->build_id));
#endif
#if !EMULATOR
    resp->has_bootloader_version = true;
    strlcpy(resp->bootloader_version, bootloader_version,
            sizeof(resp->bootloader_version));
#endif

#if ONEKEY_MINI
    char *dev_serial;
    if (device_get_serial(&dev_serial)) {
      resp->has_serial_no = true;
      strlcpy(resp->serial_no, dev_serial, sizeof(resp->serial_no));
    }
    resp->has_spi_flash = true;
    strlcpy(resp->spi_flash, w25qxx_get_desc(), sizeof(resp->spi_flash));

    if (device_get_NFT_voucher(resp->NFT_voucher.bytes)) {
      resp->has_NFT_voucher = true;
      resp->NFT_voucher.size = 32;
    }
#endif
  }
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

  return resp;
}

void fsm_msgInitialize(const Initialize *msg) {
  recovery_abort();
  signing_abort();

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
    // Note that if storage is locked, then config_hasWipeCode() returns
    // false.
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
  if (g_bIsBixinAPP) {
    CHECK_PIN_UNCACHED
  }
  (void)msg;
  layoutDialogSwipe(&bmp_icon_question, _("Cancel"), _("Confirm"), NULL,
                    _("Do you really want to"), _("wipe the device?"), NULL,
                    _("All data will be lost."), NULL, NULL);

  if (!protectButton(ButtonRequestType_ButtonRequest_WipeDevice, false)) {
    i2c_set_wait(false);
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }
  config_wipe();
  // the following does not work on Mac anyway :-/ Linux/Windows are fine, so
  // it is not needed usbReconnect(); // force re-enumeration because of the
  // serial number change
  i2c_set_wait(false);
  fsm_sendSuccess(_("Device wiped"));
  layoutHome();
}

void fsm_msgGetEntropy(const GetEntropy *msg) {
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
  recovery_abort();
  signing_abort();
#if !BITCOIN_ONLY
  ethereum_signing_abort();
#endif
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
    config_setAutoLockDelayMs(msg->auto_lock_delay_ms);
  }
#if !ONEKEY_MINI
  if (msg->has_use_ble) {
    config_setBleTrans(msg->use_ble);
  }
#endif
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

void fsm_msgWordAck(const WordAck *msg) { recovery_word(msg->word); }

void fsm_msgSetU2FCounter(const SetU2FCounter *msg) {
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

void fsm_msgBixinReboot(const BixinReboot *msg) {
  (void)msg;
  layoutDialogSwipeCenterAdapter(
      NULL, &bmp_btn_cancel, _("Cancel"), &bmp_btn_confirm, _("Confirm"), NULL,
      NULL, NULL, NULL, _("Are you sure to update?"), NULL, NULL);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }
  CHECK_PIN_UNCACHED
  fsm_sendSuccess(_("reboot start"));
  usbPoll();  // send response before reboot
#if !EMULATOR
  sys_backtoboot();
#endif
}

void fsm_msgBixinMessageSE(const BixinMessageSE *msg) {
  (void)msg;
#if ONEKEY_MINI
  fsm_sendFailure(FailureType_Failure_ProcessError,
                  "This operation is not supported");
  layoutHome();
  return;
#else
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
#endif
  layoutHome();
  return;
}

void fsm_msgBixinVerifyDeviceRequest(const BixinVerifyDeviceRequest *msg) {
  (void)msg;
#if ONEKEY_MINI
  fsm_sendFailure(FailureType_Failure_ProcessError,
                  "This operation is not supported");
  layoutHome();
  return;
#else
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
#endif
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

void fsm_msgDeviceInfoSettings(const DeviceInfoSettings *msg) {
  (void)msg;
#if ONEKEY_MINI
  if (msg->has_serial_no) {
    if (!device_set_serial((char *)msg->serial_no)) {
      fsm_sendFailure(FailureType_Failure_ProcessError, _("Set serial failed"));
    }
  }
  if ((msg->has_cpu_info | msg->has_pre_firmware) &&
      !(msg->has_cpu_info & msg->has_pre_firmware)) {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Parameter error"));
  }

  if (device_set_cpu_firmware((char *)msg->cpu_info,
                              (char *)msg->pre_firmware)) {
    fsm_sendSuccess(_("Settings applied"));
  } else {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Set cpu/firmware failed"));
  }
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  return;
}

void fsm_msgGetDeviceInfo(const GetDeviceInfo *msg) {
  (void)msg;
#if ONEKEY_MINI
  RESP_INIT(DeviceInfo);
  char *serial;
  if (device_get_serial(&serial)) {
    resp->has_serial_no = true;
    strlcpy(resp->serial_no, serial, sizeof(resp->serial_no));
  }

  if (device_get_NFT_voucher(resp->NFT_voucher.bytes)) {
    resp->has_NFT_voucher = true;
    resp->NFT_voucher.size = 32;
  }

  char *cpu, *firmware;
  if (device_get_cpu_firmware(&cpu, &firmware)) {
    resp->has_cpu_info = true;
    resp->has_pre_firmware = true;
    strlcpy(resp->cpu_info, cpu, sizeof(resp->cpu_info));
    strlcpy(resp->pre_firmware, firmware, sizeof(resp->pre_firmware));
  }
  msg_write(MessageType_MessageType_DeviceInfo, resp);
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  return;
}

void fsm_msgReadSEPublicKey(const ReadSEPublicKey *msg) {
  (void)msg;
#if ONEKEY_MINI
  RESP_INIT(SEPublicKey);
  if (se_get_pubkey(resp->public_key.bytes)) {
    resp->public_key.size = 64;
    msg_write(MessageType_MessageType_SEPublicKey, resp);
  } else {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Get SE pubkey Failed"));
  }
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  return;
}

void fsm_msgWriteSEPublicCert(const WriteSEPublicCert *msg) {
  (void)msg;
#if ONEKEY_MINI
  if (se_write_certificate(msg->public_cert.bytes, msg->public_cert.size)) {
    fsm_sendSuccess(_("Settings applied"));
  } else {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Settings failed"));
  }
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  return;
}

void fsm_msgReadSEPublicCert(const ReadSEPublicCert *msg) {
  (void)msg;
#if ONEKEY_MINI
  uint32_t cert_len = 0;

  RESP_INIT(SEPublicCert);
  if (se_read_certificate(resp->public_cert.bytes, &cert_len)) {
    resp->public_cert.size = cert_len;
    msg_write(MessageType_MessageType_SEPublicCert, resp);
  } else {
    fsm_sendFailure(FailureType_Failure_ProcessError,
                    _("Get certificate failed"));
  }
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  return;
}

void fsm_msgSpiFlashWrite(const SpiFlashWrite *msg) {
  (void)msg;
#if ONEKEY_MINI
  if (flash_write_enc((uint8_t *)msg->data.bytes, msg->address,
                      msg->data.size)) {
    fsm_sendSuccess(_("Write success"));
  } else {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Write failed"));
  }
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  return;
}

void fsm_msgSpiFlashRead(const SpiFlashRead *msg) {
  (void)msg;
#if ONEKEY_MINI
  RESP_INIT(SpiFlashData);

  if (flash_read_enc(resp->data.bytes, msg->address, msg->len)) {
    resp->data.size = msg->len;
    msg_write(MessageType_MessageType_SpiFlashData, resp);
  } else {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Read failed"));
  }
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  return;
}

void fsm_msgSESignMessage(const SESignMessage *msg) {
  (void)msg;
#if ONEKEY_MINI
  if (!device_is_factory_mode()) {
    CHECK_PIN
  }

  layoutDialogSwipeCenterAdapterEx(
      NULL, &bmp_button_back, _("Cancel"), &bmp_button_forward, _("Confirm"),
      NULL, true, NULL, NULL, NULL, NULL,
      _("Check this device with\nOneKey secure server?"), NULL, NULL, NULL,
      NULL, NULL, NULL, NULL);
  if (!protectButton(ButtonRequestType_ButtonRequest_ProtectCall, false)) {
    fsm_sendFailure(FailureType_Failure_ActionCancelled, NULL);
    layoutHome();
    return;
  }

  RESP_INIT(SEMessageSignature);

  if (se_sign_message((uint8_t *)msg->message.bytes, msg->message.size,
                      resp->signature.bytes)) {
    resp->signature.size = 64;
    msg_write(MessageType_MessageType_SEMessageSignature, resp);
  } else {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("SE sign failed"));
  }
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  layoutHome();
  return;
}

void fsm_msgNFTWriteInfo(const NFTWriteInfo *msg) {
  (void)msg;
#if ONEKEY_MINI
  NFTInformation nft_info;

  memzero(&nft_info, sizeof(nft_info));

  nft_info.index = msg->index;
  nft_info.width = msg->width;
  nft_info.height = msg->height;

  if (msg->has_name_zh) {
    strlcpy(nft_info.name_zh, msg->name_zh, sizeof(nft_info.name_zh));
  }

  if (msg->has_name_en) {
    strlcpy(nft_info.name_en, msg->name_en, sizeof(nft_info.name_en));
  }

  if (nft_add_info(&nft_info)) {
    fsm_sendSuccess(_("Write success"));
  } else {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Write failed"));
  }
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  return;
}

void fsm_msgNFTWriteData(const NFTWriteData *msg) {
  (void)msg;
#if ONEKEY_MINI
  if (nft_add_data(msg->index, (uint8_t *)msg->data.bytes, msg->offset,
                   msg->data.size)) {
    fsm_sendSuccess(_("Write success"));
  } else {
    fsm_sendFailure(FailureType_Failure_ProcessError, _("Write failed"));
  }
#else
  fsm_sendFailure(FailureType_Failure_UnexpectedMessage, _("Unknown message"));
#endif
  return;
}
