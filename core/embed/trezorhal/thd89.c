/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include STM32_HAL_H

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "irq.h"
#include "memzero.h"

#include "thd89.h"

#define THD89_ADDRESS (0x10 << 1)

static I2C_HandleTypeDef i2c_handle;

static uint8_t sw1 = 0, sw2 = 0;

static void _i2c_msp_init(void) {
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed =
      GPIO_SPEED_FREQ_LOW;  // I2C is a KHz bus and low speed is still good
                            // into the low MHz
  GPIO_InitStructure.Alternate = GPIO_AF4_I2C4;
  GPIO_InitStructure.Pin = GPIO_PIN_12 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

  __HAL_RCC_I2C4_CLK_ENABLE();
  // __HAL_RCC_I2C4_FORCE_RESET();
  // __HAL_RCC_I2C4_RELEASE_RESET();
}

static void _i2c_init(void) {
  if (i2c_handle.Instance) {
    return;
  }
  i2c_handle.Instance = I2C4;

  i2c_handle.Init.Timing = 0x10C0ECff;
  i2c_handle.Init.OwnAddress1 = 0;  // master
  i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  i2c_handle.Init.OwnAddress2 = 0;
  i2c_handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  i2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_OK != HAL_I2C_Init(&i2c_handle)) {
    ensure(secfalse, NULL);
    return;
  }

  if (HAL_I2CEx_ConfigAnalogFilter(&i2c_handle, I2C_ANALOGFILTER_ENABLE) !=
      HAL_OK) {
    ensure(secfalse, NULL);
    return;
  } else {
    if (HAL_I2CEx_ConfigDigitalFilter(&i2c_handle, 0) != HAL_OK) {
      ensure(secfalse, NULL);
      return;
    }
  }
}

void thd89_init(void) {
  _i2c_msp_init();
  _i2c_init();
}

static uint8_t xor_check(uint8_t init, uint8_t *data, uint16_t len) {
  uint16_t i;
  uint8_t xor ;

  xor = init;
  for (i = 0; i < len; i++) {
    xor ^= data[i];
  }
  return xor;
}

#define I2C_TIMEOUT_BUSY (25U) /*!< 25 ms */
#define MAX_NBYTE_SIZE 255U

static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c,
                                                    uint32_t Flag,
                                                    FlagStatus Status,
                                                    uint32_t Timeout,
                                                    uint32_t Tickstart) {
  while (__HAL_I2C_GET_FLAG(hi2c, Flag) == Status) {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY) {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U)) {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
        return HAL_ERROR;
      }
    }
  }
  return HAL_OK;
}

static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c,
                                                 uint32_t Timeout,
                                                 uint32_t Tickstart) {
  if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET) {
    /* Wait until STOP Flag is reset */
    /* AutoEnd should be initiate after AF */
    while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET) {
      /* Check for the Timeout */
      if (Timeout != HAL_MAX_DELAY) {
        if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U)) {
          hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
          hi2c->State = HAL_I2C_STATE_READY;
          hi2c->Mode = HAL_I2C_MODE_NONE;

          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);

          return HAL_ERROR;
        }
      }
    }

    /* Clear NACKF Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_ERROR;
  }
  return HAL_OK;
}

static HAL_StatusTypeDef I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c,
                                                        uint32_t Timeout,
                                                        uint32_t Tickstart) {
  while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == RESET) {
    /* Check if a NACK is detected */
    if (I2C_IsAcknowledgeFailed(hi2c, Timeout, Tickstart) != HAL_OK) {
      return HAL_ERROR;
    }

    /* Check if a STOPF is detected */
    if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET) {
      /* Check if an RXNE is pending */
      /* Store Last receive data if any */
      if ((__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET) &&
          (hi2c->XferSize > 0U)) {
        /* Return HAL_OK */
        /* The Reading of data from RXDR will be done in caller function */
        return HAL_OK;
      } else {
        /* Clear STOP Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

        /* Clear Configuration Register 2 */
        I2C_RESET_CR2(hi2c);

        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_ERROR;
      }
    }

    /* Check for the Timeout */
    if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U)) {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      hi2c->State = HAL_I2C_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

static HAL_StatusTypeDef I2C_WaitOnSTOPFlagUntilTimeout(I2C_HandleTypeDef *hi2c,
                                                        uint32_t Timeout,
                                                        uint32_t Tickstart) {
  while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET) {
    /* Check if a NACK is detected */
    if (I2C_IsAcknowledgeFailed(hi2c, Timeout, Tickstart) != HAL_OK) {
      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U)) {
      hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
      hi2c->State = HAL_I2C_STATE_READY;
      hi2c->Mode = HAL_I2C_MODE_NONE;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

static HAL_StatusTypeDef I2C_WaitOnTXISFlagUntilTimeout(I2C_HandleTypeDef *hi2c,
                                                        uint32_t Timeout,
                                                        uint32_t Tickstart) {
  while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) == RESET) {
    /* Check if a NACK is detected */
    if (I2C_IsAcknowledgeFailed(hi2c, Timeout, Tickstart) != HAL_OK) {
      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY) {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U)) {
        hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_ERROR;
      }
    }
  }
  return HAL_OK;
}

static void I2C_TransferConfig(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                               uint8_t Size, uint32_t Mode, uint32_t Request) {
  /* Check the parameters */
  assert_param(IS_I2C_ALL_INSTANCE(hi2c->Instance));
  assert_param(IS_TRANSFER_MODE(Mode));
  assert_param(IS_TRANSFER_REQUEST(Request));

  /* update CR2 register */
  MODIFY_REG(
      hi2c->Instance->CR2,
      ((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND |
        (I2C_CR2_RD_WRN & (uint32_t)(Request >> (31U - I2C_CR2_RD_WRN_Pos))) |
        I2C_CR2_START | I2C_CR2_STOP)),
      (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) |
                 (((uint32_t)Size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) |
                 (uint32_t)Mode | (uint32_t)Request));
}

HAL_StatusTypeDef i2c_master_send(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                  uint8_t *pData, uint16_t Size,
                                  uint32_t Timeout) {
  uint32_t tickstart;
  uint8_t data[2];
  uint8_t xor = 0;

  if (hi2c->State == HAL_I2C_STATE_READY) {
    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY,
                                   tickstart) != HAL_OK) {
      return HAL_ERROR;
    }

    hi2c->State = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr = pData;
    hi2c->XferCount = Size;
    hi2c->XferISR = NULL;

    data[0] = (Size >> 8) & 0xff;
    data[1] = Size & 0xff;

    xor = xor_check(0, data, 2);
    xor = xor_check(xor, pData, Size);

    I2C_TransferConfig(hi2c, DevAddress, 2, I2C_RELOAD_MODE,
                       I2C_GENERATE_START_WRITE);

    for (uint8_t i = 0; i < 2; i++) {
      if (I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK) {
        return HAL_ERROR;
      }
      hi2c->Instance->TXDR = data[i];
    }

    /* Wait until TCR flag is set */
    if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout,
                                   tickstart) != HAL_OK) {
      return HAL_ERROR;
    }

    /* Send Slave Address */
    /* Set NBYTES to write and reload if hi2c->XferCount > MAX_NBYTE_SIZE and
     * generate RESTART */

    hi2c->XferSize =
        hi2c->XferCount > MAX_NBYTE_SIZE ? MAX_NBYTE_SIZE : hi2c->XferCount;
    I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize,
                       I2C_RELOAD_MODE, I2C_NO_STARTSTOP);

    while (hi2c->XferCount > 0U) {
      /* Wait until TXIS flag is set */
      if (I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK) {
        return HAL_ERROR;
      }
      /* Write data to TXDR */
      hi2c->Instance->TXDR = *hi2c->pBuffPtr;

      /* Increment Buffer pointer */
      hi2c->pBuffPtr++;

      hi2c->XferCount--;
      hi2c->XferSize--;

      if ((hi2c->XferCount != 0U) && (hi2c->XferSize == 0U)) {
        /* Wait until TCR flag is set */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout,
                                       tickstart) != HAL_OK) {
          return HAL_ERROR;
        }

        hi2c->XferSize =
            hi2c->XferCount > MAX_NBYTE_SIZE ? MAX_NBYTE_SIZE : hi2c->XferCount;
        I2C_TransferConfig(hi2c, DevAddress, (uint8_t)hi2c->XferSize,
                           I2C_RELOAD_MODE, I2C_NO_STARTSTOP);
      }
    }

    // send xor
    /* Wait until TCR flag is set */
    if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_TCR, RESET, Timeout,
                                   tickstart) != HAL_OK) {
      return HAL_ERROR;
    }

    I2C_TransferConfig(hi2c, DevAddress, 1, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
    /* Wait until TXIS flag is set */
    if (I2C_WaitOnTXISFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK) {
      return HAL_ERROR;
    }
    /* Write data to TXDR */
    hi2c->Instance->TXDR = xor;

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically
     * generated */
    /* Wait until STOPF flag is set */
    if (I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK) {
      return HAL_ERROR;
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  } else {
    return HAL_BUSY;
  }
}

HAL_StatusTypeDef i2c_master_recive(I2C_HandleTypeDef *hi2c,
                                    uint16_t DevAddress, uint8_t *pData,
                                    uint16_t *Size, uint32_t Timeout) {
  uint32_t tickstart, tickstart1;
  uint8_t data[4];
  uint16_t data_len;
  uint8_t *data_ptr = pData;
  uint8_t xor = 0x00;
  sw1 = sw2 = 0;

  if (hi2c->State == HAL_I2C_STATE_READY) {
    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    while (1) {
      /* Check for the Timeout */
      if (Timeout != HAL_MAX_DELAY) {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U)) {
          hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;
          hi2c->State = HAL_I2C_STATE_READY;
          hi2c->Mode = HAL_I2C_MODE_NONE;

          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);

          return HAL_ERROR;
        }
      }

      tickstart1 = HAL_GetTick();
      if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY,
                                     tickstart1) != HAL_OK) {
        return HAL_ERROR;
      }

      hi2c->State = HAL_I2C_STATE_BUSY_RX;
      hi2c->Mode = HAL_I2C_MODE_MASTER;
      hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

      hi2c->XferISR = NULL;

      // send start
      I2C_TransferConfig(hi2c, DevAddress, 2, I2C_RELOAD_MODE,
                         I2C_GENERATE_START_READ);

      tickstart1 = HAL_GetTick();
      if (I2C_WaitOnRXNEFlagUntilTimeout(hi2c, 5, tickstart1) != HAL_OK) {
        hal_delay(3);
        continue;
      }
      data[0] = (uint8_t)hi2c->Instance->RXDR;

      if (I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart1) != HAL_OK) {
        return HAL_ERROR;
      }
      data[1] = (uint8_t)hi2c->Instance->RXDR;

      data_len = (data[0] << 8) + data[1];
      *Size = data_len -= 2;

      xor = xor_check(0, data, 2);
      while (data_len > 0) {
        uint8_t data_len_tmp =
            data_len > MAX_NBYTE_SIZE ? MAX_NBYTE_SIZE : data_len;
        I2C_TransferConfig(hi2c, DevAddress, data_len_tmp, I2C_RELOAD_MODE,
                           I2C_NO_STARTSTOP);
        for (int i = 0; i < data_len_tmp; i++) {
          if (I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) !=
              HAL_OK) {
            return HAL_ERROR;
          }
          *data_ptr++ = (uint8_t)hi2c->Instance->RXDR;
        }
        data_len -= data_len_tmp;
      }

      // sw1 sw2 xor
      I2C_TransferConfig(hi2c, DevAddress, 3, I2C_AUTOEND_MODE,
                         I2C_NO_STARTSTOP);
      for (int i = 0; i < 3; i++) {
        if (I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) !=
            HAL_OK) {
          return HAL_ERROR;
        }
        data[i] = (uint8_t)hi2c->Instance->RXDR;
      }

      break;
    }

    /* No need to Check TC flag, with AUTOEND mode the stop is automatically
     * generated */
    /* Wait until STOPF flag is set */
    if (I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK) {
      return HAL_ERROR;
    }

    /* Clear STOP Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(hi2c);

    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);
    xor = xor_check(xor, pData, *Size);
    xor = xor_check(xor, data, 2);
    if (xor != data[2]) {
      *Size = 0;
      return HAL_ERROR;
    }
    sw1 = data[0];
    sw2 = data[1];

    if ((0x90 != sw1) || (0x00 != sw2)) {
      if (sw1 == 0x6c || sw1 == 0x90) {
        pData[0] = sw2;
        *Size = 1;
      }
      return HAL_ERROR;
    }

    return HAL_OK;
  } else {
    return HAL_BUSY;
  }
}

secbool thd89_transmit(uint8_t *cmd, uint16_t len, uint8_t *resp,
                       uint16_t *resp_len) {
  *resp_len = 0;

  if (i2c_master_send(&i2c_handle, THD89_ADDRESS, cmd, len, 500) != HAL_OK) {
    ensure(secfalse, "se send error");
    return secfalse;
  }
  if (i2c_master_recive(&i2c_handle, THD89_ADDRESS, resp, resp_len,
                        HAL_MAX_DELAY) != HAL_OK) {
    if (sw1 != 0x90) {
      // ensure(secfalse, "se recive error");
    }
    return secfalse;
  }

  return sectrue;
}

uint16_t thd89_last_error() { return sw1 << 8 | sw2; }
