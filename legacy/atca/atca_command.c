#include "atca_command.h"
#include "atca_hal.h"
#include "atca_status.h"
#include "supervise.h"

void calculate_crc16(uint32_t length, const uint8_t *data, uint8_t *crc16) {
  uint32_t counter;
  uint16_t crc_register = 0;
  uint16_t polynom = 0x8005;
  uint8_t shift_register;
  uint8_t data_bit, crc_bit;

  if (!data) {
    *crc16 = 0;
  }

  for (counter = 0; counter < length; counter++) {
    for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1) {
      data_bit = (data[counter] & shift_register) ? 1 : 0;
      crc_bit = crc_register >> 15;
      crc_register <<= 1;
      if (data_bit != crc_bit) {
        crc_register ^= polynom;
      }
    }
  }
  crc16[0] = (uint8_t)(crc_register & 0x00FF);
  crc16[1] = (uint8_t)(crc_register >> 8);
}

ATCA_STATUS atca_check_crc(uint8_t *response) {
  uint8_t crc[ATCA_CRC_SIZE];
  uint8_t count = response[ATCA_COUNT_IDX];

  count -= ATCA_CRC_SIZE;
  calculate_crc16(count, response, crc);

  return (crc[0] == response[count] && crc[1] == response[count + 1])
             ? ATCA_SUCCESS
             : ATCA_RX_CRC_ERROR;
}

ATCA_STATUS atca_result_check(uint8_t *data) {
  if (data[0] == 0x04)  // error packets are always 4 bytes long
  {
    switch (data[1]) {
      case 0x00:  // No Error
        return ATCA_SUCCESS;
      case 0x01:  // checkmac or verify failed
        return ATCA_CHECKMAC_VERIFY_FAILED;
        break;
      case 0x03:  // command received byte length, opcode or parameter was
                  // illegal
        return ATCA_PARSE_ERROR;
        break;
      case 0x05:  // computation error during ECC processing causing invalid
                  // results
        return ATCA_STATUS_ECC;
        break;
      case 0x07:  // chip is in self test failure mode
        return ATCA_STATUS_SELFTEST_ERROR;
        break;
      case 0x08:  // random number generator health test error
        return ATCA_HEALTH_TEST_ERROR;
      case 0x0f:  // chip can't execute the command
        return ATCA_EXECUTION_ERROR;
        break;
      case 0x11:  // chip was successfully woken up
        return ATCA_WAKE_SUCCESS;
        break;
      case 0xff:  // bad crc found (command not properly received by device) or
                  // other comm error
        return ATCA_STATUS_CRC;
        break;
      default:
        return ATCA_GEN_FAIL;
        break;
    }
  } else {
    return ATCA_SUCCESS;
  }
}

ATCA_STATUS atca_send_packet(ATCAPacket *packet) {
  uint8_t length, *crc;

  // calculat crc16
  length = packet->txsize - ATCA_CRC_SIZE;
  crc = &(packet->txsize) + length;
  calculate_crc16(length, &(packet->txsize), crc);
#ifdef ATCA_INTERFACE_I2C
  // set word address value
  packet->word_address = ATCA_WORD_ADDRESS_COMMAND;
  packet->txsize++;
  return atca_send((uint8_t *)packet, packet->txsize);
#else
  packet->word_address = SWI_FLAG_CMD;
  return atca_send((uint8_t *)packet, packet->txsize + 1);
#endif
}

ATCA_STATUS atca_exec_cmd(ATCAPacket *packet) {
  static ATCA_STATUS status = ATCA_SUCCESS;
  uint8_t rx_len;
  svc_system_irq(0);

  do {
    if ((status = atca_wake()) != ATCA_SUCCESS) {
      break;
    }
    if ((status = atca_send_packet(packet)) != ATCA_SUCCESS) {
      break;
    }

    rx_len = sizeof(packet->data);
    memset(packet->data, 0, rx_len);
    for (int reties = 0; reties < ATCA_MAX_TIME_MSEC / ATCA_FREQUENCY_TIME_MSEC;
         reties++) {
      rx_len = sizeof(packet->data);
      if ((status = atca_receive(packet->data, &rx_len)) == ATCA_SUCCESS) {
        break;
      }
    }
    if (status != ATCA_SUCCESS) {
      break;
    }

    if ((status = atca_check_crc(packet->data)) != ATCA_SUCCESS) {
      break;
    }

    if ((status = atca_result_check(packet->data)) != ATCA_SUCCESS) {
      break;
    }

  } while (0);

  atca_idle();
  svc_system_irq(1);

  return status;
}

/** \brief Compute the address given the zone, slot, block, and offset
 *  \param[in] zone   Zone to get address from. Config(0), OTP(1), or
 *                    Data(2) which requires a slot.
 *  \param[in] slot   Slot Id number for data zone and zero for other zones.
 *  \param[in] block  Block number within the data or configuration or OTP zone
 * . \param[in] offset Offset Number within the block of data or configuration
 * or OTP zone. \param[out] addr  Pointer to the address of data or
 * configuration or OTP zone. \return ATCA_SUCCESS on success, otherwise an
 * error code.
 */
ATCA_STATUS atca_get_addr(uint8_t zone, uint16_t slot, uint8_t block,
                          uint8_t offset, uint16_t *addr) {
  ATCA_STATUS status = ATCA_SUCCESS;
  uint8_t mem_zone = zone & 0x03;

  if (addr == NULL) {
    return ATCA_BAD_PARAM;
  }
  if ((mem_zone != ATCA_ZONE_CONFIG) && (mem_zone != ATCA_ZONE_DATA) &&
      (mem_zone != ATCA_ZONE_OTP)) {
    return ATCA_BAD_PARAM;
  }
  do {
    // Initialize the addr to 00
    *addr = 0;
    // Mask the offset
    offset = offset & (uint8_t)0x07;
    if ((mem_zone == ATCA_ZONE_CONFIG) || (mem_zone == ATCA_ZONE_OTP)) {
      *addr = block << 3;
      *addr |= offset;
    } else  // ATCA_ZONE_DATA
    {
      *addr = slot << 3;
      *addr |= offset;
      *addr |= block << 8;
    }
  } while (0);

  return status;
}

/** \brief Gets the size of the specified zone in bytes.
 *
 * \param[in]  zone  Zone to get size information from. Config(0), OTP(1), or
 *                   Data(2) which requires a slot.
 * \param[in]  slot  If zone is Data(2), the slot to query for size.
 * \param[out] size  Zone size is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_get_zone_size(uint8_t zone, uint16_t slot, size_t *size) {
  ATCA_STATUS status = ATCA_SUCCESS;

  if (size == NULL) {
    return ATCA_BAD_PARAM;
  }

  switch (zone) {
    case ATCA_ZONE_CONFIG:
      *size = 128;
      break;
    case ATCA_ZONE_OTP:
      *size = 64;
      break;
    case ATCA_ZONE_DATA:
      if (slot < 8) {
        *size = 36;
      } else if (slot == 8) {
        *size = 416;
      } else if (slot < 16) {
        *size = 72;
      } else {
        status = ATCA_BAD_PARAM;
      }
      break;
    default:
      status = ATCA_BAD_PARAM;
      break;
  }

  return status;
}

// ATCA COMMAND

/** \brief Issues an Info command, which return internal device information and
 *          can control GPIO and the persistent latch.
 *
 * \param[in]  mode      Selects which mode to be used for info command.
 * \param[in]  param2    Selects the particular fields for the mode.
 * \param[out] out_data  Response from info command (4 bytes). Can be set to
 *                       NULL if not required.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_info_base(uint8_t mode, uint16_t param2, uint8_t *out_data) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  // build an info command
  packet.opcode = ATCA_INFO;
  packet.param1 = mode;
  packet.param2 = param2;
  packet.txsize = INFO_COUNT;

  do {
    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

    if (out_data != NULL && packet.data[ATCA_COUNT_IDX] >= 7) {
      memcpy(out_data, &packet.data[ATCA_RSP_DATA_IDX], 4);
    }
  } while (0);

  return status;
}

/** \brief Executes Random command, which generates a 32 byte random number
 *          from the CryptoAuth device.
 *
 * \param[out] rand_out  32 bytes of random data is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_random(uint8_t *rand_out) {
  ATCA_STATUS status;
  ATCAPacket packet;

  do {
    packet.opcode = ATCA_RANDOM;
    packet.txsize = RANDOM_COUNT;
    packet.param1 = RANDOM_SEED_UPDATE;
    packet.param2 = 0x0000;

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

    if (packet.data[ATCA_COUNT_IDX] != RANDOM_RSP_SIZE) {
      status = ATCA_RX_FAIL;
      break;
    }
    if (rand_out) {
      memcpy(rand_out, &packet.data[ATCA_RSP_DATA_IDX], RANDOM_NUM_SIZE);
    }

  } while (0);
  return status;
}

/** \brief Executes Nonce command, which loads a random or fixed nonce/data
 *          into the device for use by subsequent commands.
 *
 * \param[in]  mode         Controls the mechanism of the internal RNG or fixed
 *                          write.
 * \param[in]  zero         Param2, normally 0, but can be used to indicate a
 *                          nonce calculation mode (bit 15).
 * \param[in]  num_in       Input value to either be included in the nonce
 *                          calculation in random modes (20 bytes) or to be
 *                          written directly (32 bytes or 64 bytes(ATECC608A))
 *                          in pass-through mode.
 * \param[out] rand_out     If using a random mode, the internally generated
 *                          32-byte random number that was used in the nonce
 *                          calculation is returned here. Can be NULL if not
 *                          needed.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_nonce(uint8_t mode, uint16_t zero, const uint8_t *num_in,
                       uint8_t *rand_out) {
  ATCA_STATUS status;
  ATCAPacket packet;

  uint8_t nonce_mode = mode & NONCE_MODE_MASK;

  do {
    packet.opcode = ATCA_NONCE;
    packet.param1 = mode;
    packet.param2 = zero;

    // Copy the right amount of NumIn data
    if ((nonce_mode == NONCE_MODE_SEED_UPDATE ||
         nonce_mode == NONCE_MODE_NO_SEED_UPDATE)) {
      memcpy(packet.data, num_in, NONCE_NUMIN_SIZE);
      packet.txsize = NONCE_COUNT_SHORT;
    } else if (nonce_mode == NONCE_MODE_PASSTHROUGH) {
      if ((mode & NONCE_MODE_INPUT_LEN_MASK) == NONCE_MODE_INPUT_LEN_64) {
        memcpy(packet.data, num_in, 64);
        packet.txsize = NONCE_COUNT_LONG_64;
      } else {
        memcpy(packet.data, num_in, 32);
        packet.txsize = NONCE_COUNT_LONG;
      }
    } else {
      return ATCA_BAD_PARAM;
    }

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

    if (rand_out && (packet.data[ATCA_COUNT_IDX] >= RANDOM_RSP_SIZE)) {
      memcpy(rand_out, &packet.data[ATCA_RSP_DATA_IDX], RANDOM_NUM_SIZE);
    }

  } while (0);
  return status;
}

/** \brief Execute a Nonce command in pass-through mode to load one of the
 *          device's internal buffers with a fixed value.
 *
 * For the ATECC608A, available targets are TempKey (32 or 64 bytes), Message
 * Digest Buffer (32 or 64 bytes), or the Alternate Key Buffer (32 bytes). For
 * all other devices, only TempKey (32 bytes) is available.
 *
 *  \param[in] target       Target device buffer to load. Can be
 *                          NONCE_MODE_TARGET_TEMPKEY,
 *                          NONCE_MODE_TARGET_MSGDIGBUF, or
 *                          NONCE_MODE_TARGET_ALTKEYBUF.
 *  \param[in] num_in       Data to load into the buffer.
 *  \param[in] num_in_size  Size of num_in in bytes. Can be 32 or 64 bytes
 *                          depending on device and target.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_nonce_load(uint8_t target, const uint8_t *num_in,
                            uint16_t num_in_size) {
  uint8_t mode = NONCE_MODE_PASSTHROUGH | (NONCE_MODE_TARGET_MASK & target);

  if (num_in_size == 32) {
    mode |= NONCE_MODE_INPUT_LEN_32;
  } else if (num_in_size == 64) {
    mode |= NONCE_MODE_INPUT_LEN_64;
  } else {
    return ATCA_BAD_PARAM;
  }

  return atca_nonce(mode, 0, num_in, NULL);
}

/** \brief Executes Read command, which reads either 4 or 32 bytes of data from
 *          a given slot, configuration zone, or the OTP zone.
 *
 *   When reading a slot or OTP, data zone must be locked and the slot
 *   configuration must not be secret for a slot to be successfully read.
 *
 *  \param[in]  zone    Zone to be read from device. Options are
 *                      ATCA_ZONE_CONFIG, ATCA_ZONE_OTP, or ATCA_ZONE_DATA.
 *  \param[in]  slot    Slot number for data zone and ignored for other zones.
 *  \param[in]  block   32 byte block index within the zone.
 *  \param[in]  offset  4 byte work index within the block. Ignored for 32 byte
 *                      reads.
 *  \param[out] data    Read data is returned here.
 *  \param[in]  len     Length of the data to be read. Must be either 4 or 32.
 *
 *  returns ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_read_zone(uint8_t zone, uint16_t slot, uint8_t block,
                           uint8_t offset, uint8_t *data, uint8_t len) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;
  uint16_t addr;

  do {
    // Check the input parameters
    if (data == NULL) {
      return ATCA_BAD_PARAM;
    }

    if (len != 4 && len != 32) {
      return ATCA_BAD_PARAM;
    }

    // The get address function checks the remaining variables
    if ((status = atca_get_addr(zone, slot, block, offset, &addr)) !=
        ATCA_SUCCESS) {
      break;
    }

    // If there are 32 bytes to read, then OR the bit into the mode
    if (len == ATCA_BLOCK_SIZE) {
      zone = zone | ATCA_ZONE_READWRITE_32;
    }

    // build a read command
    packet.opcode = ATCA_READ;
    packet.txsize = READ_COUNT;
    packet.param1 = zone;
    packet.param2 = addr;

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

    memcpy(data, &packet.data[1], len);
  } while (0);

  return status;
}

/** \brief Used to read an arbitrary number of bytes from any zone configured
 *          for clear reads.
 *
 * This function will issue the Read command as many times as is required to
 * read the requested data.
 *
 *  \param[in]  zone    Zone to read data from. Option are ATCA_ZONE_CONFIG(0),
 *                      ATCA_ZONE_OTP(1), or ATCA_ZONE_DATA(2).
 *  \param[in]  slot    Slot number to read from if zone is ATCA_ZONE_DATA(2).
 *                      Ignored for all other zones.
 *  \param[in]  offset  Byte offset within the zone to read from.
 *  \param[out] data    Read data is returned here.
 *  \param[in]  length  Number of bytes to read starting from the offset.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_read_bytes_zone(uint8_t zone, uint16_t slot, size_t offset,
                                 uint8_t *data, size_t length) {
  ATCA_STATUS status = ATCA_GEN_FAIL;
  size_t zone_size = 0;
  uint8_t read_buf[32] = {0};
  size_t data_idx = 0;
  size_t cur_block = 0;
  size_t cur_offset = 0;
  uint8_t read_size = ATCA_BLOCK_SIZE;
  size_t read_buf_idx = 0;
  size_t copy_length = 0;
  size_t read_offset = 0;

  if (zone != ATCA_ZONE_CONFIG && zone != ATCA_ZONE_OTP &&
      zone != ATCA_ZONE_DATA) {
    return ATCA_BAD_PARAM;
  }
  if (zone == ATCA_ZONE_DATA && slot > 15) {
    return ATCA_BAD_PARAM;
  }
  if (length == 0) {
    return ATCA_SUCCESS;  // Always succeed reading 0 bytes
  }
  if (data == NULL) {
    return ATCA_BAD_PARAM;
  }

  do {
    status = atca_get_zone_size(zone, slot, &zone_size);
    if (status != ATCA_SUCCESS) {
      break;
    }
    if (offset + length > zone_size) {
      return ATCA_BAD_PARAM;  // Can't read past the end of a zone
    }
    cur_block = offset / ATCA_BLOCK_SIZE;

    while (data_idx < length) {
      if (read_size == ATCA_BLOCK_SIZE &&
          zone_size - cur_block * ATCA_BLOCK_SIZE < ATCA_BLOCK_SIZE) {
        // We have less than a block to read and can't read past the end of the
        // zone, switch to word reads
        read_size = ATCA_WORD_SIZE;
        cur_offset = ((data_idx + offset) / ATCA_WORD_SIZE) %
                     (ATCA_BLOCK_SIZE / ATCA_WORD_SIZE);
      }

      // Read next chunk of data
      status = atca_read_zone(zone, slot, (uint8_t)cur_block,
                              (uint8_t)cur_offset, read_buf, read_size);
      if (status != ATCA_SUCCESS) {
        break;
      }

      // Calculate where in the read buffer we need data from
      read_offset = cur_block * ATCA_BLOCK_SIZE + cur_offset * ATCA_WORD_SIZE;
      if (read_offset < offset) {
        read_buf_idx =
            offset -
            read_offset;  // Read data starts before the requested chunk
      } else {
        read_buf_idx = 0;  // Read data is within the requested chunk
      }
      // Calculate how much data from the read buffer we want to copy
      if (length - data_idx < read_size - read_buf_idx) {
        copy_length = length - data_idx;
      } else {
        copy_length = read_size - read_buf_idx;
      }

      memcpy(&data[data_idx], &read_buf[read_buf_idx], copy_length);
      data_idx += copy_length;
      if (read_size == ATCA_BLOCK_SIZE) {
        cur_block += 1;
      } else {
        cur_offset += 1;
      }
    }
    if (status != ATCA_SUCCESS) {
      break;
    }
  } while (0);

  return status;
}

/** \brief Executes Read command to read the complete device configuration
 *          zone.
 *
 *  \param[out] config_data  Configuration zone data is returned here. 88 bytes
 *                           for ATSHA devices, 128 bytes for ATECC devices.
 *
 *  \returns ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_read_config_zone(uint8_t *config_data) {
  ATCA_STATUS status = ATCA_GEN_FAIL;

  do {
    // Verify the inputs
    if (config_data == NULL) {
      status = ATCA_BAD_PARAM;
      break;
    }

    status = atca_read_bytes_zone(ATCA_ZONE_CONFIG, 0, 0x00, config_data,
                                  ATCA_ECC_CONFIG_SIZE);

    if (status != ATCA_SUCCESS) {
      break;
    }

  } while (0);

  return status;
}

/** \brief Executes UpdateExtra command to update the values of the two
 *          extra bytes within the Configuration zone (bytes 84 and 85).
 *
 * Can also be used to decrement the limited use counter associated with the
 * key in slot NewValue.
 *
 * \param[in] mode       Mode determines what operations the UpdateExtra
 *                      command performs.
 * \param[in] new_value  Value to be written.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_update_extra(uint8_t mode, uint16_t new_value) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  do {
    // Build command
    memset(&packet, 0, sizeof(packet));
    packet.opcode = ATCA_UPDATE_EXTRA;
    packet.param1 = mode;
    packet.param2 = new_value;
    packet.txsize = UPDATE_COUNT;

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

  } while (0);

  return status;
}

/**
 * \brief Executes the Write command, which writes either one four byte word or
 *        a 32-byte block to one of the EEPROM zones on the device. Depending
 *        upon the value of the WriteConfig byte for this slot, the data may be
 *        required to be encrypted by the system prior to being sent to the
 *        device. This command cannot be used to write slots configured as ECC
 *        private keys.
 *
 * \param[in] zone     Zone/Param1 for the write command.
 * \param[in] address  Address/Param2 for the write command.
 * \param[in] value    Plain-text data to be written or cipher-text for
 *                     encrypted writes. 32 or 4 bytes depending on bit 7 in the
 *                     zone.
 * \param[in] mac      MAC required for encrypted writes (32 bytes). Set to NULL
 *                     if not required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_write(uint8_t zone, uint16_t address, const uint8_t *value,
                       const uint8_t *mac) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  if (value == NULL) {
    return ATCA_BAD_PARAM;
  }

  do {
    // Build the write command
    packet.opcode = ATCA_WRITE;
    packet.param1 = zone;
    packet.param2 = address;
    packet.txsize = ATCA_CMD_SIZE_MIN;
    if (zone & ATCA_ZONE_READWRITE_32) {
      // 32-byte write
      memcpy(packet.data, value, 32);
      packet.txsize += ATCA_BLOCK_SIZE;
      // Only 32-byte writes can have a MAC
      if (mac) {
        memcpy(&packet.data[32], mac, 32);
        packet.txsize += WRITE_MAC_SIZE;
      }
    } else {
      // 4-byte write
      memcpy(packet.data, value, 4);
      packet.txsize += ATCA_WORD_SIZE;
    }

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

  } while (0);

  return status;
}

/** \brief Executes the Write command, which writes either 4 or 32 bytes of
 *          data into a device zone.
 *
 *  \param[in] zone    Device zone to write to (0=config, 1=OTP, 2=data).
 *  \param[in] slot    If writing to the data zone, it is the slot to write to,
 *                     otherwise it should be 0.
 *  \param[in] block   32-byte block to write to.
 *  \param[in] offset  4-byte word within the specified block to write to. If
 *                     performing a 32-byte write, this should be 0.
 *  \param[in] data    Data to be written.
 *  \param[in] len     Number of bytes to be written. Must be either 4 or 32.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_write_zone(uint8_t zone, uint16_t slot, uint8_t block,
                            uint8_t offset, const uint8_t *data, uint8_t len) {
  ATCA_STATUS status = ATCA_GEN_FAIL;
  uint16_t addr;

  // Check the input parameters
  if (data == NULL) {
    return ATCA_BAD_PARAM;
  }

  if (len != 4 && len != 32) {
    return ATCA_BAD_PARAM;
  }

  do {
    // The get address function checks the remaining variables
    if ((status = atca_get_addr(zone, slot, block, offset, &addr)) !=
        ATCA_SUCCESS) {
      break;
    }

    // If there are 32 bytes to write, then xor the bit into the mode
    if (len == ATCA_BLOCK_SIZE) {
      zone = zone | ATCA_ZONE_READWRITE_32;
    }

    status = atca_write(zone, addr, data, NULL);

  } while (0);

  return status;
}

/** \brief Executes the Write command, which writes data into the
 *          configuration, otp, or data zones with a given byte offset and
 *          length. Offset and length must be multiples of a word (4 bytes).
 *
 * Config zone must be unlocked for writes to that zone. If data zone is
 * unlocked, only 32-byte writes are allowed to slots and OTP and the offset
 * and length must be multiples of 32 or the write will fail.
 *
 *  \param[in] zone          Zone to write data to: ATCA_ZONE_CONFIG(0),
 *                           ATCA_ZONE_OTP(1), or ATCA_ZONE_DATA(2).
 *  \param[in] slot          If zone is ATCA_ZONE_DATA(2), the slot number to
 *                           write to. Ignored for all other zones.
 *  \param[in] offset_bytes  Byte offset within the zone to write to. Must be
 *                           a multiple of a word (4 bytes).
 *  \param[in] data          Data to be written.
 *  \param[in] length        Number of bytes to be written. Must be a multiple
 *                           of a word (4 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_write_bytes_zone(uint8_t zone, uint16_t slot,
                                  size_t offset_bytes, const uint8_t *data,
                                  size_t length) {
  ATCA_STATUS status = ATCA_GEN_FAIL;
  size_t zone_size = 0;
  size_t data_idx = 0;
  size_t cur_block = 0;
  size_t cur_word = 0;

  if (zone != ATCA_ZONE_CONFIG && zone != ATCA_ZONE_OTP &&
      zone != ATCA_ZONE_DATA) {
    return ATCA_BAD_PARAM;
  }
  if (zone == ATCA_ZONE_DATA && slot > 15) {
    return ATCA_BAD_PARAM;
  }
  if (length == 0) {
    return ATCA_SUCCESS;  // Always succeed writing 0 bytes
  }
  if (data == NULL) {
    return ATCA_BAD_PARAM;
  }
  if (offset_bytes % ATCA_WORD_SIZE != 0 || length % ATCA_WORD_SIZE != 0) {
    return ATCA_BAD_PARAM;
  }

  do {
    status = atca_get_zone_size(zone, slot, &zone_size);
    if (status != ATCA_SUCCESS) {
      break;
    }
    if (offset_bytes + length > zone_size) {
      return ATCA_BAD_PARAM;
    }

    cur_block = offset_bytes / ATCA_BLOCK_SIZE;
    cur_word = (offset_bytes % ATCA_BLOCK_SIZE) / ATCA_WORD_SIZE;

    while (data_idx < length) {
      // The last item makes sure we handle the selector, user extra, and lock
      // bytes in the config properly
      if (cur_word == 0 && length - data_idx >= ATCA_BLOCK_SIZE &&
          !(zone == ATCA_ZONE_CONFIG && cur_block == 2)) {
        status = atca_write_zone(zone, slot, (uint8_t)cur_block, 0,
                                 &data[data_idx], ATCA_BLOCK_SIZE);
        if (status != ATCA_SUCCESS) {
          break;
        }
        data_idx += ATCA_BLOCK_SIZE;
        cur_block += 1;
      } else {
        // Skip trying to change UserExtra, Selector, LockValue, and LockConfig
        // which require the UpdateExtra command to change
        if (!(zone == ATCA_ZONE_CONFIG && cur_block == 2 && cur_word == 5)) {
          status =
              atca_write_zone(zone, slot, (uint8_t)cur_block, (uint8_t)cur_word,
                              &data[data_idx], ATCA_WORD_SIZE);
          if (status != ATCA_SUCCESS) {
            break;
          }
        }
        data_idx += ATCA_WORD_SIZE;
        cur_word += 1;
        if (cur_word == ATCA_BLOCK_SIZE / ATCA_WORD_SIZE) {
          cur_block += 1;
          cur_word = 0;
        }
      }
    }
  } while (0);

  return status;
}

/** \brief Executes the Write command, which writes the configuration zone.
 *
 *  First 16 bytes are skipped as they are not writable. LockValue and
 *  LockConfig are also skipped and can only be changed via the Lock
 *  command.
 *
 *  This command may fail if UserExtra and/or Selector bytes have
 *  already been set to non-zero values.
 *
 *  \param[in] config_data  Data to the config zone data. This should be 88
 *
 * bytes for SHA devices and 128 bytes for ECC
 *                          devices.
 *
 *  \returns ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_write_config_zone(const uint8_t *config_data) {
  ATCA_STATUS status = ATCA_GEN_FAIL;
  size_t config_size = 0;

  if (config_data == NULL) {
    return ATCA_BAD_PARAM;
  }

  do {
    // Get config zone size for the device
    status = atca_get_zone_size(ATCA_ZONE_CONFIG, 0, &config_size);
    if (status != ATCA_SUCCESS) {
      break;
    }

    // Write config zone excluding UserExtra and Selector
    status = atca_write_bytes_zone(ATCA_ZONE_CONFIG, 0, 16, &config_data[16],
                                   config_size - 16);
    if (status != ATCA_SUCCESS) {
      break;
    }

  } while (0);

  return status;
}

/** \brief Compute the Counter functions
 *  \param[in]  mode the mode used for the counter
 *  \param[in]  counter_id The counter to be used
 *  \param[out] counter_value pointer to the counter value returned from device
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS atca_counter(uint8_t mode, uint16_t counter_id,
                         uint32_t *counter_value) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  do {
    if (counter_id > 1) {
      return ATCA_BAD_PARAM;
    }

    // build a Counter command
    packet.opcode = ATCA_COUNTER;
    packet.param1 = mode;
    packet.param2 = counter_id;
    packet.txsize = COUNTER_COUNT;

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

    if (counter_value != NULL && packet.data[ATCA_COUNT_IDX] >= 7) {
      *counter_value = ((uint32_t)packet.data[ATCA_RSP_DATA_IDX + 0] << 0) |
                       ((uint32_t)packet.data[ATCA_RSP_DATA_IDX + 1] << 8) |
                       ((uint32_t)packet.data[ATCA_RSP_DATA_IDX + 2] << 16) |
                       ((uint32_t)packet.data[ATCA_RSP_DATA_IDX + 3] << 24);
    }
  } while (0);

  return status;
}

/** \brief Increments one of the device's monotonic counters
 *  \param[in]  counter_id     Counter to be incremented
 *  \param[out] counter_value  New value of the counter is returned here. Can be
 *                             NULL if not needed.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_counter_increment(uint16_t counter_id,
                                   uint32_t *counter_value) {
  return atca_counter(COUNTER_MODE_INCREMENT, counter_id, counter_value);
}

/** \brief Read one of the device's monotonic counters
 *  \param[in]  counter_id     Counter to be read
 *  \param[out] counter_value  Counter value is returned here.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_counter_read(uint16_t counter_id, uint32_t *counter_value) {
  return atca_counter(COUNTER_MODE_READ, counter_id, counter_value);
}

/** \brief Issues GenKey command, which can generate a private key, compute a
 *          public key, nd/or compute a digest of a public key.
 *
 * \param[in]  mode        Mode determines what operations the GenKey
 *                         command performs.
 * \param[in]  key_id      Slot to perform the GenKey command on.
 * \param[in]  other_data  OtherData for PubKey digest calculation. Can be set
 *                         to NULL otherwise.
 * \param[out] public_key  If the mode indicates a public key will be
 *                         calculated, it will be returned here. Format will
 *                         be the X and Y integers in big-endian format.
 *                         64 bytes for P256 curve. Set to NULL if public key
 *                         isn't required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_genkey_base(uint8_t mode, uint16_t key_id,
                             const uint8_t *other_data, uint8_t *public_key) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  do {
    // Build GenKey command
    packet.opcode = ATCA_GENKEY;
    packet.param1 = mode;
    packet.param2 = key_id;
    packet.txsize = GENKEY_COUNT;
    if (other_data) {
      memcpy(packet.data, other_data, GENKEY_OTHER_DATA_SIZE);
      packet.txsize = GENKEY_COUNT_DATA;
    }

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

    if (public_key && packet.data[ATCA_COUNT_IDX] > 4) {
      memcpy(public_key, &packet.data[ATCA_RSP_DATA_IDX],
             packet.data[ATCA_COUNT_IDX] - 3);
    }
  } while (0);

  return status;
}

/** \brief Issues GenKey command, which generates a new random private key in
 *          slot and returns the public key.
 *
 * \param[in]  key_id      Slot number where an ECC private key is configured.
 *                         Can also be ATCA_TEMPKEY_KEYID to generate a private
 *                         key in TempKey.
 * \param[out] public_key  Public key will be returned here. Format will be
 *                         the X and Y integers in big-endian format.
 *                         64 bytes for P256 curve. Set to NULL if public key
 *                         isn't required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_genkey(uint16_t key_id, uint8_t *public_key) {
  return atca_genkey_base(GENKEY_MODE_PRIVATE, key_id, NULL, public_key);
}

/** \brief Uses GenKey command to calculate the public key from an existing
 *          private key in a slot.
 *
 *  \param[in]  key_id      Slot number of the private key.
 *  \param[out] public_key  Public key will be returned here. Format will be
 *                          the X and Y integers in big-endian format.
 *                          64 bytes for P256 curve. Set to NULL if public key
 *                          isn't required.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_get_pubkey(uint16_t key_id, uint8_t *public_key) {
  return atca_genkey_base(GENKEY_MODE_PUBLIC, key_id, NULL, public_key);
}

/** \brief The Lock command prevents future modifications of the Configuration
 *         and/or Data and OTP zones. If the device is so configured, then
 *         this command can be used to lock individual data slots. This
 *         command fails if the designated area is already locked.
 *
 * \param[in]  mode           Zone, and/or slot, and summary check (bit 7).
 * \param[in]  summary_crc    CRC of the config or data zones. Ignored for
 *                            slot locks or when mode bit 7 is set.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_lock(uint8_t mode, uint16_t summary_crc) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  // build command for lock zone and send
  memset(&packet, 0, sizeof(packet));
  packet.opcode = ATCA_LOCK;
  packet.param1 = mode;
  packet.param2 = summary_crc;
  packet.txsize = LOCK_COUNT;

  do {
    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

  } while (0);

  return status;
}

/** \brief Unconditionally (no CRC required) lock the config zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_lock_config_zone(void) {
  return atca_lock(LOCK_ZONE_NO_CRC | LOCK_ZONE_CONFIG, 0);
}

/** \brief Lock the config zone with summary CRC.
 *
 *  The CRC is calculated over the entire config zone contents. 88 bytes for
 *  ATSHA devices, 128 bytes for ATECC devices. Lock will fail if the provided
 *  CRC doesn't match the internally calculated one.
 *
 *  \param[in] summary_crc  Expected CRC over the config zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_lock_config_zone_crc(uint16_t summary_crc) {
  return atca_lock(LOCK_ZONE_CONFIG, summary_crc);
}

/** \brief Unconditionally (no CRC required) lock the data zone (slots and OTP).
 *
 *	ConfigZone must be locked and DataZone must be unlocked for the zone to
 *be successfully locked.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_lock_data_zone(void) {
  return atca_lock(LOCK_ZONE_NO_CRC | LOCK_ZONE_DATA, 0);
}

/** \brief Lock the data zone (slots and OTP) with summary CRC.
 *
 *  The CRC is calculated over the concatenated contents of all the slots and
 *  OTP at the end. Private keys (KeyConfig.Private=1) are skipped. Lock will
 *  fail if the provided CRC doesn't match the internally calculated one.
 *
 *  \param[in] summary_crc  Expected CRC over the data zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_lock_data_zone_crc(uint16_t summary_crc) {
  return atca_lock(LOCK_ZONE_DATA, summary_crc);
}

/** \brief Lock an individual slot in the data zone on an ATECC device. Not
 *         available for ATSHA devices. Slot must be configured to be slot
 *         lockable (KeyConfig.Lockable=1).
 *
 *  \param[in] slot  Slot to be locked in data zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_lock_data_slot(uint16_t slot) {
  return atca_lock(((uint8_t)slot << 2) | LOCK_ZONE_DATA_SLOT, 0);
}

/** \brief Issues a GenDig command, which performs a SHA256 hash on the source
 * data indicated by zone with the contents of TempKey.  See the CryptoAuth
 * datasheet for your chip to see what the values of zone correspond to.
 *  \param[in] zone             Designates the source of the data to hash with
 * TempKey. \param[in] key_id           Indicates the key, OTP block, or message
 * order for shared nonce mode. \param[in] other_data       Four bytes of data
 * for SHA calculation when using a NoMac key, 32 bytes for "Shared Nonce" mode,
 * otherwise ignored (can be NULL). \param[in] other_data_size  Size of
 * other_data in bytes. \return ATCA_SUCCESS on success, otherwise an error
 * code.
 */
ATCA_STATUS atca_gendig(uint8_t zone, uint16_t key_id,
                        const uint8_t *other_data, uint8_t other_data_size) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  if (other_data_size > 0 && other_data == NULL) {
    return ATCA_BAD_PARAM;
  }

  do {
    // build gendig command
    packet.opcode = ATCA_GENDIG;
    packet.param1 = zone;
    packet.param2 = key_id;
    packet.txsize = GENDIG_COUNT;

    if (packet.param1 == GENDIG_ZONE_SHARED_NONCE &&
        other_data_size >= ATCA_BLOCK_SIZE) {
      memcpy(&packet.data[0], &other_data[0], ATCA_BLOCK_SIZE);
      packet.txsize = GENDIG_COUNT + ATCA_BLOCK_SIZE;
    } else if (packet.param1 == GENDIG_ZONE_DATA &&
               other_data_size >= ATCA_WORD_SIZE) {
      memcpy(&packet.data[0], &other_data[0], ATCA_WORD_SIZE);
      packet.txsize = GENDIG_COUNT + ATCA_WORD_SIZE;
    }

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

  } while (0);

  return status;
}

/** \brief Compares a MAC response with input values
 *	\param[in] mode        Controls which fields within the device are used
 *in the message \param[in] key_id      Key location in the CryptoAuth device to
 *use for the MAC \param[in] challenge   Challenge data (32 bytes) \param[in]
 *response    MAC response data (32 bytes) \param[in] other_data  OtherData
 *parameter (13 bytes) \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS atca_checkmac(uint8_t mode, uint16_t key_id,
                          const uint8_t *challenge, const uint8_t *response,
                          const uint8_t *other_data) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  // Verify the inputs
  if (response == NULL || other_data == NULL) {
    return ATCA_BAD_PARAM;
  }
  if (!(mode & CHECKMAC_MODE_BLOCK2_TEMPKEY) && challenge == NULL) {
    return ATCA_BAD_PARAM;
  }

  do {
    // build Check MAC command
    packet.opcode = ATCA_CHECKMAC;
    packet.param1 = mode;
    packet.param2 = key_id;
    packet.txsize = CHECKMAC_COUNT;
    if (challenge != NULL) {
      memcpy(&packet.data[0], challenge, CHECKMAC_CLIENT_CHALLENGE_SIZE);
    } else {
      memset(&packet.data[0], 0, CHECKMAC_CLIENT_CHALLENGE_SIZE);
    }
    memcpy(&packet.data[32], response, CHECKMAC_CLIENT_RESPONSE_SIZE);
    memcpy(&packet.data[64], other_data, CHECKMAC_OTHER_DATA_SIZE);

    if ((status = atca_exec_cmd((void *)&packet)) != ATCA_SUCCESS) {
      break;
    }
  } while (0);

  return status;
}

/** \brief Executes MAC command, which computes a SHA-256 digest of a key
 *          stored in the device, a challenge, and other information on the
 *          device.
 *
 *	\param[in]  mode       Controls which fields within the device are used
 *in the message \param[in]  key_id     Key in the CryptoAuth device to use for
 *the MAC \param[in]  challenge  Challenge message (32 bytes). May be NULL if
 *mode indicates a challenge isn't required. \param[out] digest     MAC response
 *is returned here (32 bytes).
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_mac(uint8_t mode, uint16_t key_id, const uint8_t *challenge,
                     uint8_t *digest) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  do {
    if (digest == NULL) {
      return ATCA_BAD_PARAM;
    }

    // build mac command
    packet.opcode = ATCA_MAC;
    packet.param1 = mode;
    packet.param2 = key_id;
    packet.txsize = MAC_COUNT_SHORT;
    if (!(mode & MAC_MODE_BLOCK2_TEMPKEY)) {
      if (challenge == NULL) {
        return ATCA_BAD_PARAM;
      }
      memcpy(&packet.data[0], challenge, 32);  // a 32-byte challenge
      packet.txsize = MAC_COUNT_LONG;
    }

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

    memcpy(digest, &packet.data[ATCA_RSP_DATA_IDX], MAC_SIZE);

  } while (0);

  return status;
}

/** \brief Executes SHA command, which computes a SHA-256 or HMAC/SHA-256
 *          digest for general purpose use by the host system.
 *
 * Only the Start(0) and Compute(1) modes are available for ATSHA devices.
 *
 * \param[in]    mode           SHA command mode Start(0), Update/Compute(1),
 *                              End(2), Public(3), HMACstart(4), HMACend(5),
 *                              Read_Context(6), or Write_Context(7). Also
 *                              message digest target location for the
 *                              ATECC608A.
 * \param[in]    length         Number of bytes in the message parameter or
 *                              KeySlot for the HMAC key if Mode is
 *                              HMACstart(4) or Public(3).
 * \param[in]    message        Message bytes to be hashed or Write_Context if
 *                              restoring a context on the ATECC608A. Can be
 *                              NULL if not required by the mode.
 * \param[out]   data_out       Data returned by the command (digest or
 *                              context).
 * \param[inout] data_out_size  As input, the size of the data_out buffer. As
 *                              output, the number of bytes returned in
 *                              data_out.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_sha_base(uint8_t mode, uint16_t length, const uint8_t *message,
                          uint8_t *data_out, uint16_t *data_out_size) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;
  uint8_t cmd_mode = (mode & SHA_MODE_MASK);

  if (cmd_mode != SHA_MODE_SHA256_PUBLIC && cmd_mode != SHA_MODE_HMAC_START &&
      length > 0 && message == NULL) {
    return ATCA_BAD_PARAM;  // message data indicated, but nothing provided
  }
  if (data_out != NULL && data_out_size == NULL) {
    return ATCA_BAD_PARAM;
  }

  do {
    // Build Command
    packet.opcode = ATCA_SHA;
    packet.param1 = mode;
    packet.param2 = length;
    packet.txsize = ATCA_CMD_SIZE_MIN + length;

    if (cmd_mode != SHA_MODE_SHA256_PUBLIC && cmd_mode != SHA_MODE_HMAC_START) {
      memcpy(packet.data, message, length);
    }

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

    if ((data_out != NULL) && (packet.data[ATCA_COUNT_IDX] > 4)) {
      if (packet.data[ATCA_COUNT_IDX] - ATCA_PACKET_OVERHEAD > *data_out_size) {
        status = ATCA_SMALL_BUFFER;
        break;
      }
      *data_out_size = packet.data[ATCA_COUNT_IDX] - ATCA_PACKET_OVERHEAD;
      memcpy(data_out, &packet.data[ATCA_RSP_DATA_IDX], *data_out_size);
    }
  } while (0);

  return status;
}

/** \brief Executes SHA command to start an HMAC/SHA-256 operation
 *
 * \param[in] ctx       HMAC/SHA-256 context
 * \param[in] key_slot  Slot key id to use for the HMAC calculation
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_sha_hmac_init(atca_hmac_sha256_ctx_t *ctx, uint16_t key_slot) {
  memset(ctx, 0, sizeof(*ctx));
  return atca_sha_base(SHA_MODE_HMAC_START, key_slot, NULL, NULL, NULL);
}

/** \brief Executes SHA command to add an arbitrary amount of message data to
 *          a HMAC/SHA-256 operation.
 *
 * \param[in] ctx        HMAC/SHA-256 context
 * \param[in] data       Message data to add
 * \param[in] data_size  Size of message data in bytes
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_sha_hmac_update(atca_hmac_sha256_ctx_t *ctx,
                                 const uint8_t *data, size_t data_size) {
  ATCA_STATUS status = ATCA_SUCCESS;
  uint32_t block_count;
  uint32_t rem_size = ATCA_SHA256_BLOCK_SIZE - ctx->block_size;
  uint32_t copy_size = data_size > rem_size ? rem_size : (uint32_t)data_size;
  uint32_t i = 0;

  // Copy data into current block
  memcpy(&ctx->block[ctx->block_size], data, copy_size);

  if (ctx->block_size + data_size < ATCA_SHA256_BLOCK_SIZE) {
    // Not enough data to finish off the current block
    ctx->block_size += (uint32_t)data_size;
    return ATCA_SUCCESS;
  }

  // Process the current block
  status = atca_sha_base(SHA_MODE_HMAC_UPDATE, ATCA_SHA256_BLOCK_SIZE,
                         ctx->block, NULL, NULL);
  if (status != ATCA_SUCCESS) {
    return status;
  }

  // Process any additional blocks
  data_size -= copy_size;  // Adjust to the remaining message bytes
  block_count = (uint32_t)(data_size / ATCA_SHA256_BLOCK_SIZE);
  for (i = 0; i < block_count; i++) {
    status = atca_sha_base(SHA_MODE_HMAC_UPDATE, ATCA_SHA256_BLOCK_SIZE,
                           &data[copy_size + i * ATCA_SHA256_BLOCK_SIZE], NULL,
                           NULL);
    if (status != ATCA_SUCCESS) {
      return status;
    }
  }

  // Save any remaining data
  ctx->total_msg_size += (block_count + 1) * ATCA_SHA256_BLOCK_SIZE;
  ctx->block_size = data_size % ATCA_SHA256_BLOCK_SIZE;
  memcpy(ctx->block, &data[copy_size + block_count * ATCA_SHA256_BLOCK_SIZE],
         ctx->block_size);

  return ATCA_SUCCESS;
}

/** \brief Executes SHA command to complete a HMAC/SHA-256 operation.
 *
 * \param[in]  ctx     HMAC/SHA-256 context
 * \param[out] digest  HMAC/SHA-256 result is returned here (32 bytes).
 * \param[in]  target  Where to save the digest internal to the device.
 *                     For ATECC608A, can be SHA_MODE_TARGET_TEMPKEY,
 *                     SHA_MODE_TARGET_MSGDIGBUF, or SHA_MODE_TARGET_OUT_ONLY.
 *                     For all other devices, SHA_MODE_TARGET_TEMPKEY is the
 *                     only option.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_sha_hmac_finish(atca_hmac_sha256_ctx_t *ctx, uint8_t *digest,
                                 uint8_t target) {
  uint8_t mode = SHA_MODE_608_HMAC_END;
  uint16_t digest_size = 32;

  mode |= target;

  return atca_sha_base(mode, ctx->block_size, ctx->block, digest, &digest_size);
}

/** \brief Executes the Sign command, which generates a signature using the
 *          ECDSA algorithm.
 *
 * \param[in]  mode       Mode determines what the source of the message to be
 *                        signed.
 * \param[in]  key_id     Private key slot used to sign the message.
 * \param[out] signature  Signature is returned here. Format is R and S
 *                        integers in big-endian format. 64 bytes for P256
 *                        curve.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_sign_base(uint8_t mode, uint16_t key_id, uint8_t *signature) {
  ATCAPacket packet;
  ATCA_STATUS status = ATCA_GEN_FAIL;

  if (signature == NULL) {
    return ATCA_BAD_PARAM;
  }

  do {
    // Build sign command
    packet.opcode = ATCA_SIGN;
    packet.param1 = mode;
    packet.param2 = key_id;
    packet.txsize = SIGN_COUNT;

    if ((status = atca_exec_cmd(&packet)) != ATCA_SUCCESS) {
      break;
    }

    if (packet.data[ATCA_COUNT_IDX] > 4) {
      memcpy(signature, &packet.data[ATCA_RSP_DATA_IDX],
             packet.data[ATCA_COUNT_IDX] - ATCA_PACKET_OVERHEAD);
    }

  } while (0);

  return status;
}

/** \brief Executes Sign command, to sign a 32-byte external message using the
 *                   private key in the specified slot. The message to be signed
 *                   will be loaded into the Message Digest Buffer to the
 *                   ATECC608A device or TempKey for other devices.
 *
 *  \param[in]  key_id     Slot of the private key to be used to sign the
 *                         message.
 *  \param[in]  msg        32-byte message to be signed. Typically the SHA256
 *                         hash of the full message.
 *  \param[out] signature  Signature will be returned here. Format is R and S
 *                         integers in big-endian format. 64 bytes for P256
 *                         curve.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atca_sign_extern(uint16_t key_id, const uint8_t *msg,
                             uint8_t *signature) {
  ATCA_STATUS status = ATCA_GEN_FAIL;
  uint8_t nonce_target = NONCE_MODE_TARGET_TEMPKEY;
  uint8_t sign_source = SIGN_MODE_SOURCE_TEMPKEY;

  do {
    // Make sure RNG has updated its seed
    if ((status = atca_random(NULL)) != ATCA_SUCCESS) {
      break;
    }

    nonce_target = NONCE_MODE_TARGET_MSGDIGBUF;
    sign_source = SIGN_MODE_SOURCE_MSGDIGBUF;

    if ((status = atca_nonce_load(nonce_target, msg, 32)) != ATCA_SUCCESS) {
      break;
    }

    // Sign the message
    if ((status = atca_sign_base(SIGN_MODE_EXTERNAL | sign_source, key_id,
                                 signature)) != ATCA_SUCCESS) {
      break;
    }
  } while (0);

  return status;
}
