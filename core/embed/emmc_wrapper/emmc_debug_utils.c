#include "emmc_debug_utils.h"

void __debug_print(bool wait_click, const char* file, int line, const char* func, const char* fmt, ...)
{
    display_orientation(0);
    display_backlight(255);
    display_clear();
    display_refresh();
    display_print_color(RGB16(0x69, 0x69, 0x69), COLOR_BLACK);

    display_printf("\n");
    display_printf("=== Debug Info ===\n");

    display_printf("file: %s:%d\n", file, line);
    display_printf("func: %s\n", func);

    display_printf("message:\n");
    va_list va;
    va_start(va, fmt);
    char buf[256] = {0};
    int len = vsnprintf(buf, sizeof(buf), fmt, va);
    display_print(buf, len);
    va_end(va);
    display_printf("\n");

    display_text(8, 784, "Tap to continue ...", -1, FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);
    while ( wait_click && !touch_click() ) {}
}

bool buffer_to_hex_string(const void* buff, size_t buff_len, char* str, size_t str_len, size_t* processed)
{
    size_t byte_str_len = sizeof("xx ");

    if ( (buff_len * (byte_str_len - 1)) + 1 > str_len )
        return false;

    const uint8_t* u8_buff = buff;

    for ( size_t i = 0; i < buff_len; i++ )
    {
        char tmp_char[byte_str_len];
        snprintf(tmp_char, byte_str_len, "%02X ", u8_buff[i]);
        strncat(str, tmp_char, byte_str_len);
        *processed = i + 1;
    }

    return true;
}

// void __print_buffer(void* buff, size_t buff_len, )
// {
//     size_t byte_str_len = buff_len * 3 + 1;
//     char str[byte_str_len];
//     size_t processed = 0;
//     memzero(str, byte_str_len);
//     if ( buffer_to_hex_string(buff, buff_len, str, byte_str_len, &processed) )
//         dbgprintf("buffer=%s\nprocessed=%lu\nbyte_str_len=%lu", str, processed, byte_str_len);
//     else
//         dbgprintf("failed, processed=%lu\n", processed);
// }