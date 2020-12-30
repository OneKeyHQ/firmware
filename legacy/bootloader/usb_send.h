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

static void send_response(usbd_device *dev, uint8_t *buf) {
  if (dev != NULL) {
    while (usbd_ep_write_packet(dev, ENDPOINT_ADDRESS_IN, buf, 64) != 64) {
    }
  } else {
    memcpy(i2c_data_out, buf, 64);
    i2c_slave_send_ex(64);
  }
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

static void send_msg_features(usbd_device *dev) {
  uint8_t response[64];
  uint8_t len;
  memzero(response, sizeof(response));
  len = getintprotobuf(response + 37, flash_pos);
  // response: Features message (id 17), payload len 26
  //           - vendor = "trezor.io"
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
         "trezor.io"
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
