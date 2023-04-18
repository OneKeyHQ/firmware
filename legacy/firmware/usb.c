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

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/usb/hid.h>
#include <libopencm3/usb/usbd.h>

#include "common.h"
#include "config.h"
#include "debug.h"
#include "messages.h"
#include "oled.h"
#include "random_delays.h"
#include "timer.h"
#include "trans_fifo.h"
#include "trezor.h"
#if U2F_ENABLED
#include "u2f.h"
#endif
#include "ble.h"
#include "layout2.h"
#include "memory.h"
#include "si2c.h"
#include "supervise.h"
#include "sys.h"
#include "usb.h"
#include "util.h"

#include "usb21_standard.h"
#include "webusb.h"
#include "winusb.h"

#define USB_INTERFACE_INDEX_MAIN 0
#if DEBUG_LINK
#define USB_INTERFACE_INDEX_DEBUG 1
#if U2F_ENABLED
#define USB_INTERFACE_INDEX_U2F 2
#define USB_INTERFACE_COUNT 3
#else
#define USB_INTERFACE_COUNT 2
#endif
#else
#if U2F_ENABLED
#define USB_INTERFACE_INDEX_U2F 1
#define USB_INTERFACE_COUNT 2
#else
#define USB_INTERFACE_COUNT 1
#endif
#endif

#define ENDPOINT_ADDRESS_MAIN_IN (0x81)
#define ENDPOINT_ADDRESS_MAIN_OUT (0x01)
#if DEBUG_LINK
#define ENDPOINT_ADDRESS_DEBUG_IN (0x82)
#define ENDPOINT_ADDRESS_DEBUG_OUT (0x02)
#endif
#if U2F_ENABLED
#define ENDPOINT_ADDRESS_U2F_IN (0x83)
#define ENDPOINT_ADDRESS_U2F_OUT (0x03)
#endif

#define USB_STRINGS                                 \
  X(MANUFACTURER, "ByteForge")                      \
  X(PRODUCT, "ONEKEY")                              \
  X(SERIAL_NUMBER, config_uuid_str)                 \
  X(INTERFACE_MAIN, "ONEKEY Interface")             \
  X(INTERFACE_DEBUG, "ONEKEY Debug Link Interface") \
  X(INTERFACE_U2F, "ONEKEY U2F Interface")

#define X(name, value) USB_STRING_##name,
enum {
  USB_STRING_LANGID_CODES,  // LANGID code array
  USB_STRINGS
};
#undef X

static uint8_t packet_buf[64] __attribute__((aligned(4)));
uint8_t s_ucPackAppRevBuf[64];
uint16_t s_usOffset;
#define X(name, value) value,
static const char *usb_strings[] = {USB_STRINGS};
#undef X

static struct usb_device_descriptor dev_descr = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0210,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = USB_PACKET_SIZE,
    .idVendor = 0x1209,
    .idProduct = 0x4F4B,
    .bcdDevice = 0x0100,
    .iManufacturer = USB_STRING_MANUFACTURER,
    .iProduct = USB_STRING_PRODUCT,
    .iSerialNumber = USB_STRING_SERIAL_NUMBER,
    .bNumConfigurations = 1,
};

#if U2F_ENABLED

static const uint8_t hid_report_descriptor_u2f[] = {
    0x06, 0xd0, 0xf1,  // USAGE_PAGE (FIDO Alliance)
    0x09, 0x01,        // USAGE (U2F HID Authenticator Device)
    0xa1, 0x01,        // COLLECTION (Application)
    0x09, 0x20,        // USAGE (Input Report Data)
    0x15, 0x00,        // LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,  // LOGICAL_MAXIMUM (255)
    0x75, 0x08,        // REPORT_SIZE (8)
    0x95, 0x40,        // REPORT_COUNT (64)
    0x81, 0x02,        // INPUT (Data,Var,Abs)
    0x09, 0x21,        // USAGE (Output Report Data)
    0x15, 0x00,        // LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,  // LOGICAL_MAXIMUM (255)
    0x75, 0x08,        // REPORT_SIZE (8)
    0x95, 0x40,        // REPORT_COUNT (64)
    0x91, 0x02,        // OUTPUT (Data,Var,Abs)
    0xc0               // END_COLLECTION
};

static const struct {
  struct usb_hid_descriptor hid_descriptor_u2f;
  struct {
    uint8_t bReportDescriptorType;
    uint16_t wDescriptorLength;
  } __attribute__((packed)) hid_report_u2f;
} __attribute__((packed))
hid_function_u2f = {.hid_descriptor_u2f =
                        {
                            .bLength = sizeof(hid_function_u2f),
                            .bDescriptorType = USB_DT_HID,
                            .bcdHID = 0x0111,
                            .bCountryCode = 0,
                            .bNumDescriptors = 1,
                        },
                    .hid_report_u2f = {
                        .bReportDescriptorType = USB_DT_REPORT,
                        .wDescriptorLength = sizeof(hid_report_descriptor_u2f),
                    }};

static const struct usb_endpoint_descriptor hid_endpoints_u2f[2] = {
    {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = ENDPOINT_ADDRESS_U2F_IN,
        .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
        .wMaxPacketSize = USB_PACKET_SIZE,
        .bInterval = 1,
    },
    {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = ENDPOINT_ADDRESS_U2F_OUT,
        .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
        .wMaxPacketSize = USB_PACKET_SIZE,
        .bInterval = 1,
    }};

static const struct usb_interface_descriptor hid_iface_u2f[] = {{
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = USB_INTERFACE_INDEX_U2F,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_HID,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface = USB_STRING_INTERFACE_U2F,
    .endpoint = hid_endpoints_u2f,
    .extra = &hid_function_u2f,
    .extralen = sizeof(hid_function_u2f),
}};

#endif

#if DEBUG_LINK

static const struct usb_endpoint_descriptor webusb_endpoints_debug[2] = {
    {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = ENDPOINT_ADDRESS_DEBUG_IN,
        .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
        .wMaxPacketSize = USB_PACKET_SIZE,
        .bInterval = 1,
    },
    {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = ENDPOINT_ADDRESS_DEBUG_OUT,
        .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
        .wMaxPacketSize = USB_PACKET_SIZE,
        .bInterval = 1,
    }};

static const struct usb_interface_descriptor webusb_iface_debug[] = {{
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = USB_INTERFACE_INDEX_DEBUG,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_VENDOR,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface = USB_STRING_INTERFACE_DEBUG,
    .endpoint = webusb_endpoints_debug,
    .extra = NULL,
    .extralen = 0,
}};

#endif

static const struct usb_endpoint_descriptor webusb_endpoints_main[2] = {
    {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = ENDPOINT_ADDRESS_MAIN_IN,
        .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
        .wMaxPacketSize = USB_PACKET_SIZE,
        .bInterval = 1,
    },
    {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = ENDPOINT_ADDRESS_MAIN_OUT,
        .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
        .wMaxPacketSize = USB_PACKET_SIZE,
        .bInterval = 1,
    }};

static const struct usb_interface_descriptor webusb_iface_main[] = {{
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = USB_INTERFACE_INDEX_MAIN,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_VENDOR,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface = USB_STRING_INTERFACE_MAIN,
    .endpoint = webusb_endpoints_main,
    .extra = NULL,
    .extralen = 0,
}};

// Windows are strict about interfaces appearing
// in correct order
static const struct usb_interface ifaces[] = {
    {
        .num_altsetting = 1,
        .altsetting = webusb_iface_main,
#if DEBUG_LINK
    },
    {
        .num_altsetting = 1,
        .altsetting = webusb_iface_debug,
#endif
#if U2F_ENABLED
    },
    {
        .num_altsetting = 1,
        .altsetting = hid_iface_u2f,
#endif
    }};

static const struct usb_config_descriptor config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = USB_INTERFACE_COUNT,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 0x32,
    .interface = ifaces,
};

static volatile char tiny = 0;

#if U2F_ENABLED

static enum usbd_request_return_codes hid_control_request(
    usbd_device *dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
    usbd_control_complete_callback *complete) {
  (void)complete;
  (void)dev;

  wait_random();

  if ((req->bmRequestType != 0x81) ||
      (req->bRequest != USB_REQ_GET_DESCRIPTOR) || (req->wValue != 0x2200))
    return 0;

  debugLog(0, "", "hid_control_request u2f");
  *buf = (uint8_t *)hid_report_descriptor_u2f;
  *len = MIN_8bits(*len, sizeof(hid_report_descriptor_u2f));
  return 1;
}

static void u2f_rx_callback(usbd_device *dev, uint8_t ep) {
  (void)ep;
  static CONFIDENTIAL uint8_t buf[USB_PACKET_SIZE] __attribute__((aligned(4)));

  debugLog(0, "", "u2f_rx_callback");
  if (usbd_ep_read_packet(dev, ENDPOINT_ADDRESS_U2F_OUT, buf, sizeof(buf)) !=
      USB_PACKET_SIZE)
    return;
  u2fhid_read(tiny, (const U2FHID_FRAME *)(void *)buf);
}

#endif

static void main_rx_callback(usbd_device *dev, uint8_t ep) {
  (void)ep;
  static CONFIDENTIAL uint8_t buf[64] __attribute__((aligned(4)));
  if (dev != NULL) {
    if (usbd_ep_read_packet(dev, ENDPOINT_ADDRESS_MAIN_OUT, buf, sizeof(buf)) !=
        USB_PACKET_SIZE)
      return;
    host_channel = CHANNEL_USB;
  } else {
    memcpy(buf, packet_buf, USB_PACKET_SIZE);
    host_channel = CHANNEL_SLAVE;
  }
  timer_sleep_start_reset();
  unregister_timer("poweroff");
  debugLog(0, "", "main_rx_callback");
  if (!tiny) {
    msg_read(buf, sizeof(buf));
  } else {
    msg_read_tiny(buf, sizeof(buf));
  }
}

#if DEBUG_LINK

static void debug_rx_callback(usbd_device *dev, uint8_t ep) {
  (void)ep;
  static uint8_t buf[USB_PACKET_SIZE] __attribute__((aligned(4)));
  if (usbd_ep_read_packet(dev, ENDPOINT_ADDRESS_DEBUG_OUT, buf, sizeof(buf)) !=
      USB_PACKET_SIZE)
    return;
  debugLog(0, "", "debug_rx_callback");
  if (!tiny) {
    msg_debug_read(buf, sizeof(buf));
  } else {
    msg_read_tiny(buf, sizeof(buf));
  }
}

#endif

static void set_config(usbd_device *dev, uint16_t wValue) {
  (void)wValue;

  usbd_ep_setup(dev, ENDPOINT_ADDRESS_MAIN_IN, USB_ENDPOINT_ATTR_INTERRUPT,
                USB_PACKET_SIZE, 0);
  usbd_ep_setup(dev, ENDPOINT_ADDRESS_MAIN_OUT, USB_ENDPOINT_ATTR_INTERRUPT,
                USB_PACKET_SIZE, main_rx_callback);
#if U2F_ENABLED
  usbd_ep_setup(dev, ENDPOINT_ADDRESS_U2F_IN, USB_ENDPOINT_ATTR_INTERRUPT,
                USB_PACKET_SIZE, 0);
  usbd_ep_setup(dev, ENDPOINT_ADDRESS_U2F_OUT, USB_ENDPOINT_ATTR_INTERRUPT,
                USB_PACKET_SIZE, u2f_rx_callback);
#endif
#if DEBUG_LINK
  usbd_ep_setup(dev, ENDPOINT_ADDRESS_DEBUG_IN, USB_ENDPOINT_ATTR_INTERRUPT,
                USB_PACKET_SIZE, 0);
  usbd_ep_setup(dev, ENDPOINT_ADDRESS_DEBUG_OUT, USB_ENDPOINT_ATTR_INTERRUPT,
                USB_PACKET_SIZE, debug_rx_callback);
#endif
#if U2F_ENABLED
  usbd_register_control_callback(
      dev, USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
      USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT, hid_control_request);
#endif
}

static usbd_device *usbd_dev = NULL;
static uint8_t usbd_control_buffer[256] __attribute__((aligned(2)));

static const struct usb_device_capability_descriptor *capabilities[] = {
    (const struct usb_device_capability_descriptor
         *)&webusb_platform_capability_descriptor_no_landing,
};

static const struct usb_bos_descriptor bos_descriptor = {
    .bLength = USB_DT_BOS_SIZE,
    .bDescriptorType = USB_DT_BOS,
    .bNumDeviceCaps = sizeof(capabilities) / sizeof(capabilities[0]),
    .capabilities = capabilities};

void usbInit(void) {
  bool trezor_comp_mode = false;
  if (!config_hasTrezorCompMode()) {
    config_setTrezorCompMode(true);
    trezor_comp_mode = true;
  } else {
    config_getTrezorCompMode(&trezor_comp_mode);
  }
  // dev_descr.idProduct = trezor_comp_mode ? 0x53c1 : 0x4F4B;
  if (trezor_comp_mode) {
    dev_descr.idProduct = 0x53c1;
  }
  usbd_dev = usbd_init(&otgfs_usb_driver, &dev_descr, &config, usb_strings,
                       sizeof(usb_strings) / sizeof(*usb_strings),
                       usbd_control_buffer, sizeof(usbd_control_buffer));
  usbd_register_set_config_callback(usbd_dev, set_config);
  usb21_setup(usbd_dev, &bos_descriptor);
  static const char *origin_url = "onekey.so";
  webusb_setup(usbd_dev, origin_url);
  // Debug link interface does not have WinUSB set;
  // if you really need debug link on windows, edit the descriptor in winusb.c
  winusb_setup(usbd_dev, USB_INTERFACE_INDEX_MAIN);
}

static void i2c_slave_poll(void) {
  uint32_t total_len, len;
  while ((total_len = fifo_lockdata_len(&i2c_fifo_in)) > 0) {
    memset(packet_buf, 0x00, sizeof(packet_buf));
    len = total_len > 64 ? 64 : total_len;
    fifo_read_lock(&i2c_fifo_in, packet_buf, len);
    main_rx_callback(NULL, 0);
  }
}

void usbPoll(void) {
  static const uint8_t *data;

  bool reset = false;

  static bool usb_status_bak = false;

  ble_update_poll();

  if (usb_connect_status && !usb_status_bak) {
    usb_status_bak = true;
    if (config_hasPin() && session_isUnlocked()) {
      reset = true;
    }
  } else if (!usb_connect_status && usb_status_bak) {
    usb_status_bak = false;
    if (config_hasPin() && session_isUnlocked()) {
      reset = true;
    }
  }
  if (reset) {
    svc_system_privileged();
    vector_table_t *ivt = (vector_table_t *)FLASH_PTR(FLASH_APP_START);
    __asm__ volatile("msr msp, %0" ::"r"(ivt->initial_sp_value));
    if (cpu_mode == UNPRIVILEGED) {
      mpu_config_firmware();
    }
    __asm__ volatile("b reset_handler");
  }

  i2c_slave_poll();
  if (usbd_dev == NULL) {
    return;
  }
  // poll read buffer
  usbd_poll(usbd_dev);
  // write pending data
  if (CHANNEL_USB == host_channel) {
    data = msg_out_data();
    if (data) {
      timer_out_set(timer_out_resp, timer1s / 2);
      while (usbd_ep_write_packet(usbd_dev, ENDPOINT_ADDRESS_MAIN_IN, data,
                                  USB_PACKET_SIZE) != USB_PACKET_SIZE) {
        if (timer_out_get(timer_out_resp) == 0) {
          clear_msg_out();

          RCC_AHB2RSTR |= RCC_AHB2RSTR_OTGFSRST;
          RCC_AHB2RSTR &= ~RCC_AHB2RSTR_OTGFSRST;
          usbInit();
          break;
        }
      }
    }
  }

#if U2F_ENABLED
  data = u2f_out_data();
  if (data) {
    while (usbd_ep_write_packet(usbd_dev, ENDPOINT_ADDRESS_U2F_IN, data,
                                USB_PACKET_SIZE) != USB_PACKET_SIZE) {
    }
  }
#endif

#if DEBUG_LINK
  // write pending debug data
  data = msg_debug_out_data();
  if (data) {
    while (usbd_ep_write_packet(usbd_dev, ENDPOINT_ADDRESS_DEBUG_IN, data,
                                USB_PACKET_SIZE) != USB_PACKET_SIZE) {
    }
  }
#endif
}

void usbReconnect(void) {
  if (usbd_dev != NULL) {
    usbd_disconnect(usbd_dev, 1);
    delay(120000);
    usbd_disconnect(usbd_dev, 0);
  }
}

char usbTiny(char set) {
  char old = tiny;
  tiny = set;
  return old;
}

void waitAndProcessUSBRequests(uint32_t millis) {
  uint32_t start = timer_ms();

  while ((timer_ms() - start) < millis) {
    if (usbd_dev != NULL) {
      usbd_poll(usbd_dev);
    }
    i2c_slave_poll();
  }
}

void usbFlush(uint32_t millis) {
  if (usbd_dev == NULL) {
    return;
  }

  static const uint8_t *data;
  data = msg_out_data();
  if (data) {
    while (usbd_ep_write_packet(usbd_dev, ENDPOINT_ADDRESS_MAIN_IN, data,
                                USB_PACKET_SIZE) != USB_PACKET_SIZE) {
    }
  }

  uint32_t start = timer_ms();

  while ((timer_ms() - start) < millis) {
    asm("nop");
  }
}
