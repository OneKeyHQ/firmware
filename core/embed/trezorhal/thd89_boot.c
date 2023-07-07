#include "thd89_boot.h"
#include "common.h"
#include "rand.h"
#include "thd89.h"

bool se_get_firmware_version(uint8_t *version) {
  uint8_t cmd[5] = {0x00, 0xf7, 0x00, 00, 0x02};
  uint16_t ver_len = 0;

  if (!thd89_transmit(cmd, sizeof(cmd), version, &ver_len)) {
    return false;
  }

  return true;
}

bool se_get_state(uint8_t *state) {
  uint8_t cmd[5] = {0x80, 0xfc, 0x00, 00, 0x00};
  uint16_t resp_len = 0;

  if (!thd89_transmit(cmd, sizeof(cmd), state, &resp_len)) {
    return false;
  }

  if ((resp_len != 0x01) ||
      ((state[0] != 0x00) && (state[0] != 0x55) && (state[0] != 0x33))) {
    return false;
  }
  return true;
}

bool se_back_to_boot(void) {
  uint8_t cmd[5] = {0x80, 0xfc, 0x00, 0xff, 0x00};
  uint16_t resp_len = 0;
  if (!thd89_transmit(cmd, sizeof(cmd), NULL, &resp_len)) {
    return false;
  }
  return true;
}

bool se_active_app(void) {
  uint8_t cmd[5] = {0x80, 0xfc, 0x00, 0x04, 0x00};
  uint16_t resp_len = 0;
  if (!thd89_transmit(cmd, sizeof(cmd), NULL, &resp_len)) {
    return false;
  }
  return true;
}

bool se_update(uint8_t step, uint8_t *data, uint16_t data_len) {
  uint8_t cmd[520];
  uint16_t cmd_len = 5, resp_len = 0;
  cmd[0] = 0x80;
  cmd[1] = 0xFC;
  cmd[2] = 0x00;
  cmd[3] = step;
  cmd[4] = 0x00;

  // send steps
  if (0x01 == step) {
    if (data_len != 0x60) {
      return false;
    }
    cmd[4] = 0x60;

    memcpy(cmd + 5, data, data_len);
    cmd_len += 0x60;

  } else if ((0x02 == step) || (0x05 == step)) {
    if (data_len != 512) {
      return false;
    }
    cmd[5] = 0x02;
    cmd[6] = 0x00;
    memcpy(cmd + 7, data, 512);
    cmd_len += 2 + 512;
  }
  if (!thd89_transmit(cmd, cmd_len, NULL, &resp_len)) {
    return false;
  }
  return true;
}

bool se_back_to_boot_progress(void) {
  uint8_t state;
  if (!se_get_state(&state)) {
    return false;
  }
  if (state == THD89_STATE_APP) {
    se_back_to_boot();
    hal_delay(1000);
    se_get_state(&state);
  }
  if (state != THD89_STATE_BOOT) {
    return false;
  }
  return true;
}

bool se_verify_firmware(uint8_t hash[32], uint8_t sig[64]) {
  uint8_t verify[96];
  memcpy(verify, hash, 32);
  memcpy(verify + 32, sig, 64);

  if (!se_update(1, verify, 0x60)) {
    return false;
  }

  return true;
}

bool se_update_firmware(uint8_t *data, uint32_t data_len,
                        void (*ui_callback)(int progress)) {
  uint32_t offset_len = 0;
  while (data_len) {
    uint32_t packet_len = data_len > 512 ? 512 : data_len;

    if (!se_update(2, data + offset_len, packet_len)) {
      return false;
    }
    data_len -= packet_len;
    offset_len += packet_len;
    if (ui_callback) {
      ui_callback(1000 * offset_len / (offset_len + data_len));
    }
  }

  return true;
}

bool se_active_app_progress(void) {
  if (!se_update(3, NULL, 0)) {
    return false;
  }
  hal_delay(1000);

  uint8_t state;
  if (!se_get_state(&state)) {
    return false;
  }
  if (state != THD89_STATE_NOT_ACTIVATED) {
    return false;
  }
  if (!se_active_app()) {
    return false;
  }
  if (!se_get_state(&state)) {
    return false;
  }
  if (state != THD89_STATE_APP) {
    return false;
  }
  return true;
}

#if 0

void se_reset_storage(void) {
  uint8_t cmd[37] = {0x80, 0xe1, 0x1b, 0x00, 0x20};
  uint16_t resp_len = 0;
  random_buffer(cmd + 5, 32);
  thd89_transmit(cmd, sizeof(cmd), NULL, &resp_len);
}

char *se_get_version(void) {
  uint8_t get_ver[5] = {0x00, 0xf7, 0x00, 00, 0x02};
  uint8_t ver[2] = {0};
  uint16_t ver_len = sizeof(ver);
  static char ver_char[9] = {0};
  int i = 0;

  if (!thd89_transmit(get_ver, sizeof(get_ver), ver, &ver_len)) {
    return NULL;
  }

  ver_char[i++] = (ver[0] >> 4) + '0';
  ver_char[i++] = '.';
  ver_char[i++] = (ver[0] & 0x0f) + '0';
  ver_char[i++] = '.';
  ver_char[i++] = (ver[1] >> 4) + '0';
  ver_char[i++] = (ver[1] & 0x0f) + '0';

  return ver_char;
}

bool se_get_pubkey(uint8_t pubkey[64]) {
  uint8_t get_pubkey[5] = {0x00, 0x73, 0x00, 00, 0x40};
  uint16_t pubkey_len = 0;
  if (!thd89_transmit(get_pubkey, sizeof(get_pubkey), pubkey, &pubkey_len)) {
    return false;
  }
  if (pubkey_len != 64) {
    return false;
  }
  return true;
}

bool se_read_certificate(uint8_t *cert, uint32_t *cert_len) {
  uint8_t get_cert[5] = {0x00, 0xf8, 0x01, 0x00, 0x00};
  if (!thd89_transmit(get_cert, sizeof(get_cert), cert, (uint16_t *)cert_len)) {
    return false;
  }
  return true;
}

bool se_write_certificate(const uint8_t *cert, uint32_t cert_len) {
  return true;
}

bool se_sign_message(uint8_t *msg, uint32_t msg_len, uint8_t *signature) {
  uint8_t sign[37] = {0x00, 0x72, 0x00, 00, 0x20};
  uint16_t signature_len;

  if (msg_len != 0x20) {
    return false;
  }
  memcpy(sign + 5, msg, msg_len);
  return sectrue ==
                 thd89_transmit(sign, sizeof(sign), signature, &signature_len)
             ? true
             : false;
}
#endif
