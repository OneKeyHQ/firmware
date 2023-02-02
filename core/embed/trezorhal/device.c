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

#include "ble.h"
#include "ff.h"
#include "jpeg_dma.h"
#include "sdram.h"
#include "usart.h"

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
    for (int j = 0; j < 6; j++) {
      display_bar_radius(j * 80, (j % 2) * 80 + i * 160, 80, 80, COLOR_RED,
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
      for (int j = 0; j < 6; j++) {
        if (x > (j * 80) && x < (j * 80 + 80) && y > ((j % 2) * 80 + i * 160) &&
            y < ((j % 2) * 80 + i * 160 + 80)) {
          display_bar_radius(j * 80, (j % 2) * 80 + i * 160, 80, 80,
                             COLOR_GREEN, COLOR_WHITE, 16);
          pos |= 1 << (6 * i + j);
        }
        if (pos == 0x3FFFFFFF) {
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

  motor_ctrl(MOTOR_REVERSE);
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
  for (int i = 1; i >= 0; i--) {
    display_bar(0, 140, DISPLAY_RESX, 140, COLOR_BLACK);
    mini_snprintf(count_str, sizeof(count_str), "Done! Restarting in %d s", i);
    display_text(0, 170, count_str, -1, FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
    hal_delay(1000);
  }
  HAL_NVIC_SystemReset();
}

static FATFS fs_instance;

typedef struct {
  uint32_t flag;
  uint32_t time;
  uint32_t touch;
} test_result;

typedef enum {
  TEST_NULL = 0x00000000,
  TEST_TESTING = 0x11111111,
  TEST_PASS = 0x22222222,
  TEST_FAILED = 0x33333333
} test_status;

static TIM_HandleTypeDef TimHandle;

static void timer_init(void) {
  __HAL_RCC_TIM2_CLK_ENABLE();
  TimHandle.Instance = TIM2;
  TimHandle.Init.Prescaler = (uint32_t)(SystemCoreClock / (2 * 10000)) - 1;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.Period = 0xffffffff;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&TimHandle);
  HAL_TIM_Base_Start(&TimHandle);
}

void device_burnin_test(void) {
  char *serial;
  device_get_serial(&serial);
  if (memcmp(serial + 7, "20230201", 8) < 0) {
    return;
  }

  uint32_t start, current;
  uint8_t rand_buffer[32];
  uint32_t se_err = 0;

  volatile uint64_t emmc_cap = 0;
  volatile uint32_t flash_id = 0;
  volatile uint32_t index = 0, index_bak = 0xff;
  volatile uint32_t click = 0, click_pre = 0, click_now = 0;
  FRESULT res;
  FIL fil;

  UINT br, bw;
  test_result test_res = {0};

  emmc_init();
  qspi_flash_init();
  timer_init();
  jpeg_init();

  start = __HAL_TIM_GET_COUNTER(&TimHandle);

  res = f_mount(&fs_instance, "", 1);
  if (res != FR_OK) {
    display_text_center(DISPLAY_RESX / 2, DISPLAY_RESY / 2,
                        "mount fatfs failed", -1, FONT_NORMAL, COLOR_RED,
                        COLOR_BLACK);
    while (1)
      ;
  }

  f_open(&fil, "test_res", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
  f_chmod("test_res", AM_SYS | AM_HID, AM_SYS | AM_HID);

  f_read(&fil, &test_res, sizeof(test_res), &br);
  if (br == 0) {
    test_res.flag = TEST_TESTING;
    test_res.time = start;
    f_write(&fil, &test_res, sizeof(test_res), &bw);
    f_sync(&fil);
  } else {
    if (test_res.flag == TEST_TESTING) {
      start = test_res.time;
    } else if (test_res.flag == TEST_PASS) {
      if (test_res.touch != TEST_PASS) {
        ui_test_input();
        test_res.touch = TEST_PASS;
        f_lseek(&fil, 0);
        f_write(&fil, &test_res, sizeof(test_res), &bw);
        f_sync(&fil);
        ble_cmd_req(BLE_BT, BLE_BT_ON);
        restart();
      }
      return;
    }
  }

  do {
    ble_uart_poll();
    if (touch_click()) {
      if (click == 0) {
        click = 1;
        click_pre = __HAL_TIM_GET_COUNTER(&TimHandle);
      } else {
        click_now = __HAL_TIM_GET_COUNTER(&TimHandle);
        // 1s
        if (click_now - click_pre > 10000) {
          click_pre = click_now;
        } else {
          display_clear();
          HAL_TIM_Base_Stop(&TimHandle);
          ui_generic_confirm_simple("EXIT  TEST?");
          if (ui_response()) {
            test_res.flag = TEST_PASS;
            f_lseek(&fil, 0);
            f_write(&fil, &test_res, sizeof(test_res), &bw);
            f_sync(&fil);
            restart();
          } else {
            click = 0;
            index_bak = 0xff;
            HAL_TIM_Base_Start(&TimHandle);
          }
        }
      }
    }
    current = start + __HAL_TIM_GET_COUNTER(&TimHandle);

    index = (current / 30000) % 4;

    if (index != index_bak) {
      index_bak = index;
      switch (index) {
        case 0:
          // display_bar(0, 0, MAX_DISPLAY_RESX, MAX_DISPLAY_RESY, COLOR_RED);
          if (jped_decode("res/wallpaper-1.jpg",
                          FMC_SDRAM_LVGL_BUFFER_ADDRESS) != 0) {
            display_print_clear();
            display_printf("show wallpaper-1.jpg err\n");
          }
          break;
        case 1:
          if (jped_decode("res/wallpaper-2.jpg",
                          FMC_SDRAM_LVGL_BUFFER_ADDRESS) != 0) {
            display_print_clear();
            display_printf("show wallpaper-2.jpg err\n");
          }
          break;
        case 2:
          if (jped_decode("res/wallpaper-3.jpg",
                          FMC_SDRAM_LVGL_BUFFER_ADDRESS) != 0) {
            display_print_clear();
            display_printf("show wallpaper-3.jpg err\n");
          }
          break;
        case 3:
          display_clear();
          display_print_clear();
          display_printf("SPI_FLASH ID= 0x%X \n", (unsigned)flash_id);

          if (emmc_cap > (1024 * 1024 * 1024)) {
            display_printf("EMMC CAP= %d GB\n", (unsigned int)(emmc_cap >> 30));
          } else if (emmc_cap > (1024 * 1024)) {
            display_printf("EMMC CAP= %d MB\n", (unsigned int)(emmc_cap >> 20));
          } else {
            display_printf("EMMC CAP= %d Bytes\n", (unsigned int)emmc_cap);
          }
          display_printf("SE RANDOM:\n");
          for (int i = 0; i < 32; i++) {
            display_printf("%02X ", rand_buffer[i]);
          }
          display_printf("\n");
          if (ble_name_state()) {
            display_printf("BLE NAME = %s\n", ble_get_name());
          }
          if (ble_battery_state()) {
            display_printf("BATTERY= %d%%\n", battery_cap);
          }
          if (ble_switch_state()) {
            if (ble_get_switch()) {
              display_printf("BLE ON,TURN OFF BLE...\n");
              ble_cmd_req(BLE_BT, BLE_BT_OFF);
              hal_delay(5);
            } else {
              display_printf("BLE OFF,TURN ON BLE...\n");
              ble_cmd_req(BLE_BT, BLE_BT_ON);
              hal_delay(5);
            }
          }

          break;
        default:
          break;
      }
    }

    emmc_cap = emmc_get_capacity_in_bytes();
    if (emmc_cap == 0) {
      display_text_center(DISPLAY_RESX / 2, DISPLAY_RESY / 2, "EMMC test faild",
                          -1, FONT_NORMAL, COLOR_RED, COLOR_BLACK);
      while (1)
        ;
    }
    flash_id = qspi_flash_read_id();
    if (flash_id == 0) {
      display_text_center(DISPLAY_RESX / 2, DISPLAY_RESY / 2,
                          "SPI-FLASH test faild", -1, FONT_NORMAL, COLOR_RED,
                          COLOR_BLACK);
      while (1)
        ;
    }

    if (atca_random(rand_buffer)) {
      se_err++;
      if (se_err == 10) {
        display_text_center(DISPLAY_RESX / 2, DISPLAY_RESY / 2, "SE test faild",
                            -1, FONT_NORMAL, COLOR_RED, COLOR_BLACK);
        while (1)
          ;
      }
    }
    if (!ble_name_state()) {
      ble_cmd_req(BLE_VER, BLE_VER_ADV);
      hal_delay(5);
    }
    if (!ble_battery_state()) {
      ble_cmd_req(BLE_PWR, BLE_PWR_EQ);
      hal_delay(5);
    }
    if (!ble_switch_state()) {
      ble_cmd_req(BLE_BT, BLE_BT_STA);
      hal_delay(5);
    }

    if ((current - start) > 15 * 60 * 10000) {
      test_res.flag = TEST_TESTING;
      test_res.time = current;
      f_lseek(&fil, 0);
      f_write(&fil, &test_res, sizeof(test_res), &bw);
      f_sync(&fil);
      restart();
    }
    hal_delay(5);
  } while (current < 3 * 60 * 60 * 10000);  // 2hours

  test_res.flag = TEST_PASS;
  test_res.time = current;
  f_lseek(&fil, 0);
  f_write(&fil, &test_res, sizeof(test_res), &bw);
  f_sync(&fil);
  restart();
}
