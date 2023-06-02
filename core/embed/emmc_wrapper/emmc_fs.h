#ifndef _EMMC_FS_H_
#define _EMMC_FS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emmc.h"
#include "ff.h"

// emmc wrapper status
typedef struct
{
    bool is_inited;
    bool is_mounted_onekey_data;
    FATFS fs_obj_onekeyData;
    bool is_mounted_user_data;
    FATFS fs_obj_userData;
    char from_function[64];
    char ff_call[128];
    FRESULT ff_result;
    char debug_extra[512];
} EMMC_WRAPPER_STATUS;
extern EMMC_WRAPPER_STATUS emmc_wrapper_status;

// emmc partition table
extern PARTITION VolToPart[FF_VOLUMES];

// emmc fs object
extern FATFS fs_obj_onekeyData;
extern FATFS fs_obj_userData;

// expanded version of FILINFO
typedef struct
{
    // exists
    bool path_exist;
// size
#if FF_FS_EXFAT
    uint64_t fsize;
#else
    uint32_t size;
#endif
    // last modified date
    struct
    {
        uint16_t year : 13; // Year (1-9999)
        uint8_t month : 4;  // Month (1..12)
        uint8_t day   : 5;  // Day (1..31)
    } date;
    // last modified time
    struct
    {
        uint8_t hour   : 5; // Hour (0..23)
        uint8_t minute : 6; // Minute (0..59)
        uint8_t second : 5; // Second / 2 (0..29)
    } time;
    // attribute
    struct
    {
        bool readonly;
        bool hidden;
        bool system;
        bool archive;
        bool directory;
    } attrib;
} EMMC_PATH_INFO;

// macros
#define OKEMMC_DBG_STATUS_SETUP()                                 \
  {                                                               \
    strncpy(emmc_wrapper_status.from_function, __FUNCTION__, 64); \
    emmc_wrapper_status.ff_call[0] = '\0';                        \
  }

#define ExecuteCheck_OKEMMC_ADVANCED(func_call, expected_condition, on_false) \
  {                                                                           \
    strncpy(emmc_wrapper_status.ff_call, #func_call, 128);                    \
    FRESULT EC_res = emmc_wrapper_status.ff_result = (func_call);             \
    if ( !(expected_condition) )                                              \
    {                                                                         \
      on_false                                                                \
    }                                                                         \
  }
#define ExecuteCheck_OKEMMC_SIMPLE(func_call)                                      \
  {                                                                                \
    ExecuteCheck_OKEMMC_ADVANCED(func_call, (EC_res == FR_OK), { return false; }); \
  }

#define ensure_emmcfs(expr, msg)                                                                            \
  {                                                                                                         \
    if ( !(expr) )                                                                                          \
    {                                                                                                       \
      char gg[512];                                                                                         \
      sprintf(                                                                                              \
          gg, "msg=%s, f_func=%s, ff_call=%s, ff_ret=%x, extra=%s", msg, emmc_wrapper_status.from_function, \
          emmc_wrapper_status.ff_call, emmc_wrapper_status.ff_result, emmc_wrapper_status.debug_extra       \
      );                                                                                                    \
                                                                                                            \
      __fatal_error(#expr, gg, __FILE__, __LINE__, __func__);                                               \
    }                                                                                                       \
  }

// functions
void emmc_fs_dbgex_set(const char* msg);
void emmc_fs_dbgex_append(const char* msg);
void emmc_fs_dbgex_clr();
void emmc_fs_format_status(char* str, size_t str_len);
bool emmc_fs_init();
bool emmc_fs_recreate(bool partition_table, bool onekey_data, bool user_data);
bool emmc_fs_is_partitioned();
bool emmc_fs_mount(bool onekey_data, bool user_data);
bool emmc_fs_unmount(bool onekey_data, bool user_data);
bool emmc_fs_path_exist(char* path_buff);
bool emmc_fs_path_info(char* path_buff, EMMC_PATH_INFO* file_info);

// bool emmc_fs_fix_permission_internal(char* path_buff);
bool emmc_fs_fix_permission(bool onekey_data, bool user_data);

bool emmc_fs_file_read(
    char* path_buff, uint32_t offset, void* buff, uint32_t target_len, uint32_t* processed_len
);
bool emmc_fs_file_write(
    char* path_buff, uint32_t offset, void* buff, uint32_t target_len, uint32_t* processed_len,
    bool overwrite, bool append
);
bool emmc_fs_file_delete(char* path_buff);

// bool emmc_fs_dir_list_internal(
//     char* path_buff, char* list_subdirs_buff, uint32_t list_subdirs_len, char* list_files_buff,
//     uint32_t list_files_len
// );
bool emmc_fs_dir_list(
    char* path_buff, char* list_subdirs_buff, uint32_t list_subdirs_len, char* list_files_buff,
    uint32_t list_files_len
);

bool emmc_fs_dir_make(char* path_buff);
// bool emmc_fs_dir_delete_internal(char* path_buff);
bool emmc_fs_dir_delete(char* path_buff);

#endif //_EMMC_FS_H_
