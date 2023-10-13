#include "emmc_commands.h"
#include "emmc_commands_macros.h"

#include "se_thd89.h"
#include "thd89_boot.h"

// ######## global vars ########
// SDRAM BUFFER
bootloader_buffer* bl_buffer = (bootloader_buffer*)FMC_SDRAM_BOOLOADER_BUFFER_ADDRESS;
// UI progesss
bool ui_progress_bar_visible = false;

// ######## helpers ########

void ui_progress_bar_visible_clear()
{
    ui_progress_bar_visible = false;
}

static void packet_generate_first(
    uint16_t msg_id, uint32_t msg_size, uint8_t* desc_buff, size_t desc_buff_offset, const uint8_t* src_buff,
    size_t data_len
)
{
    // magic
    desc_buff[0] = (uint8_t)'?';
    desc_buff[1] = (uint8_t)'#';
    desc_buff[2] = (uint8_t)'#';
    // id
    desc_buff[3] = (msg_id >> 8) & 0xFF;
    desc_buff[4] = msg_id & 0xFF;
    // size
    desc_buff[5] = (msg_size >> 24) & 0xFF;
    desc_buff[6] = (msg_size >> 16) & 0xFF;
    desc_buff[7] = (msg_size >> 8) & 0xFF;
    desc_buff[8] = msg_size & 0xFF;
    // data
    memcpy((uint8_t*)desc_buff + MSG_HEADER_LEN_FIRST + desc_buff_offset, src_buff, data_len);
}

static void packet_generate_subsequent(
    uint8_t* desc_buff, size_t desc_buff_offset, const uint8_t* src_buff, size_t data_len
)
{
    // magic
    desc_buff[0] = (uint8_t)'?';
    // data
    memcpy((uint8_t*)desc_buff + MSG_HEADER_LEN_SUBSEQUENT + desc_buff_offset, src_buff, data_len);
}

static bool inline packet_process(
    uint8_t* desc_buff, const uint8_t* src_buff, size_t buff_len, size_t msg_len, size_t* outside_counter
)
{
    // Please note, as the protocol not really designed properly, there are cases that the package is subseq
    // with single byte HEADER "3F", and following two byte DATA is "23 23", which will render the header
    // useless for diff bewtween what packet it is. We added (outside_counter != 0) to the condition to
    // workaround this issue
    if ( src_buff[0] == '?' && src_buff[1] == '#' && src_buff[2] == '#' && (*outside_counter == 0) )
    {
        size_t process_size =
            (msg_len < (buff_len - MSG_HEADER_LEN_FIRST)) ? msg_len : (buff_len - MSG_HEADER_LEN_FIRST);
        memcpy(desc_buff, src_buff + MSG_HEADER_LEN_FIRST, process_size);
        *outside_counter += process_size;
        return true;
    }
    else if ( src_buff[0] == '?' && (*outside_counter != 0) )
    {
        size_t process_size = (msg_len < (buff_len - MSG_HEADER_LEN_SUBSEQUENT))
                                ? msg_len
                                : (buff_len - MSG_HEADER_LEN_SUBSEQUENT);
        memcpy(desc_buff, src_buff + MSG_HEADER_LEN_SUBSEQUENT, process_size);
        *outside_counter += process_size;
        return true;
    }
    else
    {
        return false;
    }
}

// ######## communication handlers ########

static bool callback_encode_bytes(pb_ostream_t* stream, const pb_field_iter_t* field, void* const* arg)
{
    const nanopb_callback_args* cb_arg = (nanopb_callback_args*)*arg;

    if ( cb_arg->payload_size > cb_arg->buffer_size )
    {
        return false;
    }
    if ( !pb_encode_tag_for_field(stream, field) )
    {
        return false;
    }
    if ( !pb_encode_string(stream, cb_arg->buffer, cb_arg->payload_size) )
    {
        return false;
    }

    return true;
}
static bool callback_decode_bytes(pb_istream_t* stream, const pb_field_iter_t* field, void** arg)
{
    const nanopb_callback_args* cb_arg = (nanopb_callback_args*)*arg;

    // Don't care cb_arg->payload_size since not used in here
    // if ( cb_arg->payload_size > cb_arg->buffer_size )
    // {
    //     return false;
    // }

    if ( stream->bytes_left > cb_arg->buffer_size )
    {
        return false;
    }

    memzero(cb_arg->buffer, cb_arg->buffer_size);
    while ( stream->bytes_left )
    {
        if ( !pb_read(stream, (pb_byte_t*)(cb_arg->buffer), stream->bytes_left) )
        {
            return false;
        }
    }

    return true;
}

#ifdef ASYNC_PARSE_NOT_USED
// this is just a cleanner way to keep code here
bool recv_msg_sync_parse(
    uint8_t iface_num, uint32_t msg_size, uint8_t* buf, const pb_msgdesc_t* fields, void* msg
)
{
    // sanity check
    if ( msg_size > SDRAM_BOOLOADER_BUFFER_RECV_LEN )
        return false;

    // new transfer, wipe buffer
    memzero(bl_buffer->recv_buff, SDRAM_BOOLOADER_BUFFER_RECV_LEN);

    // recv buffer count
    size_t recv_count = 0;

    // process initial packet
    if ( !packet_process(bl_buffer->recv_buff + recv_count, buf, IO_PACKET_SIZE, msg_size, &recv_count) )
        return false;

    while ( recv_count < msg_size )
    {
        // read next (blocking, with retry)
        int retry = 0;
        int result = 0;
        switch ( host_channel )
        {
        case CHANNEL_USB:
            while ( true )
            {
                // clear buffer
                memzero(buf, IO_PACKET_SIZE);

                // try
                result = usb_webusb_read_blocking(iface_num, buf, IO_PACKET_SIZE, IO_TIMEOUT);

                if ( result == IO_PACKET_SIZE )
                    // succeed, leave
                    break;

                // try again
                retry++;

                // exceed max retry, error
                if ( retry >= IO_RETRY_MAX )
                    break;
                // not error out since nanopb parse will fail anyways, and this way allows retry from
                outside
                // error_shutdown("\0", "\0", "Error reading from USB.", "Try different USB cable.");
            }
            break;
        case CHANNEL_SLAVE:
            if ( spi_slave_poll(buf) == 0 )
            {
                spi_read_retry(buf);
            }
            break;
        default:
            // error unknown channel
            return false;
            break;
        }

        // process packet
        if ( !packet_process(bl_buffer->recv_buff + recv_count, buf, IO_PACKET_SIZE, msg_size, &recv_count) )
            return false;
    }

    // cleanup outside buffer
    memzero(buf, IO_PACKET_SIZE);

    // decode it
    pb_istream_t istream = pb_istream_from_buffer(bl_buffer->recv_buff, msg_size);
    if ( !pb_decode_noinit(&istream, fields, msg) )
        return false;

    return true;
}

#endif

bool recv_msg_async_parse(
    uint8_t iface_num, uint32_t msg_size, uint8_t* buf, const pb_msgdesc_t* fields, void* msg
)
{
    // sanity check
    if ( msg_size > SDRAM_BOOLOADER_BUFFER_RECV_LEN )
        return false;

    // new transfer, wipe buffer
    memzero(bl_buffer->recv_buff, SDRAM_BOOLOADER_BUFFER_RECV_LEN);

    // recv buffer count
    size_t recv_count_raw = 0;

    // process initial packet
    memcpy(bl_buffer->recv_buff + recv_count_raw, buf, IO_PACKET_SIZE);
    recv_count_raw += IO_PACKET_SIZE;

    // expacted raw receive size
    size_t expected_raw_size = 0;

    if ( msg_size > (IO_PACKET_SIZE - MSG_HEADER_LEN_FIRST) )
    {
        // first packet
        expected_raw_size += IO_PACKET_SIZE;
        // mid packetS
        expected_raw_size += (msg_size - (IO_PACKET_SIZE - MSG_HEADER_LEN_FIRST)) /
                             (IO_PACKET_SIZE - MSG_HEADER_LEN_SUBSEQUENT) * IO_PACKET_SIZE;
        // last packet
        expected_raw_size += ((msg_size - (IO_PACKET_SIZE - MSG_HEADER_LEN_FIRST)) %
                                  (IO_PACKET_SIZE - MSG_HEADER_LEN_SUBSEQUENT) ==
                              0)
                               ? 0
                               : IO_PACKET_SIZE;
    }
    else
    {
        expected_raw_size += MSG_HEADER_LEN_FIRST + msg_size;
    }

    int retry = 0;
    int result = 0;
    while ( recv_count_raw < expected_raw_size )
    {
        // read next (blocking, with retry)
        retry = 0;
        switch ( host_channel )
        {
        case CHANNEL_USB:
            while ( true )
            {
                // try
                result = usb_webusb_read_blocking(
                    iface_num, bl_buffer->recv_buff + recv_count_raw, IO_PACKET_SIZE, IO_TIMEOUT
                );

                if ( result == IO_PACKET_SIZE )
                {
                    // succeed, leave
                    recv_count_raw += result;
                    break;
                }

                // try again
                retry++;

                // exceed max retry, error
                if ( retry >= IO_RETRY_MAX )
                {
                    // error_shutdown("\0", "\0", "Error reading from USB.", "Try different USB cable.");
                    send_failure_nocheck(
                        iface_num, FailureType_Failure_DataError, "Communication timed out!"
                    );
                    return false;
                    break;
                }
            }
            break;
        case CHANNEL_SLAVE:
            while ( true )
            {
                // try
                result = spi_read_blocking(bl_buffer->recv_buff + recv_count_raw, IO_TIMEOUT);

                if ( result == IO_PACKET_SIZE )
                {
                    // succeed, leave
                    recv_count_raw += result;
                    break;
                }

                // try again
                retry++;

                // exceed max retry, error
                if ( retry >= IO_RETRY_MAX )
                {
                    send_failure_nocheck(
                        iface_num, FailureType_Failure_DataError, "Communication timed out!"
                    );
                    return false;
                    break;
                }
            }
            break;
        default:
            // error unknown channel
            return false;
            break;
        }
    }

    // cleanup outside buffer
    memzero(buf, IO_PACKET_SIZE);

    // parse all packet in the same buffer, this works because packet always larger than data in side it
    size_t recv_raw_index = 0;
    size_t recv_parsed_index = 0;
    while ( recv_parsed_index < msg_size )
    {
        // process packet
        if ( !packet_process(
                 bl_buffer->recv_buff + recv_parsed_index, // parsed write to
                 bl_buffer->recv_buff + recv_raw_index,    // parse from
                 IO_PACKET_SIZE, msg_size, &recv_parsed_index
             ) )
            return false;

        recv_raw_index += IO_PACKET_SIZE;
    }

    // wipe unused space
    memzero(bl_buffer->recv_buff + msg_size, SDRAM_BOOLOADER_BUFFER_RECV_LEN - msg_size);

    // decode it
    pb_istream_t istream = pb_istream_from_buffer(bl_buffer->recv_buff, msg_size);
    if ( !pb_decode_noinit(&istream, fields, msg) )
        return false;

    return true;
}
bool send_msg(
    uint8_t iface_num, uint16_t msg_id, const pb_msgdesc_t* fields, const void* msg, bool ignore_check
)
{
    // new transfer, wipe buffer
    memzero(bl_buffer->send_buff, SDRAM_BOOLOADER_BUFFER_SEND_LEN);

    // send buffer count
    size_t send_count = 0;

    // encode it
    pb_ostream_t ostream = pb_ostream_from_buffer(bl_buffer->send_buff, SDRAM_BOOLOADER_BUFFER_SEND_LEN);
    if ( !pb_encode(&ostream, fields, msg) )
        return false;

    // msg_size
    const uint32_t msg_size = ostream.bytes_written;

    // temp buf
    uint8_t buf[IO_PACKET_SIZE];

    while ( send_count < msg_size )
    {
        // wipe temp buffer
        memzero(buf, IO_PACKET_SIZE);

        // generate packet
        if ( send_count == 0 )
        {
            size_t process_size = (msg_size < (IO_PACKET_SIZE - MSG_HEADER_LEN_FIRST))
                                    ? msg_size
                                    : (IO_PACKET_SIZE - MSG_HEADER_LEN_FIRST);
            packet_generate_first(
                msg_id, msg_size,                  // required header
                buf, 0,                            // target buff, no offset
                bl_buffer->send_buff + send_count, // source buff+offset
                process_size                       // bytes that fits or required
            );
            send_count += process_size;
        }
        else
        {
            size_t process_size = (msg_size < (IO_PACKET_SIZE - MSG_HEADER_LEN_SUBSEQUENT))
                                    ? msg_size
                                    : (IO_PACKET_SIZE - MSG_HEADER_LEN_SUBSEQUENT);
            packet_generate_subsequent(
                buf, 0,                            // target buff, no offset
                bl_buffer->send_buff + send_count, // source buff+offset
                process_size                       // bytes that fits or required
            );
            send_count += process_size;
        }

        // write (blocking)
        int result = 0;
        switch ( host_channel )
        {
        case CHANNEL_USB:
            result = usb_webusb_write_blocking(iface_num, buf, IO_PACKET_SIZE, IO_TIMEOUT);
            break;
        case CHANNEL_SLAVE:
            hal_delay(5);
            result = spi_slave_send(buf, IO_PACKET_SIZE, IO_TIMEOUT);
            break;
        default:
            // error unknown channel
            break;
        }

        if ( !ignore_check )
            ensure(sectrue * (result == IO_PACKET_SIZE), NULL);
    }

    return true;
}

// ######## standard messages ########
static void send_success(uint8_t iface_num, const char* text)
{
    MSG_INIT(msg_send, Success);
    MSG_ASSIGN_STRING(msg_send, message, text);
    MSG_SEND(msg_send, Success);
}
void send_success_nocheck(uint8_t iface_num, const char* text)
{
    MSG_INIT(msg_send, Success);
    MSG_ASSIGN_STRING(msg_send, message, text);
    MSG_SEND_NOCHECK(msg_send, Success);
}
static void send_failure(uint8_t iface_num, FailureType type, const char* text)
{
    MSG_INIT(msg_send, Failure);
    MSG_ASSIGN_VALUE(msg_send, code, type);
    MSG_ASSIGN_STRING(msg_send, message, text);
    MSG_SEND(msg_send, Failure);
}
void send_failure_nocheck(uint8_t iface_num, FailureType type, const char* text)
{
    MSG_INIT(msg_send, Failure);
    MSG_ASSIGN_VALUE(msg_send, code, type);
    MSG_ASSIGN_STRING(msg_send, message, text);
    MSG_SEND_NOCHECK(msg_send, Failure);
}
void send_failure_detailed(uint8_t iface_num, FailureType type, const char* fmt, ...)
{
    // format message
    char msg_buff[256];
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(msg_buff, sizeof(msg_buff) / sizeof(char), fmt, argptr);
    va_end(argptr);

    MSG_INIT(msg_send, Failure);
    MSG_ASSIGN_VALUE(msg_send, code, type);
    MSG_ASSIGN_STRING(msg_send, message, msg_buff);
    MSG_SEND(msg_send, Failure);
}
static void send_user_abort(uint8_t iface_num, const char* msg)
{
    MSG_INIT(msg_send, Failure);
    MSG_ASSIGN_VALUE(msg_send, code, FailureType_Failure_ActionCancelled);
    MSG_ASSIGN_STRING(msg_send, message, msg);
    MSG_SEND(msg_send, Failure);
}
void send_user_abort_nocheck(uint8_t iface_num, const char* msg)
{
    MSG_INIT(msg_send, Failure);
    MSG_ASSIGN_VALUE(msg_send, code, FailureType_Failure_ActionCancelled);
    MSG_ASSIGN_STRING(msg_send, message, msg);
    MSG_SEND_NOCHECK(msg_send, Failure);
}

// ######## message handlers ########

static int version_compare(uint32_t vera, uint32_t verb)
{
    int a, b;
    a = vera & 0xFF;
    b = verb & 0xFF;
    if ( a != b )
        return a - b;
    a = (vera >> 8) & 0xFF;
    b = (verb >> 8) & 0xFF;
    if ( a != b )
        return a - b;
    a = (vera >> 16) & 0xFF;
    b = (verb >> 16) & 0xFF;
    if ( a != b )
        return a - b;
    a = (vera >> 24) & 0xFF;
    b = (verb >> 24) & 0xFF;
    return a - b;
}

// not used since no where to store them
// static void firmware_headers_store(const uint8_t* const input_buffer, size_t buffer_len)
// {
//     // not implemented yet
// }
// static void firmware_headers_retrieve(uint8_t* const outputput_buffer, size_t buffer_len)
// {
//     memzero(outputput_buffer, buffer_len);
//     // not implemented yet
// }

static secbool check_image_contents_ram(
    const image_header* const hdr, const uint8_t* const buffer, size_t code_offset, size_t blocks
)
{
    if ( hdr == NULL || buffer == NULL || (code_offset <= 0) || (blocks <= 0) )
    {
        return false;
    }

    secbool result = secfalse;

    const uint8_t* code_data = buffer + code_offset;
    const size_t code_len = hdr->codelen;
    const size_t hash_chunk_size = IMAGE_CHUNK_SIZE * 2;

    size_t processed_size = 0;
    size_t block = 0;
    size_t process_size = 0;

    while ( true )
    {
        if ( processed_size >= code_len || block >= blocks )
        {
            // make sure we actually checked something
            if ( processed_size > 0 )
                // flag set to valid
                result = sectrue;

            // normal exit, no error found
            break;
        }

        if ( processed_size == 0 )
            process_size = MIN((code_len - processed_size), (hash_chunk_size - code_offset));
        else
            process_size = MIN((code_len - processed_size), (hash_chunk_size));

        if ( sectrue ==
             check_single_hash(hdr->hashes + block * 32, code_data + processed_size, process_size) )
        {
            block++;
            processed_size += process_size;
        }
        else
        {
            // error exit, hash mismatch
            break;
        }
    }

    return result;
}

int process_msg_FirmwareUpdateEmmc(uint8_t iface_num, uint32_t msg_size, uint8_t* buf)
{
    MSG_INIT(msg_recv, FirmwareUpdateEmmc);
    MSG_RECV_RET_ON_ERR(msg_recv, FirmwareUpdateEmmc);

    // wipe whole buffer
    memzero(bl_buffer->misc_buff, SDRAM_BOOLOADER_BUFFER_MISC_LEN);

    // get file info
    EMMC_PATH_INFO file_info;
    ExecuteCheck_MSGS_ADV(emmc_fs_path_info(msg_recv.path, &file_info), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
            char emmc_fs_status_str[512];
            emmc_fs_format_status(emmc_fs_status_str, 512);
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed \n%s", __func__, str_func_call,
                emmc_fs_status_str
            );
#endif
        return -1;
    });

    // check file exist, type, size
    ExecuteCheck_MSGS_ADV((file_info.path_exist), true, {
        send_failure(iface_num, FailureType_Failure_ProcessError, "Firmware file not exist!");
        return -1;
    });
    ExecuteCheck_MSGS_ADV((file_info.attrib.directory), false, {
        send_failure(iface_num, FailureType_Failure_ProcessError, "Firmware file path is a directory!");
        return -1;
    });
    ExecuteCheck_MSGS_ADV(
        ((file_info.size > 0) && (file_info.size < SDRAM_BOOLOADER_BUFFER_MISC_LEN)), true,
        {
            emmc_fs_file_delete(msg_recv.path);
            send_failure(iface_num, FailureType_Failure_ProcessError, "Firmware file size invalid!");
            return -1;
        }
    );

    // copy over size
    uint32_t emmc_file_size = file_info.size;

    // read firmware file to ram
    uint32_t processed = 0;
    ExecuteCheck_MSGS_ADV(
        emmc_fs_file_read(msg_recv.path, 0, bl_buffer->misc_buff, emmc_file_size, &processed), true,
        {
#if PRODUCTION
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
            );
#else
            char emmc_fs_status_str[512];
            emmc_fs_format_status(emmc_fs_status_str, 512);
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed \n%s", __func__, str_func_call,
                emmc_fs_status_str
            );
#endif
            return -1;
        }
    );

    // make sure same size read
    if ( processed != emmc_file_size )
    {
        emmc_fs_file_delete(msg_recv.path);
        send_failure(iface_num, FailureType_Failure_ProcessError, "Firmware reading error!");
        return -1;
    }

    // check firmware header
    extern const uint8_t BOOTLOADER_KEY_M;
    extern const uint8_t BOOTLOADER_KEY_N;
    extern const uint8_t* const BOOTLOADER_KEYS[];

    vendor_header file_vhdr;
    image_header file_hdr;
    image_header_old thd89_hdr;

    // detect firmware type
    if ( memcmp(bl_buffer->misc_buff, "5283", 4) == 0 )
    {
        // bluetooth update

        // check header
        ExecuteCheck_MSGS_ADV(
            load_ble_image_header(
                bl_buffer->misc_buff, FIRMWARE_IMAGE_MAGIC_BLE, FIRMWARE_IMAGE_MAXSIZE_BLE, &file_hdr
            ),
            sectrue,
            {
                emmc_fs_file_delete(msg_recv.path);
                send_failure(iface_num, FailureType_Failure_ProcessError, "Update file header invalid!");
                return -3;
            }
        );

        // make sure we have latest bluetooth status
        ble_refresh_dev_info();

        // ui confirm
        ui_fadeout();
        ui_install_ble_confirm();
        ui_fadein();

        int response = ui_input_poll(INPUT_CONFIRM | INPUT_CANCEL, true);
        if ( INPUT_CONFIRM != response )
        {
            // We could but should not remove the file if user cancels
            // emmc_fs_file_delete(msg_recv.path);
            ui_fadeout();
            ui_bootloader_first(NULL);
            ui_fadein();
            send_user_abort_nocheck(iface_num, "Firmware install cancelled");
            return -4;
        }

        // return success as bluetooth will be disconnected, we have no way to send result back if the update
        // started via bluetooth
        send_success_nocheck(iface_num, "Succeed");
        // may have to delay a bit to allow the message be sent out (if ui fade in and out time is too short)
        // hal_delay(50);

        // ui start install
        ui_fadeout();
        ui_screen_progress_bar_prepare("Installing", NULL);
        ui_fadein();

        // enter dfu
        ExecuteCheck_MSGS_ADV(bluetooth_enter_dfu(), true, {
            emmc_fs_file_delete(msg_recv.path);
            send_failure(iface_num, FailureType_Failure_ProcessError, "Bluetooth enter DFU failed!");
            return -6;
        });

        // install
        uint8_t* p_init = (uint8_t*)bl_buffer->misc_buff + IMAGE_HEADER_SIZE;
        uint32_t init_data_len = p_init[0] + (p_init[1] << 8);
        ExecuteCheck_MSGS_ADV(
            bluetooth_update(
                p_init + 4, init_data_len, bl_buffer->misc_buff + IMAGE_HEADER_SIZE + BLE_INIT_DATA_LEN,
                file_hdr.codelen - BLE_INIT_DATA_LEN, ui_screen_progress_bar_update
            ),
            true,
            {
                emmc_fs_file_delete(msg_recv.path);
                send_failure(
                    iface_num, FailureType_Failure_ProcessError, "Update bluetooth firmware failed!"
                );
                return -6;
            }
        );

        // update progress (final)
        ui_screen_progress_bar_update(NULL, NULL, 100);

        // delay before kick it out of DFU
        // this is important, otherwise the update may fail
        hal_delay(50);
        // reboot bluetooth
        bluetooth_reset();
        // make sure we have latest bluetooth status (and wait for bluetooth become ready)
        ble_refresh_dev_info();

        // buetooth update done
        // emmc_fs_file_delete(msg_recv.path);
        if ( msg_recv.has_reboot_on_success && msg_recv.reboot_on_success )
        {
            *STAY_IN_FLAG_ADDR = 0;
            restart();
        }
        else
        {
            ui_fadeout();
            ui_bootloader_first(NULL);
            ui_fadein();
        }
        return 0;
    }
    else if ( memcmp(bl_buffer->misc_buff, "TF89", 4) == 0 )
    {
        // se thd89 update
        // check header
        ExecuteCheck_MSGS_ADV(
            load_thd89_image_header(
                bl_buffer->misc_buff, FIRMWARE_IMAGE_MAGIC_THD89, FIRMWARE_IMAGE_MAXSIZE_THD89, &thd89_hdr
            ),
            sectrue,
            {
                emmc_fs_file_delete(msg_recv.path);
                send_failure(iface_num, FailureType_Failure_ProcessError, "Update file header invalid!");
                return -3;
            }
        );

        // check file size
        ExecuteCheck_MSGS_ADV((emmc_file_size == thd89_hdr.codelen + IMAGE_HEADER_SIZE), true, {
            emmc_fs_file_delete(msg_recv.path);
            send_failure(iface_num, FailureType_Failure_ProcessError, "Firmware file length error!");
            return -1;
        });
        const char *se_version = se_get_version();
        if ( !se_back_to_boot_progress() )
        {
            send_failure(iface_num, FailureType_Failure_ProcessError, "SE back to boot error");
            return -1;
        }

        if ( !se_verify_firmware(bl_buffer->misc_buff, IMAGE_HEADER_SIZE))
        {
            send_failure(iface_num, FailureType_Failure_ProcessError, "SE verify header error");
            return -1;
        }
        // ui confirm
        ui_fadeout();
        ui_install_thd89_confirm(se_version);
        ui_fadein();

        int response = ui_input_poll(INPUT_CONFIRM | INPUT_CANCEL, true);
        if ( INPUT_CONFIRM != response )
        {
            // We could but should not remove the file if user cancels
            // emmc_fs_file_delete(msg_recv.path);
            ui_fadeout();
            ui_bootloader_first(NULL);
            ui_fadein();
            send_user_abort_nocheck(iface_num, "Firmware install cancelled");
            return -4;
        }

        // ui start install
        ui_fadeout();
        ui_screen_install_start();
        // ui_screen_progress_bar_prepare("Installing", NULL);
        ui_fadein();

        // install
        if(!se_update_firmware(bl_buffer->misc_buff + IMAGE_HEADER_SIZE, thd89_hdr.codelen,ui_screen_install_progress_upload))
        {
            send_failure(iface_num, FailureType_Failure_ProcessError, "SE update error");
            return -1;
        }

        if ( !se_active_app_progress() )
        {
            send_failure(iface_num, FailureType_Failure_ProcessError, "SE activate app error");
            return -1;
        }

        // update progress (final)
        ui_screen_progress_bar_update(NULL, NULL, 100);

        send_success(iface_num, "Succeed");

        // emmc_fs_file_delete(msg_recv.path);
        if ( msg_recv.has_reboot_on_success && msg_recv.reboot_on_success )
        {
            *STAY_IN_FLAG_ADDR = 0;
            restart();
        }
        else
        {
            ui_fadeout();
            ui_bootloader_first(NULL);
            ui_fadein();
        }
        return 0;
    }
    else
    {
        // MCU update

        // check file size
        ExecuteCheck_MSGS_ADV((emmc_file_size <= FIRMWARE_SECTORS_COUNT * FLASH_FIRMWARE_SECTOR_SIZE), true, {
            emmc_fs_file_delete(msg_recv.path);
            send_failure(iface_num, FailureType_Failure_ProcessError, "Firmware file is too big!");
            return -1;
        });

        // check file header
        ExecuteCheck_MSGS_ADV(
            load_vendor_header(
                bl_buffer->misc_buff, BOOTLOADER_KEY_M, BOOTLOADER_KEY_N, BOOTLOADER_KEYS, &file_vhdr
            ),
            sectrue,
            {
                send_failure(
                    iface_num, FailureType_Failure_ProcessError, "Update file vender header invalid!"
                );
                return -1;
            }
        );
        ExecuteCheck_MSGS_ADV(
            load_image_header(
                bl_buffer->misc_buff + file_vhdr.hdrlen, FIRMWARE_IMAGE_MAGIC, FIRMWARE_IMAGE_MAXSIZE,
                file_vhdr.vsig_m, file_vhdr.vsig_n, file_vhdr.vpub, &file_hdr
            ),
            sectrue,
            {
                send_failure(iface_num, FailureType_Failure_ProcessError, "Update file header invalid!");
                return -1;
            }
        );

        // check file firmware hash
        ExecuteCheck_MSGS_ADV(
            check_image_contents_ram(
                &file_hdr, bl_buffer->misc_buff, file_vhdr.hdrlen + file_hdr.hdrlen, FIRMWARE_SECTORS_COUNT
            ),
            sectrue,
            {
                send_failure(iface_num, FailureType_Failure_ProcessError, "Update file hash invalid!");
                return -1;
            }
        );

        // check firmware header
        vendor_header current_vhdr;
        image_header current_hdr;

        // retrieve stored firmware headers (not used)
        // uint8_t firmware_headers_backup[FIRMWARE_HEADERS_LEN];
        // firmware_headers_retrieve(firmware_headers_backup, FIRMWARE_HEADERS_LEN);

        // initial value
        secbool firmware_header_valid = secfalse;
        secbool wipe_required = sectrue;
        secbool downgrade_or_vendor_change = secfalse;

        // check if currently have a vaild firmware header
        while ( true )
        {

            vendor_header temp_vhdr;
            image_header temp_hdr;

            // vhdr
            if ( load_vendor_header(
                     (const uint8_t*)FIRMWARE_START, BOOTLOADER_KEY_M, BOOTLOADER_KEY_N, BOOTLOADER_KEYS,
                     &temp_vhdr
                 ) == sectrue )
            {
                memcpy(&current_vhdr, &temp_vhdr, sizeof(current_vhdr));
            }
            // else if ( load_vendor_header(firmware_headers_backup, BOOTLOADER_KEY_M, BOOTLOADER_KEY_N,
            // BOOTLOADER_KEYS, &temp_vhdr) == sectrue )
            // {
            //     memcpy(&current_vhdr, &temp_vhdr, sizeof(current_vhdr));
            // }
            else
                break;

            // hdr
            if ( load_image_header(
                     (const uint8_t*)FIRMWARE_START + current_vhdr.hdrlen, FIRMWARE_IMAGE_MAGIC,
                     FIRMWARE_IMAGE_MAXSIZE, current_vhdr.vsig_m, current_vhdr.vsig_n, current_vhdr.vpub,
                     &temp_hdr
                 ) == sectrue )
            {
                memcpy(&current_hdr, &temp_hdr, sizeof(current_hdr));
            }
            // else if ( load_image_header(
            //          firmware_headers_backup + current_vhdr.hdrlen, FIRMWARE_IMAGE_MAGIC,
            //          FIRMWARE_IMAGE_MAXSIZE, current_vhdr.vsig_m, current_vhdr.vsig_n, current_vhdr.vpub,
            //          &temp_hdr
            //      ) == sectrue )
            // {
            //     memcpy(&current_hdr, &temp_hdr, sizeof(current_hdr));
            // }
            else
                break;

            // confirmed firmware detected (at least headers)
            firmware_header_valid = sectrue;

            // check vendor identity
            uint8_t hash1[32], hash2[32];
            vendor_header_hash(&file_vhdr, hash1);
            vendor_header_hash(&current_vhdr, hash2);
            if ( 0 == memcmp(hash1, hash2, 32) )
            {
                // vendor identity match

                // compare version
                if ( !(version_compare(file_hdr.onekey_version, current_hdr.onekey_version) < 0) )
                {
                    // new firwmare have same or higher version

                    // confirmed wipe not required
                    wipe_required = secfalse;

                    // downgrade_or_vendor_change ignored in this case
                    EMMC_WRAPPER_UNUSED(downgrade_or_vendor_change);
                    break;
                }
                else
                {
                    // new firwmare have lower version
                    downgrade_or_vendor_change = sectrue;
                    break;
                }
            }
            else
            {
                // vendor identity mismatch
                downgrade_or_vendor_change = secfalse;
                break;
            }

            // should not come to here
            break;
        }

        // ui confirm
        if ( firmware_header_valid == sectrue )
        {
            ui_fadeout();
            // if ( (wipe_required == sectrue) || (msg_recv.has_force_erase && msg_recv.force_erase) )
            if ( (wipe_required == sectrue) )
            {
                ui_screen_install_confirm_newvendor_or_downgrade_wipe(
                    &file_vhdr, &file_hdr, downgrade_or_vendor_change
                );
            }
            else
            {
                ui_install_confirm(&current_hdr, &file_hdr);
            }
            ui_fadein();

            int response = ui_input_poll(INPUT_CONFIRM | INPUT_CANCEL, true);
            if ( INPUT_CONFIRM != response )
            {
                // We could but should not remove the file if user cancels
                // emmc_fs_file_delete(msg_recv.path);
                ui_fadeout();
                ui_bootloader_first(&current_hdr);
                ui_fadein();
                send_user_abort_nocheck(iface_num, "Firmware install cancelled");
                return -4;
            }
        }

        // ui start install
        ui_fadeout();
        ui_screen_install_start();
        ui_fadein();

        // selectively wipe user data and reset se
        if ( sectrue == wipe_required )
        {
            se_reset_storage();

            // erease with retry, max 10 retry allowed, 10ms delay in between
            ExecuteCheck_MSGS_ADV_RETRY_DELAY(
                flash_erase_sectors(STORAGE_SECTORS, STORAGE_SECTORS_COUNT, NULL), sectrue,
                {
                    send_failure(
                        iface_num, FailureType_Failure_ProcessError, "Flash storage area erease failed!"
                    );
                    return -1;
                },
                10, 10
            );
        }

        // write firmware

        size_t processed_bytes = 0;
        size_t flash_sectors_index = 0;
        while ( processed_bytes < emmc_file_size )
        {

            // wipe firmware area
            // erease with retry, max 10 retry allowed, 10ms delay in between
            ExecuteCheck_MSGS_ADV_RETRY_DELAY(
                flash_erase(FIRMWARE_SECTORS[flash_sectors_index]), sectrue,
                {
                    send_failure(
                        iface_num, FailureType_Failure_ProcessError, "Flash firmware area erease failed!"
                    );
                },
                10, 10
            );
            // update progress (erase)
            ui_screen_install_progress_upload(
                (250 * flash_sectors_index / (emmc_file_size / flash_sector_size(flash_sectors_index))) +
                (750 * processed_bytes / emmc_file_size)
            );

            // unlock
            ExecuteCheck_MSGS_ADV(flash_unlock_write(), sectrue, {
                send_failure(iface_num, FailureType_Failure_ProcessError, "Flash unlock failed!");
                return -1;
            });

            for ( size_t sector_offset = 0;
                  sector_offset < flash_sector_size(FIRMWARE_SECTORS[flash_sectors_index]);
                  sector_offset += 32 )
            {
                // write with retry, max 10 retry allowed, 10ms delay in between
                ExecuteCheck_MSGS_ADV_RETRY_DELAY(
                    flash_write_words(
                        FIRMWARE_SECTORS[flash_sectors_index], sector_offset,
                        (uint32_t*)(bl_buffer->misc_buff + processed_bytes)
                    ),
                    sectrue,
                    {
                        send_failure(iface_num, FailureType_Failure_ProcessError, "Flash write failed!");
                        return -1;
                    },
                    10, 10
                );

                processed_bytes += ((emmc_file_size - processed_bytes) > 32)
                                     ? 32 // since we could only write 32 byte a time
                                     : (emmc_file_size - processed_bytes);
            }

            // lock
            ExecuteCheck_MSGS_ADV(flash_lock_write(), sectrue, {
                send_failure(iface_num, FailureType_Failure_ProcessError, "Flash unlock failed!");
                return -1;
            });

            // update progress (write)
            ui_screen_install_progress_upload(
                (250 * flash_sectors_index / (emmc_file_size / flash_sector_size(flash_sectors_index))) +
                (750 * processed_bytes / emmc_file_size)
            );

            flash_sectors_index++;
        }

        // wipe unused sectors
        while ( flash_sectors_index < FIRMWARE_SECTORS_COUNT )
        {
            flash_erase(FIRMWARE_SECTORS[flash_sectors_index]);
            flash_sectors_index++;
        }

        // verify flash firmware hash
        ExecuteCheck_MSGS_ADV(
            check_image_contents(
                &file_hdr, IMAGE_HEADER_SIZE + file_vhdr.hdrlen, FIRMWARE_SECTORS, FIRMWARE_SECTORS_COUNT
            ),
            sectrue,
            {
                send_failure(iface_num, FailureType_Failure_ProcessError, "New firmware hash invalid!");
                // wipe invalid firmware, don't care the result as we cannot control, but we have to try
                EMMC_WRAPPER_FORCE_IGNORE(flash_erase_sectors(FIRMWARE_SECTORS, FIRMWARE_SECTORS_COUNT, NULL)
                );
                return -1;
            }
        );

        // backup new firmware header (not used since no where to stroe it)
        // As the firmware in flash has is the the same as the one from file, and it has been verified, we
        // could use file_vhdr and file_hdr, instead of read them from flash again.
        // firmware_headers_store((const uint8_t*)FIRMWARE_START, file_vhdr.hdrlen + file_hdr.hdrlen);

        // update progress (final)
        ui_screen_install_progress_upload(1000);

        // MCU update done
        // emmc_fs_file_delete(msg_recv.path);
        send_success_nocheck(iface_num, "Succeed");
        if ( msg_recv.has_reboot_on_success && msg_recv.reboot_on_success )
        {
            *STAY_IN_FLAG_ADDR = 0;
            restart();
        }
        else
        {
            ui_fadeout();
            ui_bootloader_first(&file_hdr);
            ui_fadein();
        }
        return 0;
    }

    // should not come to here
    send_failure(iface_num, FailureType_Failure_ProcessError, "Unexpected error during firmware update!");
    return -1;
}

int process_msg_EmmcFixPermission(uint8_t iface_num, uint32_t msg_size, uint8_t* buf)
{
    // fix onekey data and user data
    ExecuteCheck_MSGS_ADV(emmc_fs_mount(true, true), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
      char emmc_fs_status_str[512];
      emmc_fs_format_status(emmc_fs_status_str, 512);
      send_failure_detailed(
        iface_num, FailureType_Failure_ProcessError,
        "%s -> %s Failed \n%s", __func__, str_func_call, emmc_fs_status_str);
#endif
        return -1;
    });
    ExecuteCheck_MSGS_ADV(emmc_fs_fix_permission(true, true), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
      char emmc_fs_status_str[512];
      emmc_fs_format_status(emmc_fs_status_str, 512);
      send_failure_detailed(
        iface_num, FailureType_Failure_ProcessError,
        "%s -> %s Failed \n%s", __func__, str_func_call, emmc_fs_status_str);
#endif
        return -1;
    });
    ExecuteCheck_MSGS_ADV(emmc_fs_unmount(true, true), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
      char emmc_fs_status_str[512];
      emmc_fs_format_status(emmc_fs_status_str, 512);
      send_failure_detailed(
        iface_num, FailureType_Failure_ProcessError,
        "%s -> %s Failed \n%s", __func__, str_func_call, emmc_fs_status_str);
#endif
        return -1;
    });

    send_success_nocheck(iface_num, "Succeed");
    return 0;
}

int process_msg_EmmcPathInfo(uint8_t iface_num, uint32_t msg_size, uint8_t* buf)
{
    MSG_INIT(msg_recv, EmmcPathInfo);
    MSG_RECV_RET_ON_ERR(msg_recv, EmmcPathInfo);

    if ( (strlen(msg_recv.path) == 2) && (msg_recv.path[1] == ':') )
    {
        send_failure(
            iface_num, FailureType_Failure_ProcessError, "Use this command on root path not allowed!"
        );
        return -1;
    }

    EMMC_PATH_INFO file_info;
    ExecuteCheck_MSGS_ADV(emmc_fs_path_info(msg_recv.path, &file_info), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
            char emmc_fs_status_str[512];
            emmc_fs_format_status(emmc_fs_status_str, 512);
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed \n%s", __func__, str_func_call,
                emmc_fs_status_str
            );
#endif
        return -1;
    });

    MSG_INIT(msg_send, EmmcPath);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, exist, file_info.path_exist);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, size, file_info.size);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, year, file_info.date.year);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, month, file_info.date.month);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, day, file_info.date.day);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, hour, file_info.time.hour);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, minute, file_info.time.minute);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, second, file_info.time.second);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, readonly, file_info.attrib.readonly);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, hidden, file_info.attrib.hidden);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, system, file_info.attrib.system);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, archive, file_info.attrib.archive);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, directory, file_info.attrib.directory);
    MSG_SEND_NOCHECK(msg_send, EmmcPath);
    return 0;
}

int process_msg_EmmcFileRead(uint8_t iface_num, uint32_t msg_size, uint8_t* buf)
{
    memzero(bl_buffer->misc_buff, SDRAM_BOOLOADER_BUFFER_MISC_LEN);
    nanopb_callback_args cb_args_send = {
        .buffer = bl_buffer->misc_buff,
        .buffer_size = SDRAM_BOOLOADER_BUFFER_MISC_LEN,
        .payload_size = 0,
    };

    MSG_INIT(msg_recv, EmmcFileRead);
    MSG_RECV_RET_ON_ERR(msg_recv, EmmcFileRead);

    if ( msg_recv.file.len > cb_args_send.buffer_size )
    {
        send_failure(iface_num, FailureType_Failure_ProcessError, "File length larger than buffer!");
        return -1;
    }

    // handle progress display
    // please note, this value not calculated localy as we don't know the total (requester intersted) size
    if ( msg_recv.has_ui_percentage )
    {
        char ui_progress_title[] = "Transferring Data";

        // sanity check
        if ( (msg_recv.ui_percentage < 0) || (msg_recv.ui_percentage > 100) )
        {
            send_failure(iface_num, FailureType_Failure_ProcessError, "Percentage invalid!");
            return -1;
        }

        else if ( msg_recv.ui_percentage < 100 )
        {
            if ( !ui_progress_bar_visible )
            {
                ui_fadeout();
                ui_screen_progress_bar_prepare(ui_progress_title, NULL);
                ui_fadein();
                ui_screen_progress_bar_update(NULL, NULL, msg_recv.ui_percentage);
                ui_progress_bar_visible = true;
            }
            else
                ui_screen_progress_bar_update(NULL, NULL, msg_recv.ui_percentage);
        }
        else if ( msg_recv.ui_percentage == 100 )
        {
            if ( !ui_progress_bar_visible )
            {
                // this is for the instant 100% case, which happens if the file is too small
                ui_fadeout();
                ui_screen_progress_bar_prepare(ui_progress_title, NULL);
                ui_screen_progress_bar_update(NULL, NULL, msg_recv.ui_percentage);
                ui_fadein();
            }
            else
            {
                // normal path
                ui_screen_progress_bar_update(NULL, NULL, msg_recv.ui_percentage);
                ui_progress_bar_visible = false;
            }
            ui_fadeout();
            ui_bootloader_first(NULL);
            ui_fadein();
        }
    }
    else
    {
        if ( ui_progress_bar_visible )
        {
            ui_progress_bar_visible = false;
            display_clear();
            ui_bootloader_first(NULL);
        }
    }

    // get file info
    EMMC_PATH_INFO file_info;
    ExecuteCheck_MSGS_ADV(emmc_fs_path_info(msg_recv.file.path, &file_info), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
            char emmc_fs_status_str[512];
            emmc_fs_format_status(emmc_fs_status_str, 512);
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed \n%s", __func__, str_func_call,
                emmc_fs_status_str
            );
#endif
        return -1;
    });

    // check file exist, type, size
    ExecuteCheck_MSGS_ADV((file_info.path_exist), true, {
        send_failure(iface_num, FailureType_Failure_ProcessError, "File not exist!");
        return -1;
    });
    ExecuteCheck_MSGS_ADV((file_info.attrib.directory), false, {
        send_failure(iface_num, FailureType_Failure_ProcessError, "File path is a directory!");
        return -1;
    });
    ExecuteCheck_MSGS_ADV(((file_info.size > 0) && (file_info.size < cb_args_send.buffer_size)), true, {
        send_failure(iface_num, FailureType_Failure_ProcessError, "File size invalid!");
        return -1;
    });

    // check read size
    if ( (msg_recv.file.offset + msg_recv.file.len) > file_info.size )
    {
        send_failure(
            iface_num, FailureType_Failure_ProcessError, "Read beyond available file size not allowed!"
        );
    }

    uint32_t processed = 0;
    ExecuteCheck_MSGS_ADV(
        emmc_fs_file_read(
            msg_recv.file.path, msg_recv.file.offset, cb_args_send.buffer, msg_recv.file.len, &processed
        ),
        true,
        {
#if PRODUCTION
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
            );
#else
            char emmc_fs_status_str[512];
            emmc_fs_format_status(emmc_fs_status_str, 512);
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed \n%s", __func__, str_func_call,
                emmc_fs_status_str
            );
#endif
            return -1;
        }
    );
    cb_args_send.payload_size = processed;

    MSG_INIT(msg_send, EmmcFile);
    MSG_ASSIGN_REQUIRED_STRING(msg_send, path, msg_recv.file.path);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, len, msg_recv.file.len);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, offset, msg_recv.file.offset);
    MSG_ASSIGN_VALUE(msg_send, processed_byte, processed);
    MSG_ASSIGN_CALLBACK_ENCODE(msg_send, data, &callback_encode_bytes, &cb_args_send);
    MSG_SEND_NOCHECK(msg_send, EmmcFile);

    return 0;
}
int process_msg_EmmcFileWrite(uint8_t iface_num, uint32_t msg_size, uint8_t* buf)
{
    memzero(bl_buffer->misc_buff, SDRAM_BOOLOADER_BUFFER_MISC_LEN);
    nanopb_callback_args cb_args_recv = {
        .buffer = bl_buffer->misc_buff,
        .buffer_size = SDRAM_BOOLOADER_BUFFER_MISC_LEN,
        .payload_size = 0,
    };

    MSG_INIT(msg_recv, EmmcFileWrite);
    MSG_ASSIGN_CALLBACK_DECODE(msg_recv, file.data, &callback_decode_bytes, &cb_args_recv);
    MSG_RECV_RET_ON_ERR(msg_recv, EmmcFileWrite);

    if ( msg_recv.file.len > cb_args_recv.buffer_size )
    {
        send_failure(iface_num, FailureType_Failure_ProcessError, "File length larger than buffer!");
        return -1;
    }

    // handle progress display
    // please note, this value not calculated localy as we don't know the total (requester intersted) size
    if ( msg_recv.has_ui_percentage )
    {
        char ui_progress_title[] = "Transferring Data";

        // sanity check
        if ( (msg_recv.ui_percentage < 0) || (msg_recv.ui_percentage > 100) )
        {
            send_failure(iface_num, FailureType_Failure_ProcessError, "Percentage invalid!");
            return -1;
        }
        else if ( msg_recv.ui_percentage < 100 )
        {
            if ( !ui_progress_bar_visible )
            {
                ui_fadeout();
                ui_screen_progress_bar_prepare(ui_progress_title, NULL);
                ui_fadein();
                ui_screen_progress_bar_update(NULL, NULL, msg_recv.ui_percentage);
                ui_progress_bar_visible = true;
            }
            else
                ui_screen_progress_bar_update(NULL, NULL, msg_recv.ui_percentage);
        }
        else if ( msg_recv.ui_percentage == 100 )
        {
            if ( !ui_progress_bar_visible )
            {
                // this is for the instant 100% case, which happens if the file is too small
                ui_fadeout();
                ui_screen_progress_bar_prepare(ui_progress_title, NULL);
                ui_screen_progress_bar_update(NULL, NULL, msg_recv.ui_percentage);
                ui_fadein();
            }
            else
            {
                // normal path
                ui_screen_progress_bar_update(NULL, NULL, msg_recv.ui_percentage);
                ui_progress_bar_visible = false;
            }
            ui_fadeout();
            ui_bootloader_first(NULL);
            ui_fadein();
        }
    }
    else
    {
        if ( ui_progress_bar_visible )
        {
            ui_progress_bar_visible = false;
            display_clear();
            ui_bootloader_first(NULL);
        }
    }

    // get file info
    EMMC_PATH_INFO file_info;
    ExecuteCheck_MSGS_ADV(emmc_fs_path_info(msg_recv.file.path, &file_info), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
            char emmc_fs_status_str[512];
            emmc_fs_format_status(emmc_fs_status_str, 512);
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed \n%s", __func__, str_func_call,
                emmc_fs_status_str
            );
#endif
        return -1;
    });

    if ( file_info.path_exist )
    {
        ExecuteCheck_MSGS_ADV((file_info.attrib.directory), false, {
            send_failure(iface_num, FailureType_Failure_ProcessError, "File path is a directory!");
            return -1;
        });
        ExecuteCheck_MSGS_ADV((true && msg_recv.overwrite && msg_recv.append), false, {
            send_failure(
                iface_num, FailureType_Failure_ProcessError,
                "File exists but overwrite and append both enabled!"
            );
            return -1;
        });
        ExecuteCheck_MSGS_ADV((false || msg_recv.overwrite || msg_recv.append), true, {
            send_failure(
                iface_num, FailureType_Failure_ProcessError,
                "File exists but overwrite and append both disabled!"
            );
            return -1;
        });
    }

    uint32_t processed = 0;
    ExecuteCheck_MSGS_ADV(
        emmc_fs_file_write(
            msg_recv.file.path, msg_recv.file.offset, cb_args_recv.buffer, msg_recv.file.len, &processed,
            msg_recv.overwrite, msg_recv.append
        ),
        true,
        {
#if PRODUCTION
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
            );
#else
            char emmc_fs_status_str[512];
            emmc_fs_format_status(emmc_fs_status_str, 512);
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed \n%s", __func__, str_func_call,
                emmc_fs_status_str
            );
#endif
            return -1;
        }
    );

    MSG_INIT(msg_send, EmmcFile);
    MSG_ASSIGN_REQUIRED_STRING(msg_send, path, msg_recv.file.path);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, len, msg_recv.file.len);
    MSG_ASSIGN_REQUIRED_VALUE(msg_send, offset, msg_recv.file.offset);
    MSG_ASSIGN_VALUE(msg_send, processed_byte, processed);
    MSG_SEND_NOCHECK(msg_send, EmmcFile);
    return 0;
}
int process_msg_EmmcFileDelete(uint8_t iface_num, uint32_t msg_size, uint8_t* buf)
{
    MSG_INIT(msg_recv, EmmcFileDelete);
    MSG_RECV_RET_ON_ERR(msg_recv, EmmcFileDelete);

    ExecuteCheck_MSGS_ADV(emmc_fs_file_delete(msg_recv.path), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
      char emmc_fs_status_str[512];
      emmc_fs_format_status(emmc_fs_status_str, 512);
      send_failure_detailed(
        iface_num, FailureType_Failure_ProcessError,
        "%s -> %s Failed \n%s", __func__, str_func_call, emmc_fs_status_str);
#endif

        return -1;
    });

    send_success_nocheck(iface_num, "Succeed");
    return 0;
}

int process_msg_EmmcDirList(uint8_t iface_num, uint32_t msg_size, uint8_t* buf)
{
    MSG_INIT(msg_recv, EmmcDirList);
    MSG_RECV_RET_ON_ERR(msg_recv, EmmcDirList);

    const size_t max_list_len = 8192;

    typedef struct
    {
        // both are '\n' seprated multiline strings
        char list_subdirs[max_list_len];
        char list_files[max_list_len];
    } lists_struct;

    memzero(bl_buffer->misc_buff, SDRAM_BOOLOADER_BUFFER_MISC_LEN);
    lists_struct* temp_buf = (lists_struct*)bl_buffer->misc_buff;

    ExecuteCheck_MSGS_ADV(
        emmc_fs_dir_list(
            msg_recv.path, temp_buf->list_subdirs, max_list_len, temp_buf->list_files, max_list_len
        ),
        true,
        {
#if PRODUCTION
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
            );
#else
            char emmc_fs_status_str[512];
            emmc_fs_format_status(emmc_fs_status_str, 512);
            send_failure_detailed(
                iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed \n%s", __func__, str_func_call,
                emmc_fs_status_str
            );
#endif
            return -1;
        }
    );

    nanopb_callback_args cb_args_send_subdirs = {
        .buffer = (uint8_t*)temp_buf->list_subdirs,
        .buffer_size = max_list_len,
        .payload_size = strlen(temp_buf->list_subdirs),
    };
    nanopb_callback_args cb_args_send_files = {
        .buffer = (uint8_t*)temp_buf->list_files,
        .buffer_size = max_list_len,
        .payload_size = strlen(temp_buf->list_files),
    };

    MSG_INIT(msg_send, EmmcDir);
    MSG_ASSIGN_REQUIRED_STRING(msg_send, path, msg_recv.path);
    MSG_ASSIGN_CALLBACK_ENCODE(msg_send, child_dirs, &callback_encode_bytes, &cb_args_send_subdirs);
    MSG_ASSIGN_CALLBACK_ENCODE(msg_send, child_files, &callback_encode_bytes, &cb_args_send_files);
    MSG_SEND_NOCHECK(msg_send, EmmcDir);

    return 0;
}
int process_msg_EmmcDirMake(uint8_t iface_num, uint32_t msg_size, uint8_t* buf)
{
    MSG_INIT(msg_recv, EmmcDirMake);
    MSG_RECV_RET_ON_ERR(msg_recv, EmmcDirMake);

    ExecuteCheck_MSGS_ADV(emmc_fs_dir_make(msg_recv.path), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
      char emmc_fs_status_str[512];
      emmc_fs_format_status(emmc_fs_status_str, 512);
      send_failure_detailed(
        iface_num, FailureType_Failure_ProcessError,
        "%s -> %s Failed \n%s", __func__, str_func_call, emmc_fs_status_str);
#endif

        return -1;
    });

    send_success_nocheck(iface_num, "Succeed");
    return 0;
}
int process_msg_EmmcDirRemove(uint8_t iface_num, uint32_t msg_size, uint8_t* buf)
{
    MSG_INIT(msg_recv, EmmcDirRemove);
    MSG_RECV_RET_ON_ERR(msg_recv, EmmcDirRemove);

    ExecuteCheck_MSGS_ADV(emmc_fs_dir_delete(msg_recv.path), true, {
#if PRODUCTION
        send_failure_detailed(
            iface_num, FailureType_Failure_ProcessError, "%s -> %s Failed", __func__, str_func_call
        );
#else
      char emmc_fs_status_str[512];
      emmc_fs_format_status(emmc_fs_status_str, 512);
      send_failure_detailed(
        iface_num, FailureType_Failure_ProcessError,
        "%s -> %s Failed \n%s", __func__, str_func_call, emmc_fs_status_str);
#endif

        return -1;
    });

    send_success_nocheck(iface_num, "Succeed");
    return 0;
}

// allow following to be unused
void emmc_commands_dummy()
{
    // EMMC_WRAPPER_UNUSED(bl_buffer);
    EMMC_WRAPPER_UNUSED(callback_encode_bytes);
    EMMC_WRAPPER_UNUSED(callback_decode_bytes);
    EMMC_WRAPPER_UNUSED(send_msg);
    EMMC_WRAPPER_UNUSED(send_success);
    EMMC_WRAPPER_UNUSED(send_failure);
    EMMC_WRAPPER_UNUSED(send_failure_detailed);
    EMMC_WRAPPER_UNUSED(send_user_abort);
}
