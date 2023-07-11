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
#include <sys/types.h>

#include "atca_api.h"
#include "atca_command.h"
#include "atca_hal.h"
#include "common.h"
#include "compiler_traits.h"
#include "device.h"
#include "display.h"
#include "flash.h"
#include "image.h"
#include "mini_printf.h"
#include "mipi_lcd.h"
#include "mpu.h"
#include "nand_flash.h"
#include "qspi_flash.h"
#include "random_delays.h"
#include "se_atca.h"
#include "secbool.h"
#ifdef TREZOR_MODEL_T
#include "touch.h"
#endif
#if defined TREZOR_MODEL_R
#include "button.h"
#include "rgb_led.h"
#endif
#include "usb.h"
#include "version.h"

#include "ble.h"
#include "bootui.h"
#include "device.h"
#include "messages.h"
#include "mpu.h"
#include "spi.h"
#include "sys.h"
#include "usart.h"

#define MSG_NAME_TO_ID(x) MessageType_MessageType_##x

#if defined(STM32H747xx)
#include "stm32h7xx_hal.h"
#endif

#include "emmc_wrapper.h"
const uint8_t BOOTLOADER_KEY_M = 4;
const uint8_t BOOTLOADER_KEY_N = 7;
const uint8_t * const BOOTLOADER_KEYS[] = {
    (const uint8_t *)"\x15\x4b\x8a\xb2\x61\xcc\x88\x79\x48\x3f\x68\x9a\x2d\x41\x24\x3a\xe7\xdb\xc4\x02\x16\x72\xbb\xd2\x5c\x33\x8a\xe8\x4d\x93\x11\x54",
    (const uint8_t *)"\xa9\xe6\x5e\x07\xfe\x6d\x39\xa8\xa8\x4e\x11\xa9\x96\xa0\x28\x3f\x88\x1e\x17\x5c\xba\x60\x2e\xb5\xac\x44\x2f\xb7\x5b\x39\xe8\xe0",
    (const uint8_t *)"\x6c\x88\x05\xab\xb2\xdf\x9d\x36\x79\xf1\xd2\x8a\x40\xcd\x99\x03\x99\xb9\x9f\xc3\xee\x4e\x06\x57\xd8\x1d\x38\x1e\xa1\x48\x8a\x12",
    (const uint8_t *)"\x3e\xd7\x97\x79\x06\x4d\x56\x57\x1b\x29\xbc\xaa\x73\x4c\xbb\x6d\xb6\x1d\x2e\x62\x65\x66\x62\x8e\xcf\x4c\x89\xe1\xdb\x45\xea\xec",
    (const uint8_t *)"\x54\xa4\x06\x33\xbf\xd9\xe6\x0b\x8a\x39\x12\x65\xb2\xe0\x06\x37\x4a\xbe\x63\x1d\x1e\x11\x07\x33\x2b\xca\x56\xbf\x9f\x8c\x5c\x99",
    (const uint8_t *)"\x4b\x71\x13\x4f\x18\xe0\x07\x87\xc5\x83\xd4\x07\x42\xcc\x18\x8e\x17\xfc\x85\xad\xe4\xcb\x47\x2d\xae\x5e\xf8\xe0\x69\xf0\xfe\xc5",
    (const uint8_t *)"\x2e\xcf\x80\xc8\x2b\x44\x98\x48\xc0\x00\x33\x50\x92\x13\x95\x51\xbf\xe4\x7b\x3c\x73\x17\xb4\x99\x50\xf6\x5e\x1d\x82\x43\x20\x24",
// comment the lines above and uncomment the lines below to use a custom signed onekey vendorheader
  //  (const uint8_t *)"\xEC\x3C\x75\x23\xE9\x1D\x55\x7D\xD2\xA5\x83\x05\xD6\xF9\x77\x64\xB2\xA2\x54\xC6\x19\x97\x7B\x25\x10\xD4\xE7\xE1\x8A\x83\x21\x14",
  //  (const uint8_t *)"\xF4\x79\xA8\x44\x45\x22\xB3\xF5\x81\x49\xB3\x31\x85\xA5\x07\x68\xCD\xFF\xC0\x28\x5D\x54\x69\xF4\x0D\xB6\x55\x45\x8E\x86\xED\x60",
  //  (const uint8_t *)"\x6D\xDA\xA3\x3C\x09\x1F\x0C\xB0\x20\x43\xF6\x9E\x2D\x2A\xF7\x93\x29\x6F\x65\x91\x3C\x2F\xBC\x65\xCD\xC5\x64\x67\xB1\x80\x30\xBA",
};

#define USB_IFACE_NUM 0

#if !PRODUCTION
#define atca_assert(expr, msg) \
  (((expr) == ATCA_SUCCESS)    \
       ? (void)0               \
       : __fatal_error(#expr, msg, __FILE__, __LINE__, __func__))

// Note: this is for developers to setup a dummy config only!
// configuration to SE is permanent, there is no way to reset it!
static void write_dev_dummy_config() {
  // DO NOT ENABLE THIS UNLESS YOU KNOW WHAT YOU ARE DOING
  // reset OTP to pair with a new SE
  // ensure(flash_erase(FLASH_SECTOR_OTP_EMULATOR), NULL);

  mpu_config_off();

  // device serial
  if (!device_serial_set()) {
    device_set_serial("TCTestSerialNumberXXXXXXXXXXXXX");
  }

  // se keys
  atca_init();

  ATCAConfiguration atca_configuration;
  ATCAPairingInfo pair_info_obj = {0};
  uint8_t se_serial_no[32] = {0};
  FlashLockedData* flash_otp_data = (FlashLockedData*)0x081E0000;

  // get otp data
  memcpy(&pair_info_obj, flash_otp_data->flash_otp[FLASH_OTP_BLOCK_608_SERIAL],
         sizeof(pair_info_obj));

  uint8_t dummy_key[32] = {
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  // 0-7
      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,  // 8-15
      0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,  // 16-23
      0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0,  // 24-32
  };

  if (check_all_ones(pair_info_obj.protect_key,
                     sizeof(pair_info_obj.protect_key))) {
    ensure(flash_otp_write(FLASH_OTP_BLOCK_608_PROTECT_KEY, 0, dummy_key,
                           FLASH_OTP_BLOCK_SIZE),
           NULL);
    ATCA_STATUS ret =
        atca_write_zone(ATCA_ZONE_DATA, SLOT_IO_PROTECT_KEY, 0, 0,
                        pair_info_obj.protect_key, ATCA_BLOCK_SIZE);
    // atca_assert(ret, "init IO key");
    UNUSED(ret);
  }
  if (check_all_ones(pair_info_obj.init_pin, sizeof(pair_info_obj.init_pin))) {
    ensure(flash_otp_write(FLASH_OTP_BLOCK_608_INIT_PIN, 0, dummy_key,
                           FLASH_OTP_BLOCK_SIZE),
           NULL);

    ATCA_STATUS ret = atca_write_zone(ATCA_ZONE_DATA, SLOT_USER_PIN, 0, 0,
                                      pair_info_obj.init_pin, ATCA_BLOCK_SIZE);
    // atca_assert(ret, "init IO key");
    UNUSED(ret);
  }
  if (check_all_ones(pair_info_obj.hash_mix, sizeof(pair_info_obj.hash_mix))) {
    ensure(flash_otp_write(FLASH_OTP_BLOCK_608_MIX_PIN, 0, dummy_key,
                           FLASH_OTP_BLOCK_SIZE),
           NULL);
  }

  atca_config_init();

  // get se config
  atca_assert(atca_read_config_zone((uint8_t*)&atca_configuration),
              "get config");

  // get se sn from config
  memset(se_serial_no, 0xff, sizeof(se_serial_no));
  memcpy(se_serial_no, atca_configuration.sn1, ATECC608_SN1_SIZE);
  memcpy(se_serial_no + ATECC608_SN1_SIZE, atca_configuration.sn2,
         ATECC608_SN2_SIZE);

  // write sn if not already
  if (atca_configuration.lock_value == ATCA_LOCKED) {
    if (check_all_ones(pair_info_obj.serial, sizeof(pair_info_obj.serial))) {
      ensure(flash_otp_write(FLASH_OTP_BLOCK_608_SERIAL, 0, se_serial_no,
                             FLASH_OTP_BLOCK_SIZE),
             NULL);
    }
  }

  // se cert
  uint8_t dummy_cert[] = {
      0x30, 0x82, 0x01, 0x58, 0x30, 0x82, 0x01, 0x0A, 0xA0, 0x03, 0x02, 0x01,
      0x02, 0x02, 0x08, 0x44, 0x9F, 0x65, 0xB6, 0x90, 0xE4, 0x90, 0x09, 0x30,
      0x05, 0x06, 0x03, 0x2B, 0x65, 0x70, 0x30, 0x36, 0x31, 0x0F, 0x30, 0x0D,
      0x06, 0x03, 0x55, 0x04, 0x0A, 0x13, 0x06, 0x4F, 0x6E, 0x65, 0x4B, 0x65,
      0x79, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x13, 0x02,
      0x4E, 0x41, 0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C,
      0x0D, 0x4F, 0x4E, 0x45, 0x4B, 0x45, 0x59, 0x5F, 0x44, 0x45, 0x56, 0x5F,
      0x43, 0x41, 0x30, 0x22, 0x18, 0x0F, 0x39, 0x39, 0x39, 0x39, 0x31, 0x32,
      0x33, 0x31, 0x32, 0x33, 0x35, 0x39, 0x35, 0x39, 0x5A, 0x18, 0x0F, 0x39,
      0x39, 0x39, 0x39, 0x31, 0x32, 0x33, 0x31, 0x32, 0x33, 0x35, 0x39, 0x35,
      0x39, 0x5A, 0x30, 0x2A, 0x31, 0x28, 0x30, 0x26, 0x06, 0x03, 0x55, 0x04,
      0x03, 0x13, 0x1F, 0x54, 0x43, 0x54, 0x65, 0x73, 0x74, 0x53, 0x65, 0x72,
      0x69, 0x61, 0x6C, 0x4E, 0x75, 0x6D, 0x62, 0x65, 0x72, 0x58, 0x58, 0x58,
      0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x30, 0x59,
      0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06,
      0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00,
      0x04, 0x20, 0x32, 0xF5, 0xC1, 0x3B, 0x55, 0x5C, 0x8B, 0xF7, 0xE0, 0xB4,
      0x8A, 0x83, 0x5C, 0x67, 0xD3, 0xC2, 0x04, 0xB7, 0x90, 0x2F, 0x49, 0x78,
      0xF8, 0x5D, 0x2B, 0xFE, 0xA1, 0xAF, 0x0B, 0xCA, 0x6F, 0x94, 0xD3, 0x20,
      0xD9, 0x04, 0x5B, 0xD7, 0x0B, 0xB2, 0x8D, 0xA7, 0xF1, 0x8D, 0x39, 0xA9,
      0xC5, 0x44, 0x53, 0x67, 0x5C, 0xA9, 0x6D, 0x5F, 0x45, 0x74, 0x77, 0x32,
      0x38, 0x8D, 0x91, 0x5F, 0xE2, 0xA3, 0x0F, 0x30, 0x0D, 0x30, 0x0B, 0x06,
      0x03, 0x55, 0x1D, 0x0F, 0x04, 0x04, 0x03, 0x02, 0x07, 0x80, 0x30, 0x05,
      0x06, 0x03, 0x2B, 0x65, 0x70, 0x03, 0x41, 0x00, 0x9F, 0x5D, 0x95, 0xFB,
      0x4A, 0xAD, 0xE6, 0xC6, 0x3B, 0x8E, 0x15, 0xB0, 0xBD, 0x0D, 0xF0, 0x70,
      0x81, 0x4E, 0x05, 0x9A, 0xAD, 0xC4, 0xE4, 0x6E, 0x44, 0xDE, 0xF1, 0xDB,
      0x51, 0xCB, 0x85, 0xB7, 0x5F, 0xAF, 0x55, 0xEB, 0x28, 0x9A, 0x66, 0x95,
      0xAA, 0x08, 0x66, 0x8E, 0x84, 0xC1, 0x22, 0x5D, 0x34, 0x75, 0xF3, 0x01,
      0x2F, 0x6D, 0x33, 0x21, 0x35, 0x1E, 0x54, 0xEC, 0x71, 0xEC, 0x3D, 0x04};

  atca_config_check();
  uint32_t cert_len = 0;
  if (!se_get_certificate_len(&cert_len)) {
    if (!se_write_certificate(dummy_cert, sizeof(dummy_cert)))
      ensure(secfalse, "set cert failed");
  }

  mpu_config_bootloader();
}
#endif

// this is mainly for ignore/supress faults during flash content read (for check
// purpose) if bus fault enabled, it will catched by BusFault_Handler, then we
// could ignore it if bus fault disabled, it will elevate to hard fault, this is
// not what we want
static secbool handle_flash_ecc_error = secfalse;
static void set_handle_flash_ecc_error(secbool val) {
  handle_flash_ecc_error = val;
}
static void bus_fault_enable() { SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk; }
static void bus_fault_disable() { SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTENA_Msk; }

void HardFault_Handler(void) {
  error_shutdown("Internal error", "(HF)", NULL, NULL);
}

void MemManage_Handler_MM(void) {
  error_shutdown("Internal error", "(MM)", NULL, NULL);
}

void MemManage_Handler_SO(void) {
  error_shutdown("Internal error", "(SO)", NULL, NULL);
}

void BusFault_Handler(void) {
  // if want handle flash ecc error
  if (handle_flash_ecc_error == sectrue) {
    // dbgprintf_Wait("Internal flash ECC error detected at 0x%X", SCB->BFAR);

    // check if it's triggered by flash DECC
    if (flash_check_ecc_fault()) {
      // reset flash controller error flags
      flash_clear_ecc_fault(SCB->BFAR);

      // reset bus fault error flags
      SCB->CFSR &= ~(SCB_CFSR_BFARVALID_Msk | SCB_CFSR_PRECISERR_Msk);
      __DSB();
      SCB->SHCSR &= ~(SCB_SHCSR_BUSFAULTACT_Msk);
      __DSB();

      // try to fix ecc error and reboot
      if (flash_fix_ecc_fault_FIRMWARE(SCB->BFAR)) {
        error_shutdown("Internal flash ECC error", "Cleanup successful",
                       "Firmware reinstall required",
                       "If the issue persists, contact support.");
      } else {
        error_shutdown("Internal error", "Cleanup failed",
                       "Reboot to try again",
                       "If the issue persists, contact support.");
      }
    }
  }

  // normal route
  error_shutdown("Internal error", "(BF)", NULL, NULL);
}

void UsageFault_Handler(void) {
  error_shutdown("Internal error", "(UF)", NULL, NULL);
}

static void usb_init_all(secbool usb21_landing) {
  usb_dev_info_t dev_info = {
      .device_class = 0x00,
      .device_subclass = 0x00,
      .device_protocol = 0x00,
      .vendor_id = 0x1209,
      .product_id = 0x4F4A,
      .release_num = 0x0200,
      .manufacturer = "OneKey",
      .product = "ONEKEY Touch Boot",
      .serial_number = "000000000000000000000000",
      .interface = "ONEKEY Interface",
      .usb21_enabled = sectrue,
      .usb21_landing = usb21_landing,
  };

  static uint8_t rx_buffer[USB_PACKET_SIZE];

  static const usb_webusb_info_t webusb_info = {
      .iface_num = USB_IFACE_NUM,
      .ep_in = USB_EP_DIR_IN | 0x01,
      .ep_out = USB_EP_DIR_OUT | 0x01,
      .subclass = 0,
      .protocol = 0,
      .max_packet_len = sizeof(rx_buffer),
      .rx_buffer = rx_buffer,
      .polling_interval = 1,
  };

  usb_init(&dev_info);

  ensure(usb_webusb_add(&webusb_info), NULL);

  usb_start();
}

static secbool bootloader_usb_loop(const vendor_header* const vhdr,
                                   const image_header* const hdr) {
  // if both are NULL, we don't have a firmware installed
  // let's show a webusb landing page in this case
  usb_init_all((vhdr == NULL && hdr == NULL) ? sectrue : secfalse);

  uint8_t buf[USB_PACKET_SIZE];
  int r;

  for (;;) {
    while (true) {
      ble_uart_poll();

      // check usb
      if (USB_PACKET_SIZE == spi_slave_poll(buf)) {
        host_channel = CHANNEL_SLAVE;
        break;
      }
      // check bluetooth
      else if (USB_PACKET_SIZE == usb_webusb_read_blocking(USB_IFACE_NUM, buf,
                                                           USB_PACKET_SIZE,
                                                           200)) {
        host_channel = CHANNEL_USB;
        break;
      }
      // no packet, check if power button pressed
      // else if ( ble_power_button_state() == 1 ) // short press
      else if (ble_power_button_state() == 2)  // long press
      {
        // give a way to go back to bootloader home page
        ble_power_button_state_clear();
        ui_progress_bar_visible_clear();
        ui_fadeout();
        ui_bootloader_first(NULL);
        ui_fadein();
        memzero(buf, USB_PACKET_SIZE);
        continue;
      }
      // no packet, no pwer button pressed
      else {
        ui_bootloader_page_switch(hdr);
        static uint32_t tickstart = 0;
        if ((HAL_GetTick() - tickstart) >= 1000) {
          ui_title_update();
          tickstart = HAL_GetTick();
        }
        continue;
      }
    }

    uint16_t msg_id;
    uint32_t msg_size;
    if (sectrue != msg_parse_header(buf, &msg_id, &msg_size)) {
      // invalid header -> discard
      continue;
    }

    switch (msg_id) {
      case MSG_NAME_TO_ID(Initialize):  // Initialize
        process_msg_Initialize(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
        break;
      case MSG_NAME_TO_ID(Ping):  // Ping
        process_msg_Ping(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(WipeDevice):  // WipeDevice
        ui_fadeout();
#if PRODUCTION_MODEL == 'H'
        ui_wipe_confirm(hdr);
#else
        ui_screen_wipe_confirm();
#endif
        ui_fadein();
#if PRODUCTION_MODEL == 'H'
        int response = ui_input_poll(INPUT_CONFIRM | INPUT_CANCEL, true);
#else
        int response = ui_user_input(INPUT_CONFIRM | INPUT_CANCEL);
#endif
        if (INPUT_CANCEL == response) {
          ui_fadeout();
#if PRODUCTION_MODEL == 'H'
          ui_bootloader_first(hdr);
#else
          ui_screen_firmware_info(vhdr, hdr);
#endif
          ui_fadein();
          send_user_abort(USB_IFACE_NUM, "Wipe cancelled");
          break;
        }
        ui_fadeout();
        ui_screen_wipe();
        ui_fadein();
        r = process_msg_WipeDevice(USB_IFACE_NUM, msg_size, buf);
        if (r < 0) {  // error
          ui_fadeout();
          ui_screen_fail();
          ui_fadein();
          usb_stop();
          usb_deinit();
          while (!touch_click()) {
          }
          restart();
          return secfalse;  // shutdown
        } else {            // success
          ui_fadeout();
          ui_screen_done(0, sectrue);
          ui_fadein();
          usb_stop();
          usb_deinit();
          while (!touch_click()) {
          }
          restart();
          return secfalse;  // shutdown
        }
        break;
      case MSG_NAME_TO_ID(FirmwareErase):  // FirmwareErase
        process_msg_FirmwareErase(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(FirmwareUpload):  // FirmwareUpload
        r = process_msg_FirmwareUpload(USB_IFACE_NUM, msg_size, buf);
        if (r < 0 && r != -4) {  // error, but not user abort (-4)
          ui_fadeout();
          ui_screen_fail();
          ui_fadein();
          usb_stop();
          usb_deinit();
          while (!touch_click()) {
          }
          restart();
          return secfalse;    // shutdown
        } else if (r == 0) {  // last chunk received
          // ui_screen_install_progress_upload(1000);
          ui_fadeout();
          ui_screen_done(4, sectrue);
          ui_fadein();
          ui_screen_done(3, secfalse);
          hal_delay(1000);
          ui_screen_done(2, secfalse);
          hal_delay(1000);
          ui_screen_done(1, secfalse);
          hal_delay(1000);
          usb_stop();
          usb_deinit();
          display_clear();
          return sectrue;  // jump to firmware
        }
        break;
      case MSG_NAME_TO_ID(FirmwareErase_ex):  // erase ble update buffer
        process_msg_FirmwareEraseBLE(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(GetFeatures):  // GetFeatures
        process_msg_GetFeatures(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
        break;
      case MSG_NAME_TO_ID(Reboot):  // Reboot
        process_msg_Reboot(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(FirmwareUpdateEmmc):  // FirmwareUpdateEmmc
        process_msg_FirmwareUpdateEmmc(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcFixPermission):  // EmmcFixPermission
        process_msg_EmmcFixPermission(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcPathInfo):  // EmmcPathInfo
        process_msg_EmmcPathInfo(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcFileRead):  // EmmcFileRead
        process_msg_EmmcFileRead(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcFileWrite):  // EmmcFileWrite
        process_msg_EmmcFileWrite(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcFileDelete):  // EmmcFileDelete
        process_msg_EmmcFileDelete(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcDirList):  // EmmcDirList
        process_msg_EmmcDirList(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcDirMake):  // EmmcDirMake
        process_msg_EmmcDirMake(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcDirRemove):  // EmmcDirRemove
        process_msg_EmmcDirRemove(USB_IFACE_NUM, msg_size, buf);
        break;
      default:
        process_msg_unknown(USB_IFACE_NUM, msg_size, buf);
        break;
    }
  }
}

secbool bootloader_usb_loop_factory(const vendor_header* const vhdr,
                                    const image_header* const hdr) {
  // if both are NULL, we don't have a firmware installed
  // let's show a webusb landing page in this case
  usb_init_all((vhdr == NULL && hdr == NULL) ? sectrue : secfalse);

  uint8_t buf[USB_PACKET_SIZE];
  int r;

  for (;;) {
    r = usb_webusb_read_blocking(USB_IFACE_NUM, buf, USB_PACKET_SIZE,
                                 USB_TIMEOUT);
    if (r != USB_PACKET_SIZE) {
      continue;
    }
    host_channel = CHANNEL_USB;

    uint16_t msg_id;
    uint32_t msg_size;
    if (sectrue != msg_parse_header(buf, &msg_id, &msg_size)) {
      // invalid header -> discard
      continue;
    }

    switch (msg_id) {
      case MSG_NAME_TO_ID(Initialize):  // Initialize
        process_msg_Initialize(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
        break;
      case MSG_NAME_TO_ID(Ping):  // Ping
        process_msg_Ping(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(GetFeatures):  // GetFeatures
        process_msg_GetFeatures(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
        break;
      case MSG_NAME_TO_ID(DeviceInfoSettings):  // DeviceInfoSettings
        process_msg_DeviceInfoSettings(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(GetDeviceInfo):  // GetDeviceInfo
        process_msg_GetDeviceInfo(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(ReadSEPublicKey):  // ReadSEPublicKey
        process_msg_ReadSEPublicKey(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(WriteSEPublicCert):  // WriteSEPublicCert
        process_msg_WriteSEPublicCert(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(ReadSEPublicCert):  // ReadSEPublicCert
        process_msg_ReadSEPublicCert(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(SESignMessage):  // SESignMessage
        process_msg_SESignMessage(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(Reboot):  // Reboot
        process_msg_Reboot(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcFixPermission):  // EmmcFixPermission
        process_msg_EmmcFixPermission(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcPathInfo):  // EmmcPathInfo
        process_msg_EmmcPathInfo(USB_IFACE_NUM, msg_size, buf);
        break;
      // case MSG_NAME_TO_ID(EmmcFileRead): // EmmcFileRead
      //   process_msg_EmmcFileRead(USB_IFACE_NUM, msg_size, buf);
      //   break;
      // case MSG_NAME_TO_ID(EmmcFileWrite): // EmmcFileWrite
      //   process_msg_EmmcFileWrite(USB_IFACE_NUM, msg_size, buf);
      //   break;
      // case MSG_NAME_TO_ID(EmmcFileDelete): // EmmcFileDelete
      //   process_msg_EmmcFileDelete(USB_IFACE_NUM, msg_size, buf);
      //   break;
      case MSG_NAME_TO_ID(EmmcDirList):  // EmmcDirList
        process_msg_EmmcDirList(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcDirMake):  // EmmcDirMake
        process_msg_EmmcDirMake(USB_IFACE_NUM, msg_size, buf);
        break;
      case MSG_NAME_TO_ID(EmmcDirRemove):  // EmmcDirRemove
        process_msg_EmmcDirRemove(USB_IFACE_NUM, msg_size, buf);
        break;
      default:
        process_msg_unknown(USB_IFACE_NUM, msg_size, buf);
        break;
    }
  }
  return sectrue;
}

secbool load_vendor_header_keys(const uint8_t* const data,
                                vendor_header* const vhdr) {
  return load_vendor_header(data, BOOTLOADER_KEY_M, BOOTLOADER_KEY_N,
                            BOOTLOADER_KEYS, vhdr);
}

static secbool check_vendor_header_lock(const vendor_header* const vhdr) {
  uint8_t lock[FLASH_OTP_BLOCK_SIZE];
  ensure(flash_otp_read(FLASH_OTP_BLOCK_VENDOR_HEADER_LOCK, 0, lock,
                        FLASH_OTP_BLOCK_SIZE),
         NULL);
  if (0 ==
      memcmp(lock,
             "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
             "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
             FLASH_OTP_BLOCK_SIZE)) {
    return sectrue;
  }
  uint8_t hash[32];
  vendor_header_hash(vhdr, hash);
  return sectrue * (0 == memcmp(lock, hash, 32));
}

// protection against bootloader downgrade

#if PRODUCTION

static void check_bootloader_version(void) {
  uint8_t bits[FLASH_OTP_BLOCK_SIZE];
  for (int i = 0; i < FLASH_OTP_BLOCK_SIZE * 8; i++) {
    if (i < VERSION_MONOTONIC) {
      bits[i / 8] &= ~(1 << (7 - (i % 8)));
    } else {
      bits[i / 8] |= (1 << (7 - (i % 8)));
    }
  }
  ensure(flash_otp_write(FLASH_OTP_BLOCK_BOOTLOADER_VERSION, 0, bits,
                         FLASH_OTP_BLOCK_SIZE),
         NULL);

  uint8_t bits2[FLASH_OTP_BLOCK_SIZE];
  ensure(flash_otp_read(FLASH_OTP_BLOCK_BOOTLOADER_VERSION, 0, bits2,
                        FLASH_OTP_BLOCK_SIZE),
         NULL);

  ensure(sectrue * (0 == memcmp(bits, bits2, FLASH_OTP_BLOCK_SIZE)),
         "Bootloader downgraded");
}

#endif

static secbool validate_firmware_headers(vendor_header* const vhdr,
                                         image_header* const hdr) {
  set_handle_flash_ecc_error(sectrue);
  secbool result = secfalse;
  while (true) {
    // check
    if (sectrue !=
        load_vendor_header_keys((const uint8_t*)FIRMWARE_START, vhdr))
      break;

    if (sectrue != check_vendor_header_lock(vhdr)) break;

    if (sectrue !=
        load_image_header((const uint8_t*)(FIRMWARE_START + vhdr->hdrlen),
                          FIRMWARE_IMAGE_MAGIC, FIRMWARE_IMAGE_MAXSIZE,
                          vhdr->vsig_m, vhdr->vsig_n, vhdr->vpub, hdr))
      break;

    // passed, return true
    result = sectrue;
    break;
  }
  set_handle_flash_ecc_error(secfalse);
  return result;
}

static secbool validate_firmware_code(vendor_header* const vhdr,
                                      image_header* const hdr) {
  set_handle_flash_ecc_error(sectrue);
  secbool result =
      check_image_contents(hdr, IMAGE_HEADER_SIZE + vhdr->hdrlen,
                           FIRMWARE_SECTORS, FIRMWARE_SECTORS_COUNT);
  set_handle_flash_ecc_error(secfalse);
  return result;
}

int main(void) {
  volatile uint32_t stay_in_bootloader_flag = *STAY_IN_FLAG_ADDR;
  bool serial_set = false, cert_set = false;

  SystemCoreClockUpdate();

  lcd_para_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
  lcd_pwm_init();
  random_delays_init();

  ble_usart_init();

  device_para_init();

  if (!serial_set) {
    serial_set = device_serial_set();
  }

  if (!serial_set) {
    pcb_version = PCB_VERSION_2_1_0;
  } else {
    char* factory_data = NULL;
    char* serial;
    device_get_serial(&serial);
    factory_data = serial + 7;
    if (memcmp(factory_data, "20220910", 8) >= 0) {
      pcb_version = PCB_VERSION_2_1_0;
    } else {
      pcb_version = PCB_VERSION_1_0_0;
    }
  }
#if defined TREZOR_MODEL_T
  // display_set_little_endian();
  touch_init();
  touch_power_on();
#endif

#if defined TREZOR_MODEL_R
  button_init();
  rgb_led_init();
#endif

#if !PRODUCTION
  // write_dev_dummy_config();
  UNUSED(write_dev_dummy_config);
#endif

  bus_fault_enable();

  device_test();

  atca_init();
  atca_config_check();

  if (!cert_set) {
    uint32_t cert_len = 0;
    cert_set = se_get_certificate_len(&cert_len);
  }

  if (!serial_set || !cert_set) {
    display_clear();
    device_set_factory_mode(true);
    ui_bootloader_factory();
    if (bootloader_usb_loop_factory(NULL, NULL) != sectrue) {
      return 1;
    }
  }

#if PRODUCTION
  device_burnin_test();
#endif

  qspi_flash_init();
  qspi_flash_config();
  qspi_flash_memory_mapped();

  mpu_config_bootloader();

#if PRODUCTION
  check_bootloader_version();
#endif

  ensure_emmcfs(emmc_fs_init(), "emmc_fs_init");
  ensure_emmcfs(emmc_fs_mount(true, false), "emmc_fs_mount");

  buzzer_init();
  motor_init();
  spi_slave_init();

  secbool stay_in_bootloader = secfalse;  // flag to stay in bootloader

  if (stay_in_bootloader_flag == STAY_IN_BOOTLOADER_FLAG) {
    *STAY_IN_FLAG_ADDR = 0;
    stay_in_bootloader = sectrue;
  }

  // delay to detect touch or skip if we know we are staying in bootloader
  // anyway
  uint32_t touched = 0;
#if defined TREZOR_MODEL_T
  if (stay_in_bootloader != sectrue) {
    for (int i = 0; i < 100; i++) {
      touched = touch_is_detected() | touch_read();
      if (touched) {
        break;
      }
      hal_delay(1);
    }
  }
#elif defined TREZOR_MODEL_R
  button_read();
  if (button_state_left() == 1) {
    touched = 1;
  }
#endif

  vendor_header vhdr;
  image_header hdr;

  // check stay_in_bootloader flag
  if (stay_in_bootloader == sectrue) {
    display_clear();
    if (sectrue == validate_firmware_headers(&vhdr, &hdr)) {
      ui_bootloader_first(&hdr);
      if (bootloader_usb_loop(&vhdr, &hdr) != sectrue) {
        return 1;
      }
    } else {
      ui_bootloader_first(NULL);
      if (bootloader_usb_loop(NULL, NULL) != sectrue) {
        return 1;
      }
    }
  }

  // check if firmware valid
  if (sectrue == validate_firmware_headers(&vhdr, &hdr)) {
    if (sectrue == validate_firmware_code(&vhdr, &hdr)) {
      // __asm("nop"); // all good, do nothing
    } else {
      display_clear();
      ui_bootloader_first(&hdr);
      if (bootloader_usb_loop(&vhdr, &hdr) != sectrue) {
        return 1;
      }
    }
  } else {
    display_clear();
    ui_bootloader_first(NULL);
    if (bootloader_usb_loop(NULL, NULL) != sectrue) {
      return 1;
    }
  }

  // check if firmware valid again to make sure
  ensure(validate_firmware_headers(&vhdr, &hdr), "invalid firmware header");
  ensure(validate_firmware_code(&vhdr, &hdr), "invalid firmware code");

  // if all VTRUST flags are unset = ultimate trust => skip the procedure
  if ((vhdr.vtrust & VTRUST_ALL) != VTRUST_ALL) {
    // ui_fadeout();  // no fadeout - we start from black screen
    ui_screen_boot(&vhdr, &hdr);
    ui_fadein();

    int delay = (vhdr.vtrust & VTRUST_WAIT) ^ VTRUST_WAIT;
    if (delay > 1) {
      while (delay > 0) {
        ui_screen_boot_wait(delay);
        hal_delay(1000);
        delay--;
      }
    } else if (delay == 1) {
      hal_delay(1000);
    }

    if ((vhdr.vtrust & VTRUST_CLICK) == 0) {
      ui_screen_boot_click();
      while (touch_read() == 0)
        ;
    }
    display_clear();
  }

  // mpu_config_firmware();
  // jump_to_unprivileged(FIRMWARE_START + vhdr.hdrlen + IMAGE_HEADER_SIZE);
  bus_fault_disable();
  mpu_config_off();
  jump_to(FIRMWARE_START + vhdr.hdrlen + IMAGE_HEADER_SIZE);

  return 0;
}
