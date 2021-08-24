#include "device.h"
#include <libopencm3/stm32/desig.h>
#include "common.h"
#include "memory.h"
#include "otp.h"
#include "rng.h"
#include "sha2.h"
#include "util.h"

static DeviceInfomation dev_info = {0};
static bool serial_set = false;
static bool factory_mode = false;

static bool is_valid_ascii(const uint8_t *data, uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
    if (data[i] == 0) {
      break;
    }
    if (data[i] < ' ' || data[i] > '~') {
      return false;
    }
  }
  return true;
}

void device_set_factory_mode(bool mode) { factory_mode = mode; }

bool device_is_factory_mode(void) { return factory_mode; }

void device_init(void) {
  desig_get_unique_id((uint32_t *)dev_info.st_id);

  if (flash_otp_is_locked(FLASH_OTP_FACTORY_TEST)) {
    strlcpy(dev_info.se_config,
            (char *)(FLASH_OTP_BASE +
                     FLASH_OTP_FACTORY_TEST * FLASH_OTP_BLOCK_SIZE),
            sizeof(dev_info.se_config));
  }

  if (!flash_otp_is_locked(FLASH_OTP_RANDOM_KEY)) {
    uint8_t entropy[FLASH_OTP_BLOCK_SIZE] = {0};
    uint32_t r;
    for (int i = 0; i < FLASH_OTP_BLOCK_SIZE; i += 4) {
      r = random32();
      memcpy(entropy + i, (uint8_t *)&r, 4);
    }
    ensure_ex(flash_otp_write_safe(FLASH_OTP_RANDOM_KEY, 0, entropy,
                                   FLASH_OTP_BLOCK_SIZE),
              true, NULL);
    flash_otp_lock(FLASH_OTP_RANDOM_KEY);
  }
  dev_info.random_key_init = true;
  flash_otp_read(FLASH_OTP_RANDOM_KEY, 0, dev_info.random_key,
                 FLASH_OTP_BLOCK_SIZE);

  if (flash_otp_is_locked(FLASH_OTP_CPU_FIRMWARE_INFO)) {
    strlcpy(dev_info.cpu_info,
            (char *)(FLASH_OTP_BASE +
                     FLASH_OTP_CPU_FIRMWARE_INFO * FLASH_OTP_BLOCK_SIZE),
            sizeof(dev_info.cpu_info));
    strlcpy(dev_info.pre_firmware,
            (char *)(FLASH_OTP_BASE +
                     FLASH_OTP_CPU_FIRMWARE_INFO * FLASH_OTP_BLOCK_SIZE +
                     FLASH_OTP_BLOCK_SIZE / 2),
            sizeof(dev_info.pre_firmware));
  }

  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    serial_set = false;
    return;
  }

  strlcpy(
      dev_info.serial,
      (char *)(FLASH_OTP_BASE + FLASH_OTP_DEVICE_SERIAL * FLASH_OTP_BLOCK_SIZE),
      sizeof(dev_info.serial));

  if (is_valid_ascii((uint8_t *)dev_info.serial, FLASH_OTP_BLOCK_SIZE)) {
    serial_set = true;
  }

  return;
}

bool device_serial_set(void) { return serial_set; }

bool device_set_serial(char *dev_serial) {
  uint8_t buffer[FLASH_OTP_BLOCK_SIZE] = {0};

  if (serial_set) {
    return false;
  }

  if (!is_valid_ascii((uint8_t *)dev_serial, FLASH_OTP_BLOCK_SIZE - 1)) {
    return false;
  }

  // check serial
  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    if (check_all_ones(FLASH_PTR(FLASH_OTP_BASE + FLASH_OTP_DEVICE_SERIAL *
                                                      FLASH_OTP_BLOCK_SIZE),
                       FLASH_OTP_BLOCK_SIZE)) {
      strlcpy((char *)buffer, dev_serial, sizeof(buffer));
      ensure_ex(flash_otp_write_safe(FLASH_OTP_DEVICE_SERIAL, 0, buffer,
                                     FLASH_OTP_BLOCK_SIZE),
                true, NULL);
      flash_otp_lock(FLASH_OTP_DEVICE_SERIAL);
      device_init();
      return true;
    }
  }
  return false;
}

bool device_get_serial(char **serial) {
  if (!serial_set) {
    return false;
  }
  *serial = dev_info.serial;
  return true;
}

bool device_cpu_firmware_set(void) {
  if ((0 < strlen(dev_info.cpu_info) &&
       strlen(dev_info.cpu_info) < FLASH_OTP_BLOCK_SIZE / 2) &&
      (0 < strlen(dev_info.pre_firmware) &&
       strlen(dev_info.pre_firmware) < FLASH_OTP_BLOCK_SIZE / 2)) {
    return true;
  }
  return false;
}

bool device_set_cpu_firmware(char *cpu_info, char *firmware_ver) {
  uint8_t buffer[FLASH_OTP_BLOCK_SIZE] = {0};

  // check serial
  if (!flash_otp_is_locked(FLASH_OTP_CPU_FIRMWARE_INFO)) {
    if (check_all_ones(FLASH_PTR(FLASH_OTP_BASE + FLASH_OTP_CPU_FIRMWARE_INFO *
                                                      FLASH_OTP_BLOCK_SIZE),
                       FLASH_OTP_BLOCK_SIZE)) {
      strlcpy((char *)buffer, cpu_info, sizeof(buffer) / 2);
      strlcpy((char *)buffer + FLASH_OTP_BLOCK_SIZE / 2, firmware_ver,
              sizeof(buffer) / 2);
      ensure_ex(flash_otp_write_safe(FLASH_OTP_CPU_FIRMWARE_INFO, 0, buffer,
                                     FLASH_OTP_BLOCK_SIZE),
                true, NULL);
      flash_otp_lock(FLASH_OTP_CPU_FIRMWARE_INFO);
      device_init();
      return true;
    }
  }
  return false;
}

bool device_get_cpu_firmware(char **cpu_info, char **firmware_ver) {
  if (device_cpu_firmware_set()) {
    *cpu_info = dev_info.cpu_info;
    *firmware_ver = dev_info.pre_firmware;
  }
  return false;
}

char *device_get_se_config_version(void) {
  if (check_all_ones(dev_info.se_config, sizeof(dev_info.se_config))) {
    return "0.0.1";
  } else {
    return dev_info.se_config;
  }
}

bool device_get_NFT_voucher(uint8_t voucher[32]) {
  if (!dev_info.random_key_init) {
    return false;
  }

  SHA256_CTX ctx = {0};

  sha256_Init(&ctx);
  sha256_Update(&ctx, dev_info.st_id, sizeof(dev_info.st_id));
  sha256_Update(&ctx, dev_info.random_key, sizeof(dev_info.random_key));
  sha256_Final(&ctx, voucher);
  return true;
}
