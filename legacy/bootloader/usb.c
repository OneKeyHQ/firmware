/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
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

#include <libopencm3/stm32/flash.h>
#include <libopencm3/usb/usbd.h>

#include <string.h>

#include "ble.h"
#include "bootloader.h"
#include "buttons.h"
#include "ecdsa.h"
#include "fw_signatures.h"
#include "layout.h"
#include "layout_boot.h"
#include "memory.h"
#include "memzero.h"
#include "oled.h"
#include "rng.h"
#include "secbool.h"
#include "secp256k1.h"
#include "sha2.h"
#include "si2c.h"
#include "sys.h"
#include "updateble.h"
#include "usb.h"
#include "util.h"

#include "timer.h"
#include "usart.h"

#include "nordic_dfu.h"

#include "usb21_standard.h"
#include "webusb.h"
#include "winusb.h"

#include "usb_desc.h"

#if ONEKEY_MINI
#include "w25qxx.h"
#endif

enum {
  STATE_READY,
  STATE_OPEN,
  STATE_FLASHSTART,
  STATE_FLASHING,
  STATE_INTERRPUPT,
  STATE_CHECK,
  STATE_END,
};

#define NORDIC_BLE_UPDATE 0

#define UPDATE_BLE 0x5A
#define UPDATE_ST 0x55
uint32_t flash_pos = 0, flash_len = 0;
static uint32_t chunk_idx = 0;
static char flash_state = STATE_READY;

static uint8_t packet_buf[64] __attribute__((aligned(4)));

#include "usb_send.h"

static uint32_t FW_HEADER[FLASH_FWHEADER_LEN / sizeof(uint32_t)];
uint32_t FW_CHUNK[FW_CHUNK_SIZE / sizeof(uint32_t)];
static uint8_t update_mode = 0;
static void flash_enter(void) {
  flash_wait_for_last_operation();
  flash_clear_status_flags();
  flash_unlock();
}

static void flash_exit(void) {
  flash_wait_for_last_operation();
  flash_lock();
}

#include "usb_erase.h"

static void check_and_write_chunk(void) {
  uint32_t offset = (chunk_idx == 0) ? FLASH_FWHEADER_LEN : 0;
  uint32_t chunk_pos = flash_pos % FW_CHUNK_SIZE;
  if (chunk_pos == 0) {
    chunk_pos = FW_CHUNK_SIZE;
  }
  uint8_t hash[32] = {0};
  SHA256_CTX ctx = {0};
  sha256_Init(&ctx);
  sha256_Update(&ctx, (const uint8_t *)FW_CHUNK + offset, chunk_pos - offset);
  if (chunk_pos < 64 * 1024) {
    // pad with FF
    for (uint32_t i = chunk_pos; i < 64 * 1024; i += 4) {
      sha256_Update(&ctx, (const uint8_t *)"\xFF\xFF\xFF\xFF", 4);
    }
  }
  sha256_Final(&ctx, hash);

  const image_header *hdr = (const image_header *)FW_HEADER;
  // invalid chunk sent
  if (0 != memcmp(hash, hdr->hashes + chunk_idx * 32, 32)) {
#if !ONEKEY_MINI
    // erase storage
    erase_storage();
#endif
    flash_state = STATE_END;
    show_halt("Error installing", "firmware.");
    return;
  }

#if ONEKEY_MINI
  if (chunk_idx == 0) {
    w25qxx_write_buffer_unsafe((uint8_t *)FW_CHUNK + offset,
                               SPI_FLASH_FIRMWARE_ADDR_START + offset,
                               chunk_pos - offset);
  } else {
    w25qxx_write_buffer_unsafe(
        (uint8_t *)FW_CHUNK,
        SPI_FLASH_FIRMWARE_ADDR_START + chunk_idx * FW_CHUNK_SIZE, chunk_pos);
    memzero(FW_CHUNK, sizeof(FW_CHUNK));
    w25qxx_read_bytes((uint8_t *)FW_CHUNK,
                      SPI_FLASH_FIRMWARE_ADDR_START + chunk_idx * FW_CHUNK_SIZE,
                      chunk_pos);
    memzero(FW_CHUNK, sizeof(FW_CHUNK));
  }
#endif

  // all done
  if (flash_len == flash_pos) {
    // check remaining chunks if any
    for (uint32_t i = chunk_idx + 1; i < 16; i++) {
      // hash should be empty if the chunk is unused
      if (!mem_is_empty(hdr->hashes + 32 * i, 32)) {
        flash_state = STATE_END;
        show_halt("Error installing", "firmware.");
        return;
      }
    }
  }

  memzero(FW_CHUNK, sizeof(FW_CHUNK));
  chunk_idx++;
}

// read protobuf integer and advance pointer
static secbool readprotobufint(const uint8_t **ptr, uint32_t *result) {
  *result = 0;

  for (int i = 0; i <= 3; ++i) {
    *result += (**ptr & 0x7F) << (7 * i);
    if ((**ptr & 0x80) == 0) {
      (*ptr)++;
      return sectrue;
    }
    (*ptr)++;
  }

  if (**ptr & 0xF0) {
    // result does not fit into uint32_t
    *result = 0;

    // skip over the rest of the integer
    while (**ptr & 0x80) (*ptr)++;
    (*ptr)++;
    return secfalse;
  }

  *result += (uint32_t)(**ptr) << 28;
  (*ptr)++;
  return sectrue;
}

/** Reverse-endian version comparison
 *
 * Versions are loaded from the header via a packed struct image_header. A
 * version is represented as a single uint32_t. Arm is natively little-endian,
 * but the version is actually stored as four bytes in major-minor-patch-build
 * order. This function implements `cmp` with "lowest" byte first.
 */
static int version_compare(const uint32_t vera, const uint32_t verb) {
  int a, b;  // signed temp values so that we can safely return a signed result
  a = vera & 0xFF;
  b = verb & 0xFF;
  if (a != b) return a - b;
  a = (vera >> 8) & 0xFF;
  b = (verb >> 8) & 0xFF;
  if (a != b) return a - b;
  a = (vera >> 16) & 0xFF;
  b = (verb >> 16) & 0xFF;
  if (a != b) return a - b;
  a = (vera >> 24) & 0xFF;
  b = (verb >> 24) & 0xFF;
  return a - b;
}

int should_keep_storage(int old_was_signed, uint32_t fix_version_current) {
  // if the current firmware is unsigned, always erase storage
  if (SIG_OK != old_was_signed) return SIG_FAIL;

  const image_header *new_hdr = (const image_header *)FW_HEADER;
  // new header must be signed by v3 signmessage/verifymessage scheme
  if (SIG_OK != signatures_ok(new_hdr, NULL, sectrue)) return SIG_FAIL;
  // if the new header hashes don't match flash contents, erase storage
  if (SIG_OK != check_firmware_hashes(new_hdr)) return SIG_FAIL;

  // if the current fix_version is higher than the new one, erase storage
  if (version_compare(new_hdr->version, fix_version_current) < 0) {
    return SIG_FAIL;
  }

  return SIG_OK;
}

static void rx_callback(usbd_device *dev, uint8_t ep) {
  (void)ep;
  static uint16_t msg_id = 0xFFFF;
  static uint32_t w;
  static int wi;
  static int old_was_signed;
  static uint32_t fix_version_current = 0xffffffff;
  uint8_t *p_buf;

  (void)fix_version_current;
  p_buf = packet_buf;

  if (dev != NULL) {
    if (usbd_ep_read_packet(dev, ENDPOINT_ADDRESS_OUT, packet_buf, 64) != 64)
      return;
    host_channel = CHANNEL_USB;
    if (flash_state == STATE_INTERRPUPT) {
      flash_state = STATE_READY;
      flash_pos = 0;
    }
  } else {
    host_channel = CHANNEL_SLAVE;
  }

  if (flash_state == STATE_END) {
    return;
  }

  if (flash_state == STATE_READY || flash_state == STATE_OPEN ||
      flash_state == STATE_FLASHSTART || flash_state == STATE_CHECK ||
      flash_state == STATE_INTERRPUPT) {
    if (p_buf[0] != '?' || p_buf[1] != '#' ||
        p_buf[2] != '#') {  // invalid start - discard
      return;
    }
    // struct.unpack(">HL") => msg, size
    msg_id = (p_buf[3] << 8) + p_buf[4];
  }

  if (flash_state == STATE_READY || flash_state == STATE_OPEN) {
    if (msg_id == 0x0000) {  // Initialize message (id 0)
      send_msg_features(dev);
      flash_state = STATE_OPEN;
      return;
    }
    if (msg_id == 0x0037) {  // GetFeatures message (id 55)
      send_msg_features(dev);
      return;
    }
    if (msg_id == 0x0001) {  // Ping message (id 1)
      send_msg_success(dev);
      return;
    }
    if (msg_id == 0x0005) {  // WipeDevice message (id 5)
      layoutDialog(&bmp_icon_question, "Cancel", "Confirm", NULL,
                   "Do you really want to", "wipe the device?", NULL,
                   "All data will be lost.", NULL, NULL);
      bool but = waitButtonResponse(BTN_PIN_YES, default_oper_time);
      if (host_channel == CHANNEL_SLAVE) {
      } else {
        if (but) {
          erase_storage_code_progress();
          flash_state = STATE_END;
          show_unplug("Device", "successfully wiped.");
          send_msg_success(dev);

        } else {
          flash_state = STATE_END;
          show_unplug("Device wipe", "aborted.");
          send_msg_failure(dev, 4);  // Failure_ActionCancelled
          shutdown();
        }
      }
      return;
    }
    if (msg_id != 0x0006 && msg_id != 0x0010) {
      send_msg_failure(dev, 1);  // Failure_UnexpectedMessage
      return;
    }
  }

  if (flash_state == STATE_OPEN) {
    if (msg_id == 0x0006) {  // FirmwareErase message (id 6)
      bool proceed = false;
      if (firmware_present_new()) {
#if ONEKEY_MINI
        layoutDialog(&bmp_icon_warning, "CANCEL", "OK", NULL,
                     "Install firmware?", NULL, "Please ensure you've",
                     "backed up the", "recovery phrase", NULL);
#else
        layoutDialog(&bmp_icon_question, "Abort", "Continue", NULL,
                     "Install new", "firmware?", NULL, "Never do this without",
                     "your recovery card!", NULL);
#endif
        proceed = waitButtonResponse(BTN_PIN_YES, default_oper_time);
      } else {
        proceed = true;
      }
      if (proceed) {
        // check whether the current firmware is signed (old or new method)
        if (firmware_present_new()) {
          const image_header *hdr =
              (const image_header *)FLASH_PTR(FLASH_FWHEADER_START);
          // previous firmware was signed either v2 or v3 scheme
          old_was_signed =
              signatures_match(hdr, NULL) & check_firmware_hashes(hdr);
          fix_version_current = hdr->fix_version;
        } else {
          old_was_signed = SIG_FAIL;
          fix_version_current = 0xffffffff;
        }
#if ONEKEY_MINI
        erase_code_progress_ex();
#else
        erase_code_progress();
#endif
        send_msg_success(dev);
        flash_state = STATE_FLASHSTART;
        timer_out_set(timer_out_oper, timer1s * 5);
      } else {
        send_msg_failure(dev, 4);  // Failure_ActionCancelled
        flash_state = STATE_END;
        show_unplug("Firmware installation", "aborted.");
        shutdown();
      }
      return;
    }
#if !ONEKEY_MINI
    else if (msg_id == 0x0010) {  // FirmwareErase message (id 16)
      erase_ble_code_progress();
      send_msg_success(dev);
      flash_state = STATE_FLASHSTART;
      timer_out_set(timer_out_oper, timer1s * 5);
      return;
    }
#endif
    send_msg_failure(dev, 1);  // Failure_UnexpectedMessage
    return;
  }

  if (flash_state == STATE_FLASHSTART) {
    if (msg_id == 0x0000) {  // end resume state
      send_msg_features(dev);
      flash_state = STATE_OPEN;
      flash_pos = 0;
      return;
    } else if (msg_id == 0x0007) {  // FirmwareUpload message (id 7)
      if (p_buf[9] != 0x0a) {       // invalid contents
        send_msg_failure(dev, 9);   // Failure_ProcessError
        flash_state = STATE_END;
        show_halt("Error installing", "firmware.");
        return;
      }
      // read payload length
      const uint8_t *p = p_buf + 10;
      if (flash_pos) {
        uint32_t tmp_len;
        if (readprotobufint(&p, &tmp_len) != sectrue) {  // integer too large
          send_msg_failure(dev, 9);                      // Failure_ProcessError
          flash_state = STATE_END;
          show_halt("Firmware is", "too big.");
          return;
        }
        w = 0;
        wi = 0;
        while (p < p_buf + 64 && flash_pos < flash_len) {
          // assign byte to first byte of uint32_t w
          w = (w >> 8) | (((uint32_t)*p) << 24);
          wi++;
          if (wi == 4) {
            if (flash_pos < FLASH_FWHEADER_LEN) {
              FW_HEADER[flash_pos / 4] = w;
            } else {
              FW_CHUNK[(flash_pos % FW_CHUNK_SIZE) / 4] = w;
#if ONEKEY_MINI
              w25qxx_write_buffer_unsafe(
                  (uint8_t *)&w, SPI_FLASH_FIRMWARE_ADDR_START + flash_pos, 4);
#else
              flash_enter();
              if (UPDATE_ST == update_mode) {
                flash_program_word(FLASH_FWHEADER_START + flash_pos, w);
              } else {
                flash_program_word(FLASH_BLE_ADDR_START + flash_pos, w);
              }
              flash_exit();
#endif
            }
            flash_pos += 4;
            wi = 0;
            // finished the whole chunk
            if (flash_pos % FW_CHUNK_SIZE == 0) {
              check_and_write_chunk();
            }
          }
          p++;
        }
        flash_state = STATE_FLASHING;
        return;
      } else {
        if (readprotobufint(&p, &flash_len) != sectrue) {  // integer too large
          send_msg_failure(dev, 9);  // Failure_ProcessError
          flash_state = STATE_END;
          show_halt("Firmware is", "too big.");
          return;
        }
#if ONEKEY_MINI
        // check firmware magic
        if (memcmp(p, &FIRMWARE_MAGIC_NEW, 4) != 0) {
          send_msg_failure(dev, 9);  // Failure_ProcessError
          flash_state = STATE_END;
          show_halt("Wrong firmware", "header.");
          return;
        }
        uint8_t *p_version = (uint8_t *)(p + 24);
        if (p_version[0] < 0x36) {
          send_msg_failure(dev, 9);  // Failure_ProcessError
          flash_state = STATE_END;
          show_halt("Wrong firmware", "version.");
          return;
        }
        update_mode = UPDATE_ST;

#else
        // check firmware magic
        if ((memcmp(p, &FIRMWARE_MAGIC_NEW, 4) != 0) &&
            (memcmp(p, &FIRMWARE_MAGIC_BLE, 4) != 0)) {
          send_msg_failure(dev, 9);  // Failure_ProcessError
          flash_state = STATE_END;
          show_halt("Wrong firmware", "header.");
          return;
        }
        if (memcmp(p, &FIRMWARE_MAGIC_NEW, 4) == 0) {
          update_mode = UPDATE_ST;
        } else {
          update_mode = UPDATE_BLE;
        }

#endif

        if (flash_len <= FLASH_FWHEADER_LEN) {  // firmware is too small
          send_msg_failure(dev, 9);             // Failure_ProcessError
          flash_state = STATE_END;
          show_halt("Firmware is", "too small.");
          return;
        }
        if (UPDATE_ST == update_mode) {
          if (flash_len >
              FLASH_FWHEADER_LEN + FLASH_APP_LEN) {  // firmware is too big
            send_msg_failure(dev, 9);                // Failure_ProcessError
            flash_state = STATE_END;
            show_halt("Firmware is", "too big");
            return;
          }
        }
#if !ONEKEY_MINI
        else {
          if (flash_len >
              FLASH_FWHEADER_LEN + FLASH_BLE_MAX_LEN) {  // firmware is too big
            send_msg_failure(dev, 9);                    // Failure_ProcessError
            flash_state = STATE_END;
            show_halt("Firmware is", "too small.");
            return;
          }
        }
#endif
        memzero(FW_HEADER, sizeof(FW_HEADER));
        memzero(FW_CHUNK, sizeof(FW_CHUNK));
        flash_state = STATE_FLASHING;
        flash_pos = 0;
        chunk_idx = 0;
        w = 0;
        wi = 0;
        while (p < p_buf + 64) {
          // assign byte to first byte of uint32_t w
          w = (w >> 8) | (((uint32_t)*p) << 24);
          wi++;
          if (wi == 4) {
            FW_HEADER[flash_pos / 4] = w;
            flash_pos += 4;
            wi = 0;
          }
          p++;
        }
        return;
      }
    }
    send_msg_failure(dev, 1);  // Failure_UnexpectedMessage
    return;
  }
  if (flash_state == STATE_INTERRPUPT)
    if (msg_id == 0x0000) {
      send_msg_failure(dev, 9);  // Failure_ProcessError
      flash_state = STATE_FLASHSTART;
      timer_out_set(timer_out_oper, timer1s * 5);
      return;
    }

  if (flash_state == STATE_FLASHING) {
    if (p_buf[0] != '?') {       // invalid contents
      send_msg_failure(dev, 9);  // Failure_ProcessError
      flash_state = STATE_END;
      show_halt("Error installing", "firmware.");
      return;
    }
    timer_out_set(timer_out_oper, timer1s * 5);
    static uint8_t flash_anim = 0;
    if (flash_anim % 32 == 4) {
#if ONEKEY_MINI
      layoutProgress("INSTALLING ... Please\nwait",
#else
      layoutProgress("INSTALLING ... Please wait",
#endif
                     1000 * flash_pos / flash_len);
    }
    flash_anim++;

    const uint8_t *p = p_buf + 1;
    while (p < p_buf + 64 && flash_pos < flash_len) {
      // assign byte to first byte of uint32_t w
      w = (w >> 8) | (((uint32_t)*p) << 24);
      wi++;
      if (wi == 4) {
        if (flash_pos < FLASH_FWHEADER_LEN) {
          FW_HEADER[flash_pos / 4] = w;
        } else {
          FW_CHUNK[(flash_pos % FW_CHUNK_SIZE) / 4] = w;
#if !ONEKEY_MINI
          flash_enter();
          if (UPDATE_ST == update_mode) {
            flash_program_word(FLASH_FWHEADER_START + flash_pos, w);
          } else {
            flash_program_word(FLASH_BLE_ADDR_START + flash_pos, w);
          }
          flash_exit();
#endif
        }
        flash_pos += 4;
        wi = 0;
        // finished the whole chunk
        if (flash_pos % FW_CHUNK_SIZE == 0) {
          check_and_write_chunk();
        }
      }
      p++;
    }
    // flashing done
    if (flash_pos == flash_len) {
      // flush remaining data in the last chunk
      if (flash_pos % FW_CHUNK_SIZE > 0) {
        check_and_write_chunk();
      }
      flash_state = STATE_CHECK;
      if (UPDATE_ST == update_mode) {
        const image_header *hdr = (const image_header *)FW_HEADER;
        // allow only v3 signmessage/verifymessage signature for new FW
        if (SIG_OK != signatures_ok(hdr, NULL, sectrue)) {
          send_msg_buttonrequest_firmwarecheck(dev);
          return;
        }
      }
    } else {
      return;
    }
  }

  if (flash_state == STATE_CHECK) {
    timer_out_set(timer_out_oper, 0);
    if (UPDATE_ST == update_mode) {
      // use the firmware header from RAM
      const image_header *hdr = (const image_header *)FW_HEADER;

      bool hash_check_ok;
      // show fingerprint of unsigned firmware
      if (SIG_OK != signatures_ok(hdr, NULL, sectrue)) {
        if (msg_id != 0x001B) {  // ButtonAck message (id 27)
          return;
        }
#if ONEKEY_MINI
        hash_check_ok = false;
#else
        uint8_t hash[32] = {0};
        compute_firmware_fingerprint(hdr, hash);
        layoutFirmwareFingerprint(hash);
        hash_check_ok = waitButtonResponse(BTN_PIN_YES, default_oper_time);
#endif

      } else {
        hash_check_ok = true;
      }
#if ONEKEY_MINI
      layoutProgress("Programing ... Please\nwait", 1000);
#else
      layoutProgress("Programing ... Please wait", 1000);
#endif
#if !ONEKEY_MINI
      // wipe storage if:
      // 1) old firmware was unsigned or not present
      // 2) signatures are not OK
      // 3) hashes are not OK
      if (SIG_OK != should_keep_storage(old_was_signed, fix_version_current)) {
        // erase storage
        erase_storage();
        // check erasure
        uint8_t hash[32] = {0};
        sha256_Raw(FLASH_PTR(FLASH_STORAGE_START), FLASH_STORAGE_LEN, hash);
        if (memcmp(
                hash,
                "\x2d\x86\x4c\x0b\x78\x9a\x43\x21\x4e\xee\x85\x24\xd3\x18\x20"
                "\x75\x12\x5e\x5c\xa2\xcd\x52\x7f\x35\x82\xec\x87\xff\xd9\x40"
                "\x76\xbc",
                32) != 0) {
          send_msg_failure(dev, 9);  // Failure_ProcessError
          show_halt("Error installing", "firmware.");
          return;
        }
      }
#endif
#if ONEKEY_MINI
      (void)old_was_signed;
      if (hash_check_ok) {
        w25qxx_write_buffer_unsafe((uint8_t *)FW_HEADER,
                                   SPI_FLASH_FIRMWARE_ADDR_START,
                                   FLASH_FWHEADER_LEN);
        layoutProgress("Verifing ... Please\nwait", 500);
        update_from_spi_flash();
      }
#else
      flash_enter();
      // write firmware header only when hash was confirmed
      if (hash_check_ok) {
        for (size_t i = 0; i < FLASH_FWHEADER_LEN / sizeof(uint32_t); i++) {
          flash_program_word(FLASH_FWHEADER_START + i * sizeof(uint32_t),
                             FW_HEADER[i]);
        }
      } else {
        for (size_t i = 0; i < FLASH_FWHEADER_LEN / sizeof(uint32_t); i++) {
          flash_program_word(FLASH_FWHEADER_START + i * sizeof(uint32_t), 0);
        }
      }
      flash_exit();
#endif

      flash_state = STATE_END;
      if (hash_check_ok) {
        send_msg_success(dev);
        show_unplug("New firmware", "successfully installed.");
        shutdown();
      } else {
        layoutDialog(&bmp_icon_warning, NULL, NULL, NULL,
                     "Firmware installation", "aborted.", NULL,
                     "You need to repeat", "the procedure with",
                     "the correct firmware.");
        send_msg_failure(dev, 9);  // Failure_ProcessError
        delay_ms(1000);
        shutdown();
      }
      return;
    }
#if !ONEKEY_MINI
    else {
      flash_state = STATE_END;
      i2c_set_wait(false);
      send_msg_success(dev);
      layoutProgress("Updating ... Please wait", 1000);
      delay_ms(500);  // important!!! delay for nordic reset

      uint32_t fw_len = flash_len - FLASH_FWHEADER_LEN;
      bool update_status = false;
#if BLE_SWD_UPDATE
      update_status = bUBLE_UpdateBleFirmware(
          fw_len, FLASH_BLE_ADDR_START + FLASH_FWHEADER_LEN, ERASE_ALL);

#else
      uint8_t *p_init = (uint8_t *)FLASH_INIT_DATA_START;
      uint32_t init_data_len = p_init[0] + (p_init[1] << 8);
#if NORDIC_BLE_UPDATE
      update_status = updateBle(p_init + 4, init_data_len,
                                (uint8_t *)FLASH_BLE_FIRMWARE_START,
                                fw_len - FLASH_INIT_DATA_LEN);
#else
      (void)fw_len;
      (void)init_data_len;
      update_status = false;
#endif
#endif
      if (update_status == false) {
        layoutDialog(&bmp_icon_warning, NULL, NULL, NULL, "ble installation",
                     "aborted.", NULL, "You need to repeat",
                     "the procedure with", "the correct firmware.");
      } else {
        show_unplug("ble firmware", "successfully installed.");
      }
      delay_ms(1000);
      shutdown();
    }
#endif
  }
}
static void set_config(usbd_device *dev, uint16_t wValue) {
  (void)wValue;

  usbd_ep_setup(dev, ENDPOINT_ADDRESS_IN, USB_ENDPOINT_ATTR_INTERRUPT, 64, 0);
  usbd_ep_setup(dev, ENDPOINT_ADDRESS_OUT, USB_ENDPOINT_ATTR_INTERRUPT, 64,
                rx_callback);
}

static usbd_device *usbd_dev;
static uint8_t usbd_control_buffer[256] __attribute__((aligned(2)));

static const struct usb_device_capability_descriptor *capabilities_landing[] = {
    (const struct usb_device_capability_descriptor
         *)&webusb_platform_capability_descriptor_landing,
};

static const struct usb_device_capability_descriptor
    *capabilities_no_landing[] = {
        (const struct usb_device_capability_descriptor
             *)&webusb_platform_capability_descriptor_no_landing,
};

static const struct usb_bos_descriptor bos_descriptor_landing = {
    .bLength = USB_DT_BOS_SIZE,
    .bDescriptorType = USB_DT_BOS,
    .bNumDeviceCaps =
        sizeof(capabilities_landing) / sizeof(capabilities_landing[0]),
    .capabilities = capabilities_landing};

static const struct usb_bos_descriptor bos_descriptor_no_landing = {
    .bLength = USB_DT_BOS_SIZE,
    .bDescriptorType = USB_DT_BOS,
    .bNumDeviceCaps =
        sizeof(capabilities_no_landing) / sizeof(capabilities_no_landing[0]),
    .capabilities = capabilities_no_landing};

static void usbInit(bool firmware_present) {
  usbd_dev = usbd_init(&otgfs_usb_driver, &dev_descr, &config, usb_strings,
                       sizeof(usb_strings) / sizeof(const char *),
                       usbd_control_buffer, sizeof(usbd_control_buffer));
  usbd_register_set_config_callback(usbd_dev, set_config);
  usb21_setup(usbd_dev, firmware_present ? &bos_descriptor_no_landing
                                         : &bos_descriptor_landing);
  webusb_setup(usbd_dev, "onekey.so");
  winusb_setup(usbd_dev, USB_INTERFACE_INDEX_MAIN);
}

static void checkButtons(void) {
  static bool btn_left = false, btn_right = false, btn_final = false;
  if (btn_final) {
    return;
  }
#if ONEKEY_MINI
  uint16_t state = gpio_get(BTN_PORT, BTN_PIN_YES | BTN_PIN_NO);
#else
  uint16_t state = gpio_get(BTN_PORT, BTN_PIN_YES);
  state |= gpio_get(BTN_PORT_NO, BTN_PIN_NO);
#endif
  if ((btn_left == false) && (state & BTN_PIN_NO)) {
    btn_left = true;
    oledBox(0, 0, 3, 3, true);
    oledRefresh();
  }
  if ((btn_right == false) && (state & BTN_PIN_YES) != BTN_PIN_YES) {
    btn_right = true;
    oledBox(OLED_WIDTH - 4, 0, OLED_WIDTH - 1, 3, true);
    oledRefresh();
  }
  if (btn_left && btn_right) {
    btn_final = true;
  }
}

#if !ONEKEY_MINI
static void i2cSlavePoll(void) {
  volatile uint32_t total_len, len;
  if (i2c_recv_done) {
    while (1) {
      total_len = fifo_lockdata_len(&i2c_fifo_in);
      if (total_len == 0) break;
      len = total_len > 64 ? 64 : total_len;
      fifo_read_lock(&i2c_fifo_in, packet_buf, len);
      rx_callback(NULL, 0);
    }
    i2c_recv_done = false;
  }
}
#endif

void usbLoop(void) {
  bool firmware_present = firmware_present_new();
  usbInit(firmware_present);
  for (;;) {
    usbd_poll(usbd_dev);
#if !ONEKEY_MINI
    i2cSlavePoll();
#endif
    if (!firmware_present &&
        (flash_state == STATE_READY || flash_state == STATE_OPEN)) {
      checkButtons();
    }
    if (flash_state == STATE_FLASHSTART || flash_state == STATE_FLASHING) {
      if (checkButtonOrTimeout(BTN_PIN_NO, timer_out_oper)) {
        flash_state = STATE_INTERRPUPT;
        fifo_flush(&i2c_fifo_in);
        layoutRefreshSet(true);
      }
    }
    if (flash_state == STATE_READY || flash_state == STATE_OPEN ||
        flash_state == STATE_INTERRPUPT)
      layoutBootHome();
  }
}

#if ONEKEY_MINI
void update_from_spi_flash(void) {
  image_header hdr = {0};
  w25qxx_read_bytes((uint8_t *)&hdr, SPI_FLASH_FIRMWARE_ADDR_START,
                    sizeof(image_header));
  if (hdr.magic != FIRMWARE_MAGIC_NEW) return;
  if (hdr.codelen > FLASH_APP_LEN) return;
  if (hdr.codelen < 4096) return;
  // version must be greater than 0x36
  if ((hdr.onekey_version & 0xff) < 0x36) return;

  int signed_firmware = signatures_match(&hdr, NULL);
  if (SIG_OK != signed_firmware) {
    return;
  }

  if (SIG_OK != check_firmware_hashes_ex(&hdr)) {
    return;
  }
  erase_code_progress();

  uint32_t total_len = hdr.codelen;
  uint32_t offset = FLASH_FWHEADER_LEN, packet_len = 0;

  flash_enter();

  while (total_len) {
    layoutProgress("Programing ... Please\nwait",
                   1000 * offset / (FLASH_FWHEADER_LEN + hdr.codelen));
    packet_len = total_len > FW_CHUNK_SIZE ? FW_CHUNK_SIZE : total_len;
    w25qxx_read_bytes((uint8_t *)FW_CHUNK,
                      SPI_FLASH_FIRMWARE_ADDR_START + offset, FW_CHUNK_SIZE);
    flash_program(FLASH_FWHEADER_START + offset, (uint8_t *)FW_CHUNK,
                  packet_len);
    total_len -= packet_len;
    offset += packet_len;
  }

  w25qxx_read_bytes((uint8_t *)FW_CHUNK, SPI_FLASH_FIRMWARE_ADDR_START,
                    FLASH_FWHEADER_LEN);
  flash_program(FLASH_FWHEADER_START, (uint8_t *)FW_CHUNK, FLASH_FWHEADER_LEN);

  flash_exit();

  w25qxx_erase_block((SPI_FLASH_FIRMWARE_ADDR_START / SPI_FLASH_BLOCK_SIZE));
}
#endif
