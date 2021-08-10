#include "device.h"
#include <libopencm3/stm32/desig.h>
#include "memory.h"
#include "otp.h"
#include "rng.h"
#include "sha2.h"
#include "util.h"

static char device_serial_obj[32];
static char *device_serial = device_serial_obj;
static DeviceConfig device_config_obj;
static DeviceConfig *device_config = &device_config_obj;
static bool serial_set = false;
static bool factory_mode = false;
static uint8_t st_id[12] = {0};
static uint8_t random_key[32];

void device_init(void) {
  desig_get_unique_id((uint32_t *)st_id);

  if (flash_otp_is_locked(FLASH_OTP_RANDOM_KEY)) {
    memcpy(&device_config_obj,
           FLASH_PTR(FLASH_OTP_BASE +
                     FLASH_OTP_FACTORY_TEST * FLASH_OTP_BLOCK_SIZE),
           sizeof(DeviceConfig));
  }

  if (!flash_otp_is_locked(FLASH_OTP_RANDOM_KEY)) {
    uint8_t entropy[FLASH_OTP_BLOCK_SIZE] = {0};
    uint32_t r;
    for (int i = 0; i < FLASH_OTP_BLOCK_SIZE; i += 4) {
      r = random32();
      memcpy(entropy + i, (uint8_t *)&r, 4);
    }
    flash_otp_write(FLASH_OTP_RANDOM_KEY, 0, entropy, FLASH_OTP_BLOCK_SIZE);
    flash_otp_lock(FLASH_OTP_RANDOM_KEY);
  }
  flash_otp_read(FLASH_OTP_RANDOM_KEY, 0, random_key, FLASH_OTP_BLOCK_SIZE);

  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    serial_set = false;
    return;
  }
  strlcpy(
      device_serial_obj,
      (char *)(FLASH_OTP_BASE + FLASH_OTP_DEVICE_SERIAL * FLASH_OTP_BLOCK_SIZE),
      sizeof(device_serial_obj));
  serial_set = true;
  return;
}

bool device_serial_set(void) { return serial_set; }

void device_set_factory_mode(bool mode) { factory_mode = mode; }

bool device_is_factory_mode(void) { return factory_mode; }

bool device_set_info(char *dev_serial) {
  uint8_t buffer[FLASH_OTP_BLOCK_SIZE] = {0};

  if (serial_set) {
    return false;
  }
  // check serial
  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    if (check_all_ones(FLASH_PTR(FLASH_OTP_BASE + FLASH_OTP_DEVICE_SERIAL *
                                                      FLASH_OTP_BLOCK_SIZE),
                       FLASH_OTP_BLOCK_SIZE)) {
      strlcpy((char *)buffer, dev_serial, sizeof(buffer));
      flash_otp_write(FLASH_OTP_DEVICE_SERIAL, 0, buffer, FLASH_OTP_BLOCK_SIZE);
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
  *serial = device_serial;
  return true;
}

char *device_get_se_config_version(void) {
  if (check_all_ones(device_config->atca_config_verson,
                     sizeof(device_config->atca_config_verson))) {
    return "0.0.1";
  } else {
    return device_config->atca_config_verson;
  }
}

bool device_get_NFT_voucher(uint8_t voucher[32]) {
  if (!flash_otp_is_locked(FLASH_OTP_RANDOM_KEY)) {
    return false;
  }

  SHA256_CTX ctx = {0};

  sha256_Init(&ctx);
  sha256_Update(&ctx, st_id, sizeof(st_id));
  sha256_Update(&ctx, random_key, sizeof(random_key));
  sha256_Final(&ctx, voucher);
  return true;
}
