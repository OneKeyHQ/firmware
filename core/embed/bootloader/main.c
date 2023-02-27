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
#include "atca_hal.h"
#include "common.h"
#include "compiler_traits.h"
#include "device.h"
#include "display.h"
#include "emmc.h"
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
#include "touch.h"
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

#if defined(STM32H747xx)
#include "stm32h7xx_hal.h"
#endif

#include "ff.h"

PARTITION VolToPart[FF_VOLUMES] = {
    {0, 1},
    {0, 2},
};

const uint8_t BOOTLOADER_KEY_M = 4;
const uint8_t BOOTLOADER_KEY_N = 7;
static const uint8_t * const BOOTLOADER_KEYS[] = {
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

static void usb_init_all(secbool usb21_landing) {
  usb_dev_info_t dev_info = {
      .device_class = 0x00,
      .device_subclass = 0x00,
      .device_protocol = 0x00,
      .vendor_id = 0x1209,
      .product_id = 0x4F4A,
      .release_num = 0x0200,
      .manufacturer = "OneKey",
      .product = "ONEKEY",
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

static secbool bootloader_usb_loop(const vendor_header *const vhdr,
                                   const image_header *const hdr) {
  // if both are NULL, we don't have a firmware installed
  // let's show a webusb landing page in this case
  usb_init_all((vhdr == NULL && hdr == NULL) ? sectrue : secfalse);

  uint8_t buf[USB_PACKET_SIZE];
  int r;

  for (;;) {
    ble_uart_poll();
    r = spi_slave_poll(buf);
    if (r != USB_PACKET_SIZE) {
      r = usb_webusb_read_blocking(USB_IFACE_NUM, buf, USB_PACKET_SIZE, 200);
      if (r != USB_PACKET_SIZE) {
        ui_bootloader_page_switch(hdr);
        static uint32_t tickstart = 0;
        if ((HAL_GetTick() - tickstart) >= 1000) {
          ui_title_update();
          tickstart = HAL_GetTick();
        }
        continue;
      }
      host_channel = CHANNEL_USB;
    } else {
      host_channel = CHANNEL_SLAVE;
    }

    uint16_t msg_id;
    uint32_t msg_size;
    if (sectrue != msg_parse_header(buf, &msg_id, &msg_size)) {
      // invalid header -> discard
      continue;
    }

    switch (msg_id) {
      case 0:  // Initialize
        process_msg_Initialize(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
        break;
      case 1:  // Ping
        process_msg_Ping(USB_IFACE_NUM, msg_size, buf);
        break;
      case 5:  // WipeDevice
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
      case 6:  // FirmwareErase
        process_msg_FirmwareErase(USB_IFACE_NUM, msg_size, buf);
        break;
      case 7:  // FirmwareUpload
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
          ui_fadeout();
          return sectrue;  // jump to firmware
        }
        break;
      case 16:  // erase ble update buffer
        process_msg_FirmwareEraseBLE(USB_IFACE_NUM, msg_size, buf);
        break;
      case 55:  // GetFeatures
        process_msg_GetFeatures(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
        break;
      default:
        process_msg_unknown(USB_IFACE_NUM, msg_size, buf);
        break;
    }
  }
}

secbool bootloader_usb_loop_factory(const vendor_header *const vhdr,
                                    const image_header *const hdr) {
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
      case 0:  // Initialize
        process_msg_Initialize(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
        break;
      case 1:  // Ping
        process_msg_Ping(USB_IFACE_NUM, msg_size, buf);
        break;
      case 55:  // GetFeatures
        process_msg_GetFeatures(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
        break;
      case 10001:  // DeviceInfoSettings
        process_msg_DeviceInfoSettings(USB_IFACE_NUM, msg_size, buf);
        break;
      case 10002:  // GetDeviceInfo
        process_msg_GetDeviceInfo(USB_IFACE_NUM, msg_size, buf);
        break;
      case 10004:  // ReadSEPublicKey
        process_msg_ReadSEPublicKey(USB_IFACE_NUM, msg_size, buf);
        break;
      case 10006:  // WriteSEPublicCert
        process_msg_WriteSEPublicCert(USB_IFACE_NUM, msg_size, buf);
        break;
      case 10007:  // ReadSEPublicCert
        process_msg_ReadSEPublicCert(USB_IFACE_NUM, msg_size, buf);
        break;
      case 10012:  // SESignMessage
        process_msg_SESignMessage(USB_IFACE_NUM, msg_size, buf);
        break;
      default:
        process_msg_unknown(USB_IFACE_NUM, msg_size, buf);
        break;
    }
  }
  return sectrue;
}

secbool load_vendor_header_keys(const uint8_t *const data,
                                vendor_header *const vhdr) {
  return load_vendor_header(data, BOOTLOADER_KEY_M, BOOTLOADER_KEY_N,
                            BOOTLOADER_KEYS, vhdr);
}

static secbool check_vendor_header_lock(const vendor_header *const vhdr) {
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

int main(void) {
  volatile uint32_t stay_in_bootloader_flag = *STAY_IN_FLAG_ADDR;
  bool serial_set = false, cert_set = false;
  uint32_t cert_len = 0;

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
    char *factory_data = NULL;
    char *serial;
    device_get_serial(&serial);
    factory_data = serial + 7;
    if (memcmp(factory_data, "20220910", 8) >= 0) {
      pcb_version = PCB_VERSION_2_1_0;
    } else {
      pcb_version = PCB_VERSION_1_0_0;
    }
  }

  touch_init();
  touch_power_on();

  device_test();

  atca_init();
  atca_config_check();

  if (!cert_set) {
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

  device_burnin_test();

  qspi_flash_init();
  qspi_flash_config();
  qspi_flash_memory_mapped();

  mpu_config_bootloader();

#if PRODUCTION
  check_bootloader_version();
#endif

  emmc_init();

  buzzer_init();
  motor_init();
  spi_slave_init();

  secbool stay_in_bootloader = secfalse;  // flag to stay in bootloader

  if (stay_in_bootloader_flag == STAY_IN_BOOTLOADER_FLAG) {
    *STAY_IN_FLAG_ADDR = 0;
    stay_in_bootloader = sectrue;
  }

  vendor_header vhdr;
  image_header hdr;

  // detect whether the devices contains a valid firmware

  secbool firmware_present =
      load_vendor_header_keys((const uint8_t *)FIRMWARE_START, &vhdr);
  if (sectrue == firmware_present) {
    firmware_present = check_vendor_header_lock(&vhdr);
  }

  if (sectrue == firmware_present) {
    firmware_present = load_image_header(
        (const uint8_t *)(FIRMWARE_START + vhdr.hdrlen), FIRMWARE_IMAGE_MAGIC,
        FIRMWARE_IMAGE_MAXSIZE, vhdr.vsig_m, vhdr.vsig_n, vhdr.vpub, &hdr);
  }

  if (sectrue == firmware_present) {
    firmware_present =
        check_image_contents(&hdr, IMAGE_HEADER_SIZE + vhdr.hdrlen,
                             FIRMWARE_SECTORS, FIRMWARE_SECTORS_COUNT);
  }

  // start the bootloader if no or broken firmware found ...

  if (firmware_present != sectrue) {
    display_clear();
    ui_bootloader_first(NULL);

    // erase storage
    // ensure(flash_erase_sectors(STORAGE_SECTORS, STORAGE_SECTORS_COUNT, NULL),
    //        NULL);

    // and start the usb loop
    if (bootloader_usb_loop(NULL, NULL) != sectrue) {
      return 1;
    }
  }

  // ... or if user touched the screen on start
  // ... or we have stay_in_bootloader flag to force it
  if (stay_in_bootloader == sectrue) {
    display_clear();
    ui_bootloader_first(&hdr);
    // no ui_fadeout(); - we already start from black screen
    if (firmware_present != sectrue) {
      // and start the usb loop
      if (bootloader_usb_loop(NULL, NULL) != sectrue) {
        return 1;
      }
    } else {
      // and start the usb loop
      if (bootloader_usb_loop(&vhdr, &hdr) != sectrue) {
        return 1;
      }
    }
  }

  ensure(load_vendor_header_keys((const uint8_t *)FIRMWARE_START, &vhdr),
         "invalid vendor header");

  ensure(check_vendor_header_lock(&vhdr), "unauthorized vendor keys");

  ensure(load_image_header((const uint8_t *)(FIRMWARE_START + vhdr.hdrlen),
                           FIRMWARE_IMAGE_MAGIC, FIRMWARE_IMAGE_MAXSIZE,
                           vhdr.vsig_m, vhdr.vsig_n, vhdr.vpub, &hdr),
         "invalid firmware header");

  ensure(check_image_contents(&hdr, IMAGE_HEADER_SIZE + vhdr.hdrlen,
                              FIRMWARE_SECTORS, FIRMWARE_SECTORS_COUNT),
         "invalid firmware hash");

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

    ui_fadeout();
  }

  // mpu_config_firmware();
  // jump_to_unprivileged(FIRMWARE_START + vhdr.hdrlen + IMAGE_HEADER_SIZE);
  mpu_config_off();
  jump_to(FIRMWARE_START + vhdr.hdrlen + IMAGE_HEADER_SIZE);

  return 0;
}
