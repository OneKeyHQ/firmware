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

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include <pb.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "messages.pb.h"

#include "image.h"
#include "secbool.h"

#define USB_TIMEOUT 500
#define USB_PACKET_SIZE 64

#define FIRMWARE_UPLOAD_CHUNK_RETRY_COUNT 2

void send_failure(uint8_t iface_num, FailureType type, const char *text);
void send_success(uint8_t iface_num, const char *text);

void send_user_abort(uint8_t iface_num, const char *msg);

secbool msg_parse_header(const uint8_t *buf, uint16_t *msg_id,
                         uint32_t *msg_size);

void process_msg_Initialize(uint8_t iface_num, uint32_t msg_size, uint8_t *buf,
                            const vendor_header *const vhdr,
                            const image_header *const hdr);
void process_msg_GetFeatures(uint8_t iface_num, uint32_t msg_size, uint8_t *buf,
                             const vendor_header *const vhdr,
                             const image_header *const hdr);
void process_msg_Ping(uint8_t iface_num, uint32_t msg_size, uint8_t *buf);
void process_msg_Reboot(uint8_t iface_num, uint32_t msg_size, uint8_t *buf);
void process_msg_FirmwareErase(uint8_t iface_num, uint32_t msg_size,
                               uint8_t *buf);
int process_msg_FirmwareUpload(uint8_t iface_num, uint32_t msg_size,
                               uint8_t *buf);
int process_msg_WipeDevice(uint8_t iface_num, uint32_t msg_size, uint8_t *buf);

void process_msg_DeviceInfoSettings(uint8_t iface_num, uint32_t msg_size,
                                    uint8_t *buf);
void process_msg_GetDeviceInfo(uint8_t iface_num, uint32_t msg_size,
                               uint8_t *buf);
void process_msg_ReadSEPublicKey(uint8_t iface_num, uint32_t msg_size,
                                 uint8_t *buf);
void process_msg_WriteSEPublicCert(uint8_t iface_num, uint32_t msg_size,
                                   uint8_t *buf);
void process_msg_ReadSEPublicCert(uint8_t iface_num, uint32_t msg_size,
                                  uint8_t *buf);
void process_msg_SESignMessage(uint8_t iface_num, uint32_t msg_size,
                               uint8_t *buf);

void process_msg_FirmwareEraseBLE(uint8_t iface_num, uint32_t msg_size,
                                  uint8_t *buf);

void process_msg_unknown(uint8_t iface_num, uint32_t msg_size, uint8_t *buf);

#endif
