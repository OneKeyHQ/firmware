#include STM32_HAL_H

#include "device.h"
#include "atca_api.h"
#include "atca_command.h"
#include "atca_hal.h"
#include "common.h"
#include "display.h"
#include "emmc.h"
#include "flash.h"
#include "mini_printf.h"
#include "qspi_flash.h"
#include "rand.h"
#include "sha2.h"
#include "sys.h"
#include "touch.h"

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

void device_para_init(void) {
  dev_info.st_id[0] = HAL_GetUIDw0();
  dev_info.st_id[1] = HAL_GetUIDw1();
  dev_info.st_id[2] = HAL_GetUIDw2();

  if (flash_otp_is_locked(FLASH_OTP_FACTORY_TEST)) {
    strlcpy(dev_info.se_config,
            (char *)flash_otp_data->flash_otp[FLASH_OTP_FACTORY_TEST],
            sizeof(dev_info.se_config));
  }

  if (!flash_otp_is_locked(FLASH_OTP_RANDOM_KEY)) {
    uint8_t entropy[FLASH_OTP_BLOCK_SIZE] = {0};
    random_buffer(entropy, FLASH_OTP_BLOCK_SIZE);
    ensure(
        flash_otp_write(FLASH_OTP_RANDOM_KEY, 0, entropy, FLASH_OTP_BLOCK_SIZE),
        NULL);
    ensure(flash_otp_lock(FLASH_OTP_RANDOM_KEY), NULL);
  }
  dev_info.random_key_init = true;
  ensure(flash_otp_read(FLASH_OTP_RANDOM_KEY, 0, dev_info.random_key,
                        FLASH_OTP_BLOCK_SIZE),
         NULL);

  if (flash_otp_is_locked(FLASH_OTP_CPU_FIRMWARE_INFO)) {
    strlcpy(dev_info.cpu_info,
            (char *)flash_otp_data->flash_otp[FLASH_OTP_CPU_FIRMWARE_INFO],
            sizeof(dev_info.cpu_info));
    strlcpy(dev_info.pre_firmware,
            (char *)(flash_otp_data->flash_otp[FLASH_OTP_CPU_FIRMWARE_INFO] +
                     FLASH_OTP_BLOCK_SIZE / 2),
            sizeof(dev_info.pre_firmware));
  }

  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    serial_set = false;
    return;
  }

  strlcpy(dev_info.serial,
          (char *)flash_otp_data->flash_otp[FLASH_OTP_DEVICE_SERIAL],
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
    if (check_all_ones(flash_otp_data->flash_otp[FLASH_OTP_DEVICE_SERIAL],
                       FLASH_OTP_BLOCK_SIZE)) {
      strlcpy((char *)buffer, dev_serial, sizeof(buffer));
      ensure(flash_otp_write(FLASH_OTP_DEVICE_SERIAL, 0, buffer,
                             FLASH_OTP_BLOCK_SIZE),
             NULL);
      ensure(flash_otp_lock(FLASH_OTP_DEVICE_SERIAL), NULL);
      device_para_init();
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
    if (check_all_ones(flash_otp_data->flash_otp[FLASH_OTP_CPU_FIRMWARE_INFO],
                       FLASH_OTP_BLOCK_SIZE)) {
      strlcpy((char *)buffer, cpu_info, sizeof(buffer) / 2);
      strlcpy((char *)buffer + FLASH_OTP_BLOCK_SIZE / 2, firmware_ver,
              sizeof(buffer) / 2);
      ensure(flash_otp_write(FLASH_OTP_CPU_FIRMWARE_INFO, 0, buffer,
                             FLASH_OTP_BLOCK_SIZE),
             NULL);
      ensure(flash_otp_lock(FLASH_OTP_CPU_FIRMWARE_INFO), NULL);
      device_para_init();
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

void device_get_enc_key(uint8_t key[32]) {
  SHA256_CTX ctx = {0};

  sha256_Init(&ctx);
  sha256_Update(&ctx, (uint8_t *)dev_info.st_id, sizeof(dev_info.st_id));
  sha256_Update(&ctx, dev_info.random_key, sizeof(dev_info.random_key));
  sha256_Final(&ctx, key);
}

void ui_test_input(void) {
  display_clear();
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      display_bar_radius(20 + j * 100, 20 + i * 180, 40, 40, COLOR_RED,
                         COLOR_WHITE, 16);
    }
  }
  uint32_t pos = 0;
  for (;;) {
    uint32_t evt = touch_read();
    uint16_t x = touch_unpack_x(evt);
    uint16_t y = touch_unpack_y(evt);

    if (!evt) {
      continue;
    }

    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 5; j++) {
        if (x > (20 + j * 100) && x < (20 + j * 100 + 40) &&
            y > (20 + i * 180) && y < (20 + i * 180 + 40)) {
          display_bar_radius(20 + j * 100, 20 + i * 180, 40, 40, COLOR_GREEN,
                             COLOR_WHITE, 16);
          pos |= 1 << (5 * i + j);
        }
        if (pos == 0x1FFFFFF) {
          return;
        }
      }
    }
  }
}

static void ui_generic_confirm_simple(const char *msg) {
  if (msg == NULL) return;
  display_text_center(DISPLAY_RESX / 2, DISPLAY_RESY / 2, msg, -1, FONT_NORMAL,
                      COLOR_WHITE, COLOR_BLACK);

  display_bar_radius(32, DISPLAY_RESY - 160, 128, 64, COLOR_RED, COLOR_BLACK,
                     16);
  display_bar_radius(DISPLAY_RESX - 32 - 128, DISPLAY_RESY - 160, 128, 64,
                     COLOR_GREEN, COLOR_BLACK, 16);
  display_text(80, DISPLAY_RESY - 120, "No", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_RED);
  display_text(DISPLAY_RESX - 118, DISPLAY_RESY - 120, "Yes", -1, FONT_NORMAL,
               COLOR_WHITE, COLOR_GREEN);
}

static bool ui_response(void) {
  for (;;) {
    uint32_t evt = touch_click();
    uint16_t x = touch_unpack_x(evt);
    uint16_t y = touch_unpack_y(evt);

    if (!evt) {
      continue;
    }
    // clicked on Cancel button
    if (x >= 32 && x < 32 + 128 && y > DISPLAY_RESY - 160 &&
        y < DISPLAY_RESY - 160 + 64) {
      return false;
    }
    // clicked on Confirm button
    if (x >= DISPLAY_RESX - 32 - 128 && x < DISPLAY_RESX - 32 &&
        y > DISPLAY_RESY - 160 && y < DISPLAY_RESY - 160 + 64) {
      return true;
    }
  }
}

void device_test(void) {
  if (flash_otp_is_locked(FLASH_OTP_FACTORY_TEST)) {
    return;
  }
  display_bar(0, 0, MAX_DISPLAY_RESX, 266, COLOR_RED);
  display_bar(0, 266, MAX_DISPLAY_RESX, 532, COLOR_GREEN);
  display_bar(0, 532, MAX_DISPLAY_RESX, MAX_DISPLAY_RESY, COLOR_BLUE);

  while (!touch_click()) {
  }

  ui_test_input();

  display_clear();

  uint8_t rand_buffer[32];
  atca_init();

  if (atca_random(rand_buffer)) {
    display_text(0, 20, "SE test faild", -1, FONT_NORMAL, COLOR_RED,
                 COLOR_BLACK);
    while (1)
      ;
  } else {
    display_text(0, 20, "SE test done", -1, FONT_NORMAL, COLOR_WHITE,
                 COLOR_BLACK);
  }

  qspi_flash_init();
  if (qspi_flash_read_id() == 0) {
    display_text(0, 50, "SPI-FLASH test faild", -1, FONT_NORMAL, COLOR_RED,
                 COLOR_BLACK);
    while (1)
      ;
  } else {
    display_text(0, 50, "SPI-FLASH test done", -1, FONT_NORMAL, COLOR_WHITE,
                 COLOR_BLACK);
  }

  emmc_init();

  if (emmc_get_capacity_in_bytes() == 0) {
    display_text(0, 80, "EMMC test faild", -1, FONT_NORMAL, COLOR_RED,
                 COLOR_BLACK);
    while (1)
      ;
  } else {
    display_text(0, 80, "EMMC test done", -1, FONT_NORMAL, COLOR_WHITE,
                 COLOR_BLACK);
  }

  buzzer_init();
  buzzer_ctrl(1);
  hal_delay(1000);
  buzzer_ctrl(0);
  hal_delay(1000);
  buzzer_ctrl(1);
  hal_delay(1000);
  buzzer_ctrl(0);

  ui_generic_confirm_simple("BEEP test");
  if (ui_response()) {
    display_text(0, 110, "BEEP test done", -1, FONT_NORMAL, COLOR_WHITE,
                 COLOR_BLACK);
  } else {
    display_text(0, 110, "BEEP test faild", -1, FONT_NORMAL, COLOR_RED,
                 COLOR_BLACK);
    while (1)
      ;
  }
  display_bar(0, 130, DISPLAY_RESX, DISPLAY_RESY - 100, COLOR_BLACK);

  motor_init();

  motor_ctrl(MOTOR_COAST);
  hal_delay(2000);
  motor_ctrl(MOTOR_BRAKE);

  ui_generic_confirm_simple("MOTOR test");
  if (ui_response()) {
    display_text(0, 140, "MOTOR test done", -1, FONT_NORMAL, COLOR_WHITE,
                 COLOR_BLACK);
  } else {
    display_text(0, 140, "MOTOR test faild", -1, FONT_NORMAL, COLOR_RED,
                 COLOR_BLACK);
    while (1)
      ;
  }

  uint8_t buf[FLASH_OTP_BLOCK_SIZE] = {0};
  memcpy(buf, (uint8_t *)ATCA_CONFIG_VERSION, strlen(ATCA_CONFIG_VERSION));
  ensure(flash_otp_write(FLASH_OTP_FACTORY_TEST, 0, buf, FLASH_OTP_BLOCK_SIZE),
         NULL);
  ensure(flash_otp_lock(FLASH_OTP_FACTORY_TEST), NULL);

  char count_str[24] = {0};
  for (int i = 3; i >= 0; i--) {
    display_bar(0, 140, DISPLAY_RESX, 140, COLOR_BLACK);
    mini_snprintf(count_str, sizeof(count_str), "Done! Restarting in %d s", i);
    display_text(0, 170, count_str, -1, FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
    hal_delay(1000);
  }
  HAL_NVIC_SystemReset();
}
