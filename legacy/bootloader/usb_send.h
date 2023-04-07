static void send_response(usbd_device *dev, uint8_t *buf) {
  if (dev != NULL) {
    while (usbd_ep_write_packet(dev, ENDPOINT_ADDRESS_IN, buf, 64) != 64) {
    }
  }
#if !ONEKEY_MINI
  else {
    memcpy(i2c_data_out, buf, 64);
    i2c_slave_send_ex(64);
  }
#endif
}
static void send_msg_success(usbd_device *dev) {
  uint8_t response[64];
  memzero(response, sizeof(response));
  // response: Success message (id 2), payload len 0
  memcpy(response,
         // header
         "?##"
         // msg_id
         "\x00\x02"
         // msg_size
         "\x00\x00\x00\x00",
         9);
  send_response(dev, response);
}

static void send_msg_failure(usbd_device *dev, uint8_t code) {
  uint8_t response[64];
  memzero(response, sizeof(response));
  // response: Failure message (id 3), payload len 2
  memcpy(response,
         // header
         "?##"
         // msg_id
         "\x00\x03"
         // msg_size
         "\x00\x00\x00\x02"
         // code field id
         "\x08",
         10);
  // assign code value
  response[10] = code;
  send_response(dev, response);
}

#if ONEKEY_MINI

#include "device.h"
#include "messages-management.pb.h"
#include "messages.h"
#include "messages.pb.h"
#include "w25qxx.h"

#define PROTOC_TYPE_VARINT 0
#define PROTOC_TYPE_BYTES 2

static uint8_t getprotobuftag(uint8_t *ptr, uint32_t value, uint8_t tag_type) {
  uint8_t i = 0;

  *ptr = (value & 0x0F) << 3;
  *ptr |= (0x80 | tag_type);

  i = 1;

  value >>= 4;

  while (value) {
    ptr[i] = (uint8_t)((value & 0x7F) | 0x80);
    value >>= 7;
    i++;
  }
  ptr[i - 1] &= 0x7F; /* Unset top bit on last byte */
  return i;
}

extern const char *cpu_info;

void send_msg_features(usbd_device *dev) {
  static uint8_t response[MSG_OUT_SIZE];
  uint8_t *p = response;
  uint32_t len = 0;

  Features *resp = (Features *)response;
  memzero(response, sizeof(response));

  // header
  memcpy(p, "?##", 3);
  p += 3;

  // msg_id
  memcpy(p, "\x00\x11", 2);
  p += 2;

  // msg_size
  p += 4;

  // vendor
  memcpy(p,
         "\x0a"
         "\x09"
         "onekey.so",
         11);
  p += 11;

  // version
  *p++ = 0x10;
  *p++ = VERSION_MAJOR;
  *p++ = 0x18;
  *p++ = VERSION_MINOR;
  *p++ = 0x20;
  *p++ = VERSION_PATCH;

  // bootloader_mode
  memcpy(p, "\x28\x01", 2);
  p += 2;

  // firmware_present
  len = getprotobuftag(p, Features_firmware_present_tag, PROTOC_TYPE_VARINT);
  p += len;

  *p = firmware_present_new() ? true : false;
  p++;

  char *serial;
  if (device_get_serial(&serial)) {
    len = getprotobuftag(p, Features_serial_no_tag, PROTOC_TYPE_BYTES);
    p += len;

    strlcpy((char *)p + 1, serial, sizeof(resp->serial_no));
    *p = strlen((char *)p + 1);
    p += *p + 1;
  }

  char *spi_info = w25qxx_get_desc();
  len = getprotobuftag(p, Features_spi_flash_tag, PROTOC_TYPE_BYTES);
  p += len;
  strlcpy((char *)p + 1, spi_info, sizeof(resp->spi_flash));
  *p = strlen((char *)p + 1);
  p += *p + 1;

  char *se_info = device_get_se_config_version();
  len = getprotobuftag(p, Features_se_ver_tag, PROTOC_TYPE_BYTES);
  p += len;
  strlcpy((char *)p + 1, se_info, sizeof(resp->se_ver));
  *p = strlen((char *)p + 1);
  p += *p + 1;

  len = getprotobuftag(p, Features_cpu_info_tag, PROTOC_TYPE_BYTES);
  p += len;
  strlcpy((char *)p + 1, cpu_info, sizeof(resp->cpu_info));
  *p = strlen((char *)p + 1);
  p += *p + 1;

  len = getprotobuftag(p, Features_pre_firmware_tag, PROTOC_TYPE_BYTES);
  p += len;
  strlcpy((char *)p + 1, ONEKEY_VERSION, sizeof(resp->pre_firmware));
  *p = strlen((char *)p + 1);
  p += *p + 1;

  len = p - response;

  response[5] = ((len - 9) >> 24) & 0xFF;
  response[6] = ((len - 9) >> 16) & 0xFF;
  response[7] = ((len - 9) >> 8) & 0xFF;
  response[8] = (len - 9) & 0xFF;

  const uint8_t *data = response;
  uint8_t packet_num = 0;
  uint8_t packet_len = 0;
  while (len) {
    if (packet_num == 0) {
      send_response(dev, (uint8_t *)data);
      len -= 64;
      data += 64;
    } else {
      memzero(packet_buf, sizeof(packet_buf));
      packet_buf[0] = '?';
      packet_len = len > 63 ? 63 : len;
      memcpy(packet_buf + 1, data, packet_len);
      data += packet_len;
      len -= packet_len;
      send_response(dev, packet_buf);
    }
    packet_num++;
  }
}
#else
static void send_msg_features(usbd_device *dev) {
  uint8_t response[64];
  memzero(response, sizeof(response));

  // response: Features message (id 17), payload len 26
  //           - vendor = "onekey.so"
  //           - major_version = VERSION_MAJOR
  //           - minor_version = VERSION_MINOR
  //           - patch_version = VERSION_PATCH
  //           - bootloader_mode = True
  //           - firmware_present = True/False
  //           - model = "1"
  //           ? fw_version_major = version_major
  //           ? fw_version_minor = version_minor
  //           ? fw_version_patch = version_patch
  const bool firmware_present = firmware_present_new();
  const image_header *current_hdr = (const image_header *)FLASH_FWHEADER_START;
  uint32_t version = firmware_present ? current_hdr->version : 0;

  // clang-format off
  const uint8_t feature_bytes[] = {
    0x0a,  // vendor field
    0x09,  // vendor length
    'o', 'n', 'e', 'k', 'e', 'y', '.', 's', 'o',
    0x10, VERSION_MAJOR,
    0x18, VERSION_MINOR,
    0x20, VERSION_PATCH,
    0x28, 0x01, // bootloader_mode
    0x90, 0x01, // firmware_present field
    firmware_present ? 0x01 : 0x00,
    0xaa, 0x01, // model field
    0x01,      // model length
    '1',
  };

  const uint8_t version_bytes[] = {
    // fw_version_major
    0xb0, 0x01, version & 0xff,
    // fw_version_minor
    0xb8, 0x01, (version >> 8) & 0xff,
    // fw_version_patch
    0xc0, 0x01, (version >> 16) & 0xff,
  };

  uint8_t header_bytes[] = {
    // header
    '?', '#', '#',
    // msg_id
    0x00, 0x11,
    // msg_size
    0x00, 0x00, 0x00, sizeof(feature_bytes) + (firmware_present ? sizeof(version_bytes) : 0),
  };
  // clang-format on

  // Check that the response will fit into an USB packet, and also that the
  // sizeof expression above fits into a single byte
  _Static_assert(
      sizeof(feature_bytes) + sizeof(version_bytes) + sizeof(header_bytes) <=
          64,
      "Features response too long");

  memcpy(response, header_bytes, sizeof(header_bytes));
  memcpy(response + sizeof(header_bytes), feature_bytes, sizeof(feature_bytes));
  if (firmware_present) {
    memcpy(response + sizeof(header_bytes) + sizeof(feature_bytes),
           version_bytes, sizeof(version_bytes));
  }

  send_response(dev, response);
}
#endif

static void send_msg_buttonrequest_firmwarecheck(usbd_device *dev) {
  uint8_t response[64];
  memzero(response, sizeof(response));
  // response: ButtonRequest message (id 26), payload len 2
  //           - code = ButtonRequest_FirmwareCheck (9)
  memcpy(response,
         // header
         "?##"
         // msg_id
         "\x00\x1a"
         // msg_size
         "\x00\x00\x00\x02"
         // data
         "\x08"
         "\x09",
         11);
  send_response(dev, response);
}
