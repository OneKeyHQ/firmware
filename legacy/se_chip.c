#include "se_chip.h"

#include "mi2c.h"
#include "rtt_log.h"

// mode is export seed
void se_get_seed(bool mode, const char *passphrase, uint8_t *seed) {
  rtt_log_print("SE gen seed");
  uint8_t cmd[1024];
  uint16_t resplen;
  int passphraselen = 0;
  uint8_t salt[256] = {0};
  if (passphrase) {
    passphraselen = strnlen(passphrase, 256);
    memcpy(salt, passphrase, passphraselen);
  }

  cmd[0] = mode;
  // salt LV
  cmd[1] = (passphraselen)&0xFF;
  cmd[2] = (passphraselen >> 8) & 0xFF;
  memcpy(cmd + 3, salt, passphraselen);
  MI2CDRV_Transmit(MI2C_CMD_WR_PIN, MNEMONIC_INDEX_TOSEED, cmd,
                   passphraselen + 3, seed, &resplen, MI2C_ENCRYPT,
                   SET_SESTORE_DATA);
  return;
}

bool se_ecdsa_get_pubkey(uint32_t *address, uint8_t count, uint8_t *pubkey) {
  rtt_log_print("SE get pubkey");
  uint8_t resp[256];
  uint16_t resp_len;
  if (MI2C_OK != MI2CDRV_Transmit(MI2C_CMD_ECC_EDDSA, EDDSA_INDEX_CHILDKEY,
                                  (uint8_t *)address, count * 4, resp,
                                  &resp_len, MI2C_PLAIN, SET_SESTORE_DATA)) {
    return false;
  }
  memcpy(pubkey, resp + 1 + 4 + 32 + 33, 33);
  return true;
}

bool se_set_value(const uint16_t key, const void *val_dest, uint16_t len) {
  rtt_log_print("SE set key=%x", key);
  uint8_t flag = key >> 8;
  if (MI2C_OK != MI2CDRV_Transmit(MI2C_CMD_WR_PIN, (key & 0xFF),
                                  (uint8_t *)val_dest, len, NULL, 0,
                                  (flag & MI2C_PLAIN), SET_SESTORE_DATA)) {
    rtt_log_print("SE set key failed");
    return false;
  }
  rtt_log_print("SE set key suucess");
  return true;
}

bool se_get_value(const uint16_t key, void *val_dest, uint16_t max_len,
                  uint16_t *len) {
  rtt_log_print("SE get key=%x value", key);
  uint8_t flag = key >> 8;
  if (MI2C_OK != MI2CDRV_Transmit(MI2C_CMD_WR_PIN, (key & 0xFF), NULL, 0,
                                  val_dest, len, (flag & MI2C_PLAIN),
                                  GET_SESTORE_DATA)) {
    rtt_log_print("SE get key failed");
    return false;
  }
  if (*len > max_len) {
    return false;
  }
  rtt_log_print("SE get key suucess");
  return true;
}

bool se_delete_key(const uint16_t key) {
  rtt_log_print("SE delete key=%x", key);
  if (MI2C_OK != MI2CDRV_Transmit(MI2C_CMD_WR_PIN, (key & 0xFF), NULL, 0, NULL,
                                  0, MI2C_PLAIN, DELETE_SESTORE_DATA)) {
    rtt_log_print("SE delelte key failed");
    return false;
  }
  rtt_log_print("SE delelte key suucess");
  return true;
}

void se_reset_storage(const uint16_t key) {
  rtt_log_print("SE reset storage");
  if (MI2C_OK == MI2CDRV_Transmit(MI2C_CMD_WR_PIN, (key & 0xFF), NULL, 0, NULL,
                                  NULL, MI2C_ENCRYPT, SET_SESTORE_DATA)) {
    rtt_log_print("SE reset suucess");
  } else
    rtt_log_print("SE reset failed");
}

bool se_get_sn(void *val_dest, uint16_t max_len, uint16_t *len) {
  uint8_t ucSnCmd[5] = {0x00, 0xf5, 0x01, 0x00, 0x10};
  rtt_log_print("SE get sn");
  if (MI2C_OK !=
      MI2CDRV_TransmitPlain(ucSnCmd, sizeof(ucSnCmd), val_dest, len)) {
    rtt_log_print("SE get sn failed");
    return false;
  }
  if (*len > max_len) {
    return false;
  }
  rtt_log_print("SE get sn sucess");
  return true;
}

bool se_get_version(void *val_dest, uint16_t max_len, uint16_t *len) {
  uint8_t ucVerCmd[5] = {0x00, 0xf7, 0x00, 00, 0x02};
  rtt_log_print("SE get version");
  if (MI2C_OK !=
      MI2CDRV_TransmitPlain(ucVerCmd, sizeof(ucVerCmd), val_dest, len)) {
    rtt_log_print("SE get version failed");
    return false;
  }
  if (*len > max_len) {
    return false;
  }
  rtt_log_print("SE get version sucess");
  return true;
}

bool se_verify(void *message, uint16_t message_len, void *val_dest,
               uint16_t max_len, uint16_t *len) {
  uint8_t ucSignCmd[37] = {0x00, 0x72, 0x00, 00, 0x20};
  rtt_log_print("SE device sign");

  if (message_len > 0x20) {
    return false;
  }
  memcpy(ucSignCmd + 5, message, message_len);
  if (MI2C_OK !=
      MI2CDRV_TransmitPlain(ucSignCmd, sizeof(ucSignCmd), val_dest, len)) {
    rtt_log_print("SE device fail");
    return false;
  }
  if (*len > max_len) {
    return false;
  }
  rtt_log_print("SE device sign sucess");
  return true;
}

bool se_backup(void *val_dest, uint16_t *len) {
  rtt_log_print("SE seed backup");
  if (MI2C_OK != MI2CDRV_Transmit(MI2C_CMD_WR_PIN, 0x12, NULL, 0,
                                  (uint8_t *)val_dest, len, MI2C_PLAIN,
                                  GET_SESTORE_DATA)) {
    rtt_log_print("SE seed backup fail");
    return false;
  }
  rtt_log_print("SE seed backup success");
  return true;
}
bool se_restore(void *val_src, uint16_t src_len) {
  rtt_log_print("SE seed restore");
  if (MI2C_OK != MI2CDRV_Transmit(MI2C_CMD_WR_PIN, 0x12, val_src, src_len, NULL,
                                  NULL, MI2C_PLAIN, DELETE_SESTORE_DATA)) {
    rtt_log_print("SE seed restore failed");
    return false;
  }
  rtt_log_print("SE get sn sucess");
  return true;
}

bool se_device_init(uint8_t mode, const char *passphrase) {
  rtt_log_print("SE init");
  uint8_t cmd[1024];
  uint16_t passphraselen = 0;

  if (NULL != passphrase) {
    passphraselen = strnlen(passphrase, 256);
  }
  cmd[0] = mode;
  // salt LV
  cmd[1] = passphraselen & 0xFF;
  cmd[2] = (passphraselen >> 8) & 0xFF;
  memcpy(cmd + 3, passphrase, passphraselen);
  if (MI2C_OK != MI2CDRV_Transmit(MI2C_CMD_WR_PIN, 0x12, cmd, passphraselen + 3,
                                  NULL, NULL, MI2C_ENCRYPT, DEVICEINIT_DATA)) {
    return false;
  }
  return true;
}
