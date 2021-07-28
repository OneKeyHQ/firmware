#if !ONEKEY_MINI
static uint8_t getintprotobuf(uint8_t *ptr, uint32_t value) {
  uint8_t i = 0;

  if (value <= 0x7F) {
    *ptr = value;
    return 1;
  }

  while (value) {
    ptr[i] = (uint8_t)((value & 0x7F) | 0x80);
    value >>= 7;
    i++;
  }
  ptr[i - 1] &= 0x7F; /* Unset top bit on last byte */
  return i;
}
#endif

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
  while (usbd_ep_write_packet(dev, ENDPOINT_ADDRESS_IN, response, 64) != 64) {
  }
}

#if ONEKEY_MINI
#include "device.h"
#include "messages-management.pb.h"
#include "messages.h"
#include "messages.pb.h"
#include "w25qxx.h"

void send_msg_features(usbd_device *dev) {
  static uint8_t response[MSG_OUT_SIZE];

  Features *resp = (Features *)response;
  memzero(response, sizeof(response));

  resp->has_vendor = true;
  strlcpy(resp->vendor, "onekey.so", sizeof(resp->vendor));
  resp->has_major_version = true;
  resp->major_version = VERSION_MAJOR;
  resp->has_minor_version = true;
  resp->minor_version = VERSION_MINOR;
  resp->has_patch_version = true;
  resp->patch_version = VERSION_PATCH;
  resp->has_bootloader_mode = true;
  resp->bootloader_mode = true;
  resp->has_firmware_present = true;
  resp->firmware_present = firmware_present_new() ? true : false;

  resp->has_factory_info = true;
  if (device_serial_set()) {
    DeviceSerialNo *serial;
    device_get_serial(&serial);
    strlcpy(resp->factory_info.product, serial->product,
            sizeof(resp->factory_info.product));
    strlcpy(resp->factory_info.hardware_id, serial->hardware,
            sizeof(resp->factory_info.hardware_id));
    strlcpy(resp->factory_info.shell_color, &serial->color,
            sizeof(resp->factory_info.shell_color));
    strlcpy(resp->factory_info.factory_id, serial->factory,
            sizeof(resp->factory_info.factory_id));
    strlcpy(resp->factory_info.utc, serial->utc,
            sizeof(resp->factory_info.utc));
    strlcpy(resp->factory_info.serial_no, serial->serial,
            sizeof(resp->factory_info.serial_no));
  }
  resp->has_spi_flash = true;
  strlcpy(resp->spi_flash, w25qxx_get_desc(), sizeof(resp->spi_flash));
  resp->has_se_ver = true;
  strlcpy(resp->se_ver, device_get_se_config_version(), sizeof(resp->se_ver));

  msg_write(MessageType_MessageType_Features, resp);

  const uint8_t *data;
  while ((data = msg_out_data())) {
    send_response(dev, (uint8_t *)data);
  }
}
#else
static void send_msg_features(usbd_device *dev) {
  uint8_t response[64];
  uint8_t len;
  memzero(response, sizeof(response));
  len = getintprotobuf(response + 37, flash_pos);
  // response: Features message (id 17), payload len 26
  //           - vendor = "onekey.so"
  //           - major_version = VERSION_MAJOR
  //           - minor_version = VERSION_MINOR
  //           - patch_version = VERSION_PATCH
  //           - bootloader_mode = True
  //           - firmware_present = True/False
  //           - model = "1"
  memcpy(response,
         // header
         "?##"
         // msg_id
         "\x00\x11"
         // msg_size
         "\x00\x00\x00\x1a"
         // data
         "\x0a"
         "\x09"
         "onekey.so"
         "\x10" VERSION_MAJOR_CHAR "\x18" VERSION_MINOR_CHAR
         "\x20" VERSION_PATCH_CHAR
         "\x28"
         "\x01"
         "\x90\x01"
         "\x00"
         "\xaa"
         "\x01\x01"
         "1"
         "\xa0\x1f",
         37);
  response[8] = 0x1c + len;
  response[30] = firmware_present_new() ? 0x01 : 0x00;
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
