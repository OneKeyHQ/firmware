#ifndef _EMMC_COMMANDS_H_
#define _EMMC_COMMANDS_H_

// ######## includes ########

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// utils
#include "blake2s.h"
#include "bootui.h"
#include "br_check.h"
#include "common.h"
#include "image.h"
#include "memzero.h"
#include "secbool.h"
#include "version.h"

// internal devices
#include "device.h"
#include "flash.h"

// external devices
#include "ble.h"
#include "nordic_dfu.h"
#include "sdram.h"
#include "spi.h"
#include "usb.h"

// protobuf
#include "messages.pb.h"
#include <pb.h>
#include <pb_decode.h>
#include <pb_encode.h>

// emmc_fs
#include "emmc_fs.h"

// emmc_debug
#include "emmc_debug_utils.h"

// ######## macros, defines, typedefs ########
// Note: Some macros has been moved to "emmc_commands_macros.h" and included in "emmc_commands.c"
// to avoid conflict with legacy "message.c/.h".

// FIRMWARE REALTED
#define BLE_INIT_DATA_LEN    512
#define FIRMWARE_HEADERS_LEN (sizeof(vendor_header) + sizeof(image_header))

// IO CONFIGS
#define IO_RETRY_MAX   3
#define IO_TIMEOUT     500
#define IO_PACKET_SIZE 64

// MSG CONFIGS
#define MSG_HEADER_LEN_FIRST      (3 + 2 + 4) // magic+id+size
#define MSG_HEADER_LEN_SUBSEQUENT 1

// SDRAM BUFFER
#define SDRAM_BOOLOADER_BUFFER_RECV_LEN (10 * 1024 * 1024)
#define SDRAM_BOOLOADER_BUFFER_SEND_LEN (10 * 1024 * 1024)
#define SDRAM_BOOLOADER_BUFFER_MISC_LEN (10 * 1024 * 1024)
// this is to make manage SDRAM_BOOLOADER_BUFFER easier
typedef struct __attribute__((packed))
{
    uint8_t recv_buff[SDRAM_BOOLOADER_BUFFER_RECV_LEN];
    uint8_t send_buff[SDRAM_BOOLOADER_BUFFER_SEND_LEN];
    uint8_t misc_buff[SDRAM_BOOLOADER_BUFFER_MISC_LEN];
} bootloader_buffer;

// MSG STATES
typedef struct
{
    uint8_t* buffer;          // buffer for read to OR write from, must by allocated before use
    size_t chunk_size;        // size of a single transfer trunk (IO_PACKET_SIZE)
    size_t chunk_processed;   // how many bytes are processed in this trunk (not include header)
    ChannelType host_channel; // communication channel (USB/SPI->BLE)
    uint8_t usb_iface_num;
} msg_istate;

typedef struct
{
    uint8_t* buffer;          // buffer for read to OR write from, must by allocated before use
    size_t chunk_size;        // size of a single transfer trunk (IO_PACKET_SIZE)
    size_t chunk_processed;   // how many bytes are processed in this trunk (not include header)
    uint8_t chunk_index;      // use to decide what header to use / skip
    uint16_t msg_id;          // message ID
    uint32_t msg_size;        // encoded message size
    ChannelType host_channel; // communication channel (USB/SPI->BLE)
    uint8_t usb_iface_num;
} msg_ostate;

// NANOPB CALLBACK HELPER
typedef struct
{
    uint8_t* buffer;     // buffer address
    size_t buffer_size;  // buffer maximum size
    size_t payload_size; // actuall data size needs to be processed
} nanopb_callback_args;

// UTILS
#define EMMC_WRAPPER_UNUSED(x) (void)(x)
#define EMMC_WRAPPER_FORCE_IGNORE(x)               \
  {                                                \
    __typeof__(x) __attribute__((unused)) d = (x); \
  }

#define ExecuteCheck_MSGS_ADV_RETRY_DELAY(func_call, expected_result, on_false, retry_allowed, delay_ms) \
  assert(retry_allowed >= 0);                                                                            \
  {                                                                                                      \
    char str_func_call[] = #func_call;                                                                   \
    EMMC_WRAPPER_UNUSED(str_func_call);                                                                  \
    uint16_t attempted = 0;                                                                              \
    while ( true )                                                                                       \
    {                                                                                                    \
      if ( (func_call) == (expected_result) )                                                            \
        break;                                                                                           \
      hal_delay(delay_ms);                                                                               \
      attempted++;                                                                                       \
      if ( attempted > retry_allowed )                                                                   \
      {                                                                                                  \
        on_false;                                                                                        \
        break;                                                                                           \
      }                                                                                                  \
    }                                                                                                    \
  }

#define ExecuteCheck_MSGS_ADV(func_call, expected_result, on_false) \
  ExecuteCheck_MSGS_ADV_RETRY_DELAY(func_call, expected_result, on_false, 0, 0)

#define ExecuteCheck_MSGS_ADV_old(func_call, expected_result, on_false) \
  {                                                                     \
    char str_func_call[] = #func_call;                                  \
    EMMC_WRAPPER_UNUSED(str_func_call);                                 \
    if ( (func_call) != (expected_result) )                             \
    {                                                                   \
      on_false                                                          \
    }                                                                   \
  }

// ######## global vars ########

extern bool ui_progress_bar_visible;

// ######## functions ########

void ui_progress_bar_visible_clear();

// void packet_generate_first(
//     uint16_t msg_id, uint32_t msg_size, uint8_t* desc_buff, size_t desc_buff_offset, const uint8_t*
//     src_buff, size_t data_len
// );
// void packet_generate_subsequent(
//     uint8_t* desc_buff, size_t desc_buff_offset, const uint8_t* src_buff, size_t data_len
// );
// bool packet_process(
//     uint8_t* desc_buff, const uint8_t* src_buff, size_t buff_len, size_t msg_len, size_t* outside_counter
// );

// bool callback_encode_bytes(pb_ostream_t* stream, const pb_field_iter_t* field, void* const* arg);
// bool callback_decode_bytes(pb_istream_t* stream, const pb_field_iter_t* field, void** arg);

#ifdef ASYNC_PARSE_NOT_USED
// this is just a cleanner way to keep code here
bool recv_msg_sync_parse(
    uint8_t iface_num, uint32_t msg_size, uint8_t* buf, const pb_msgdesc_t* fields, void* msg
);
#endif
bool recv_msg_async_parse(
    uint8_t iface_num, uint32_t msg_size, uint8_t* buf, const pb_msgdesc_t* fields, void* msg
);
bool send_msg(
    uint8_t iface_num, uint16_t msg_id, const pb_msgdesc_t* fields, const void* msg, bool ignore_check
);

// void send_success(uint8_t iface_num, const char* text);
void send_success_nocheck(uint8_t iface_num, const char* text);
// void send_failure(uint8_t iface_num, FailureType type, const char* text);
void send_failure_nocheck(uint8_t iface_num, FailureType type, const char* text);
void send_failure_detailed(uint8_t iface_num, FailureType type, const char* fmt, ...);
// void send_user_abort(uint8_t iface_num, const char* msg);
void send_user_abort_nocheck(uint8_t iface_num, const char* msg);

// int version_compare(uint32_t vera, uint32_t verb)
// secbool check_image_contents_ram(
//     const image_header* const hdr, const uint8_t* const buffer, size_t code_offset, size_t blocks
// );

int process_msg_FirmwareUpdateEmmc(uint8_t iface_num, uint32_t msg_size, uint8_t* buf);
int process_msg_EmmcFixPermission(uint8_t iface_num, uint32_t msg_size, uint8_t* buf);
int process_msg_EmmcPathInfo(uint8_t iface_num, uint32_t msg_size, uint8_t* buf);
int process_msg_EmmcFileRead(uint8_t iface_num, uint32_t msg_size, uint8_t* buf);
int process_msg_EmmcFileWrite(uint8_t iface_num, uint32_t msg_size, uint8_t* buf);
int process_msg_EmmcFileDelete(uint8_t iface_num, uint32_t msg_size, uint8_t* buf);
int process_msg_EmmcDirList(uint8_t iface_num, uint32_t msg_size, uint8_t* buf);
int process_msg_EmmcDirMake(uint8_t iface_num, uint32_t msg_size, uint8_t* buf);
int process_msg_EmmcDirRemove(uint8_t iface_num, uint32_t msg_size, uint8_t* buf);

#endif //_EMMC_COMMANDS_H_