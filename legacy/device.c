#include "device.h"
#include "memory.h"
#include "otp.h"
#include "util.h"

static DeviceSerialNo device_serial_obj;
static DeviceSerialNo *device_serial = &device_serial_obj;
static bool serial_set = false;
static bool factory_mode = false;

void device_init(void) {
  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    serial_set = false;
    return;
  }
  memcpy(&device_serial_obj,
         FLASH_PTR(FLASH_OTP_BASE +
                   FLASH_OTP_DEVICE_SERIAL * FLASH_OTP_BLOCK_SIZE),
         sizeof(DeviceSerialNo));
  serial_set = true;
  return;
}

bool device_serial_set(void) { return serial_set; }

void device_set_factory_mode(bool mode) { factory_mode = mode; }

bool device_is_factory_mode(void) { return factory_mode; }

bool device_set_info(DeviceSerialNo *dev_serial) {
  uint8_t buffer[FLASH_OTP_BLOCK_SIZE] = {0};

  if (serial_set) {
    return false;
  }
  // check serial
  if (!flash_otp_is_locked(FLASH_OTP_DEVICE_SERIAL)) {
    if (check_all_ones(device_serial->product, FLASH_OTP_BLOCK_SIZE)) {
      memcpy(buffer, dev_serial, sizeof(DeviceSerialNo));
      flash_otp_write(FLASH_OTP_DEVICE_SERIAL, 0, buffer, FLASH_OTP_BLOCK_SIZE);
      flash_otp_lock(FLASH_OTP_DEVICE_SERIAL);
      device_init();
      return true;
    }
  }
  return false;
}

bool device_get_serial(DeviceSerialNo **serial) {
  if (!serial_set) {
    return false;
  }
  *serial = device_serial;
  return true;
}
