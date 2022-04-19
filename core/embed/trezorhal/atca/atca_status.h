#ifndef _ATCA_STATUS_H
#define _ATCA_STATUS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// clang-format off
/* all status codes for the ATCA lib are defined here */

typedef enum
{
    ATCA_SUCCESS                = 0x00, //!< Function succeeded.
    ATCA_CONFIG_ZONE_LOCKED     = 0x01,
    ATCA_DATA_ZONE_LOCKED       = 0x02,
    ATCA_WAKE_FAILED            = 0xD0, //!< response status byte indicates CheckMac failure (status byte = 0x01)
    ATCA_CHECKMAC_VERIFY_FAILED = 0xD1, //!< response status byte indicates CheckMac failure (status byte = 0x01)
    ATCA_PARSE_ERROR            = 0xD2, //!< response status byte indicates parsing error (status byte = 0x03)
    ATCA_STATUS_CRC             = 0xD4, //!< response status byte indicates DEVICE did not receive data properly (status byte = 0xFF)
    ATCA_STATUS_UNKNOWN         = 0xD5, //!< response status byte is unknown
    ATCA_STATUS_ECC             = 0xD6, //!< response status byte is ECC fault (status byte = 0x05)
    ATCA_STATUS_SELFTEST_ERROR  = 0xD7, //!< response status byte is Self Test Error, chip in failure mode (status byte = 0x07)
    ATCA_FUNC_FAIL              = 0xE0, //!< Function could not execute due to incorrect condition / state.
    ATCA_GEN_FAIL               = 0xE1, //!< unspecified error
    ATCA_BAD_PARAM              = 0xE2, //!< bad argument (out of range, null pointer, etc.)
    ATCA_INVALID_ID             = 0xE3, //!< invalid device id, id not set
    ATCA_INVALID_SIZE           = 0xE4, //!< Count value is out of range or greater than buffer size.
    ATCA_RX_CRC_ERROR           = 0xE5, //!< CRC error in data received from device
    ATCA_RX_FAIL                = 0xE6, //!< Timed out while waiting for response. Number of bytes received is > 0.
    ATCA_RX_NO_RESPONSE         = 0xE7, //!< Not an error while the Command layer is polling for a command response.
    ATCA_RESYNC_WITH_WAKEUP     = 0xE8, //!< Re-synchronization succeeded, but only after generating a Wake-up
    ATCA_PARITY_ERROR           = 0xE9, //!< for protocols needing parity
    ATCA_TX_TIMEOUT             = 0xEA, //!< for Microchip PHY protocol, timeout on transmission waiting for master
    ATCA_RX_TIMEOUT             = 0xEB, //!< for Microchip PHY protocol, timeout on receipt waiting for master
    ATCA_TOO_MANY_COMM_RETRIES  = 0xEC, //!< Device did not respond too many times during a transmission. Could indicate no device present.
    ATCA_SMALL_BUFFER           = 0xED, //!< Supplied buffer is too small for data required
    ATCA_COMM_FAIL              = 0xF0, //!< Communication with device failed. Same as in hardware dependent modules.
    ATCA_TIMEOUT                = 0xF1, //!< Timed out while waiting for response. Number of bytes received is 0.
    ATCA_BAD_OPCODE             = 0xF2, //!< opcode is not supported by the device
    ATCA_WAKE_SUCCESS           = 0xF3, //!< received proper wake token
    ATCA_EXECUTION_ERROR        = 0xF4, //!< chip was in a state where it could not execute the command, response status byte indicates command execution error (status byte = 0x0F)
    ATCA_UNIMPLEMENTED          = 0xF5, //!< Function or some element of it hasn't been implemented yet
    ATCA_ASSERT_FAILURE         = 0xF6, //!< Code failed run-time consistency check
    ATCA_TX_FAIL                = 0xF7, //!< Failed to write
    ATCA_NOT_LOCKED             = 0xF8, //!< required zone was not locked
    ATCA_NO_DEVICES             = 0xF9, //!< For protocols that support device discovery (kit protocol), no devices were found
    ATCA_HEALTH_TEST_ERROR      = 0xFA, //!< random number generator health test error
    ATCA_ALLOC_FAILURE          = 0xFB, //!< Couldn't allocate required memory
} ATCA_STATUS;
// clang-format on
#endif
