#include STM32_HAL_H

#if defined(STM32H747xx)
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_gpio.h"
#endif

#include <string.h>

#include "atca_command.h"
#include "atca_hal.h"
#include "common.h"
#include "display.h"
#include "irq.h"
#include "secbool.h"

// #include "supervise.h"

// #define ATCA_INTERFACE_I2C

#ifdef ATCA_INTERFACE_I2C

#define ATCA_I2C I2C4
#define ATCA_SLAVE_ADDR (0xC0 >> 1)

static I2C_HandleTypeDef hi2c4;

uint8_t address = 0;

static void _i2c_msp_init(void) {
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStructure;
  /**I2C4 GPIO Configuration
  PB6     ------> I2C4_SCL
  PD13     ------> I2C4_SDA
  */
  GPIO_InitStructure.Pin = GPIO_PIN_6;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Alternate = GPIO_AF6_I2C4;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_13;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Alternate = GPIO_AF4_I2C4;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Peripheral clock enable */
  __HAL_RCC_I2C4_CLK_ENABLE();
}

static void _i2c_init(void) {
  hi2c4.Instance = I2C4;
  hi2c4.Init.Timing = 0x00401242;
  hi2c4.Init.OwnAddress1 = 0;
  hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c4.Init.OwnAddress2 = 0;
  hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c4) != HAL_OK) {
    ensure(secfalse, NULL);
  }
  /** Configure Analogue filter
   */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
    ensure(secfalse, NULL);
  }
  /** Configure Digital filter
   */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK) {
    ensure(secfalse, NULL);
  }
  /** I2C Enable Fast Mode Plus
   */
  HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C4);
}

void atca_i2c_init(void) {
  _i2c_msp_init();
  _i2c_init();
}

ATCA_STATUS atca_i2c_send_byte(uint8_t tx_data) {
  if (HAL_OK == HAL_I2C_Master_Transmit(&hi2c4, address, &tx_data, 1, 1000)) {
    return ATCA_SUCCESS;
  }
  return ATCA_COMM_FAIL;
}

ATCA_STATUS atca_i2c_send(uint8_t *tx_data, uint8_t tx_len) {
  ensure(sectrue * (HAL_OK == HAL_I2C_Master_Transmit(&hi2c4, ATCA_SLAVE_ADDR,
                                                      tx_data, tx_len, 1000)),
         NULL);

  return ATCA_SUCCESS;
}

ATCA_STATUS atca_i2c_receive(uint8_t *rx_data, uint8_t *rx_len) {
  uint8_t rx_max_len = *rx_len;

  if (HAL_OK !=
      HAL_I2C_Master_Receive(&hi2c4, ATCA_SLAVE_ADDR, &rx_data[0], 1, 1000)) {
    return ATCA_COMM_FAIL;  // read failure
  }

  if (rx_data[0] < ATCA_RSP_SIZE_MIN) {
    return ATCA_INVALID_SIZE;
  }
  if (rx_data[0] > rx_max_len) {
    return ATCA_SMALL_BUFFER;
  }

  if (HAL_OK != HAL_I2C_Master_Receive(&hi2c4, ATCA_SLAVE_ADDR, &rx_data[1],
                                       rx_data[0] - 1, 1000)) {
    return ATCA_COMM_FAIL;  // read failure
  }

  *rx_len = rx_data[0];

  return ATCA_SUCCESS;
}

ATCA_STATUS atca_i2c_wake(void) {
  uint8_t rx_buf[4] = {0};
  uint8_t rx_len = 4;

  const uint8_t expected_response[4] = {0x04, 0x11, 0x33, 0x43};

  if (atca_i2c_send_byte(0x00) == ATCA_SUCCESS) {
    hal_delay(2);

    atca_i2c_receive(rx_buf, &rx_len);
    if (memcmp(rx_buf, expected_response, 4)) return ATCA_WAKE_FAILED;
  } else {
    return ATCA_COMM_FAIL;
  }

  return ATCA_SUCCESS;
}

ATCA_STATUS atca_i2c_find_device(void) {
  ATCA_STATUS status;

  for (address = 0; address < 0x80; address++) {
    status = atca_i2c_wake();
    if (ATCA_SUCCESS == status) break;
  }

  return status;
}

ATCA_STATUS atca_i2c_idle(void) {
  return atca_i2c_send_byte(ATCA_WORD_ADDRESS_IDLE);
}

ATCA_STATUS atca_i2c_sleep(void) {
  return atca_i2c_send_byte(ATCA_WORD_ADDRESS_SLEEP);
}

#else

#define ATCA_SWI_PORT GPIOD
#define ATCA_SWI_PIN GPIO_PIN_13

#define ATCA_SWI_START_PLUS_TIMEOUT 2000  //>2ms
#define ATCA_SWI_ZERO_PLUS_TIMEOUT 50

typedef enum { PIN_DIRECTION_OUT = 0, PIN_DIRECTION_IN } SWI_PIN_DIRECTION;

// clang-format off
#define DEMCR                     (*(volatile uint32_t*) (0xE000EDFCuL))   // Debug Exception and Monitor Control Register
#define TRACEENA_BIT              (1uL << 24)                                   // Trace enable bit
#define DWT_CTRL                  (*(volatile uint32_t*) (0xE0001000uL))   // DWT Control Register
#define CYCCNTENA_BIT             (1uL << 0)
#define DWT_CYCCNT                (*(volatile uint32_t*) (0xE0001004uL))
// clang-format on

// 400MHZ  min 2.5ns,max 10.73s
static void atca_delay_ns(uint32_t delay_ns) {
  DWT_CYCCNT = 0;
  uint32_t start = DWT_CYCCNT;
  uint32_t count = (delay_ns * (SystemCoreClock / 100000000)) / 10;
  while ((DWT_CYCCNT - start) < count)
    ;
}

static void atca_delay_us(uint32_t delay_us) { atca_delay_ns(delay_us * 1000); }

#define DELAY_1_BIT atca_delay_ns(4200)
#define DELAY_5_BITS atca_delay_ns(26200)
#define DELAY_7_BITS atca_delay_ns(34500)

static void atca_pin_direction(SWI_PIN_DIRECTION direction) {
  if (PIN_DIRECTION_OUT == direction) {
    LL_GPIO_SetPinMode(ATCA_SWI_PORT, ATCA_SWI_PIN, LL_GPIO_MODE_OUTPUT);

  } else {
    LL_GPIO_SetPinMode(ATCA_SWI_PORT, ATCA_SWI_PIN, LL_GPIO_MODE_INPUT);
  }
}

static void atca_pin_out_level(bool high) {
  if (high) {
    HAL_GPIO_WritePin(ATCA_SWI_PORT, ATCA_SWI_PIN, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(ATCA_SWI_PORT, ATCA_SWI_PIN, GPIO_PIN_RESET);
  }
}

static bool atca_swi_get_pin(void) {
  return HAL_GPIO_ReadPin(ATCA_SWI_PORT, ATCA_SWI_PIN);
}

void atca_swi_init(void) {
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.Pin = ATCA_SWI_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ATCA_SWI_PORT, &GPIO_InitStructure);

  if ((DEMCR & TRACEENA_BIT) == 0) {
    DEMCR |= TRACEENA_BIT;
  }

  if ((DWT_CTRL & CYCCNTENA_BIT) == 0) {  // Cycle counter not enabled?
    DWT_CTRL |= CYCCNTENA_BIT;            // Enable Cycle counter
  }
  DWT_CYCCNT = 0;
}

ATCA_STATUS atca_swi_send_bytes(uint8_t *buffer, uint8_t len) {
  atca_pin_direction(PIN_DIRECTION_OUT);
  // uint32_t state = disable_irq();

  for (uint8_t i = 0; i < len; i++) {
    for (uint8_t one_bit = 1; one_bit > 0; one_bit <<= 1) {
      if (one_bit & buffer[i]) {
        // logic 1(0x7f)
        // start
        atca_pin_out_level(false);
        DELAY_1_BIT;
        atca_pin_out_level(true);
        DELAY_7_BITS;
      } else {
        // logic 0(0x7d)
        // start
        atca_pin_out_level(false);
        DELAY_1_BIT;
        atca_pin_out_level(true);
        DELAY_1_BIT;
        atca_pin_out_level(false);
        DELAY_1_BIT;
        atca_pin_out_level(true);
        DELAY_5_BITS;
      }
    }
  }
  // enable_irq(state);
  return ATCA_SUCCESS;
}

void atca_swi_send_byte(uint8_t byte) { atca_swi_send_bytes(&byte, 1); }

ATCA_STATUS atac_swi_receive_bytes(uint8_t *buffer, uint8_t len) {
  ATCA_STATUS status = ATCA_SUCCESS;
  uint32_t timeout = 0;
  uint8_t plus_count = 0;

  atca_pin_direction(PIN_DIRECTION_IN);
  // uint32_t state = disable_irq();
  for (uint8_t i = 0; i < len; i++) {
    buffer[i] = 0;
    for (uint8_t one_bit = 1; one_bit > 0; one_bit <<= 1) {
      plus_count = 0;
      timeout = ATCA_SWI_START_PLUS_TIMEOUT;
      // start bit
      while (timeout--) {
        if (atca_swi_get_pin() == 0) {
          break;
        }
        if (timeout == 0) {
          status = ATCA_RX_TIMEOUT;
          break;
        }
      }

      timeout = ATCA_SWI_START_PLUS_TIMEOUT;
      // start bit end
      while (timeout--) {
        if (atca_swi_get_pin() != 0) {
          plus_count = 1;
          break;
        }
      }

      if (plus_count == 0) {
        status = ATCA_RX_TIMEOUT;
        break;
      }

      timeout = ATCA_SWI_ZERO_PLUS_TIMEOUT;
      while (timeout--) {
        if (atca_swi_get_pin() == 0) {
          plus_count = 2;
          break;
        }
      }

      if (plus_count == 2) {
        timeout = ATCA_SWI_ZERO_PLUS_TIMEOUT;
        while (timeout--) {
          if (atca_swi_get_pin() != 0) {
            break;
          }
        }
      } else {
        buffer[i] |= one_bit;
      }
    }
    if (status != ATCA_SUCCESS) {
      break;
    }
    if (i == 0) {
      if (buffer[0] < 4) {
        status = ATCA_INVALID_SIZE;
        break;
      } else if (buffer[0] > len) {
        status = ATCA_SMALL_BUFFER;
        break;
      } else {
        len = buffer[0];
      }
    }
  }

  atca_pin_direction(PIN_DIRECTION_OUT);
  atca_delay_us(93);  // tTURNGROUND
  // enable_irq(state);
  return status;
}

ATCA_STATUS atca_swi_receive(uint8_t *rx_data, uint8_t *rx_len) {
  ATCA_STATUS status = ATCA_SUCCESS;
  uint8_t rx_max_len = *rx_len;
  *rx_len = 0;
  if (rx_max_len < 1) {
    return ATCA_SMALL_BUFFER;
  }
  atca_swi_send_byte(SWI_FLAG_TX);
  status = atac_swi_receive_bytes(rx_data, rx_max_len);
  if (status != ATCA_SUCCESS) {
    return status;
  }

  *rx_len = rx_data[0];

  return ATCA_SUCCESS;
}

ATCA_STATUS atca_swi_wake(void) {
  uint8_t rx_buf[4] = {0};
  uint8_t rx_len = sizeof(rx_buf);
  ATCA_STATUS status;

  const uint8_t expected_response[4] = {0x04, 0x11, 0x33, 0x43};

  atca_pin_direction(PIN_DIRECTION_OUT);
  atca_pin_out_level(false);
  atca_delay_us(60);
  atca_pin_out_level(true);
  atca_delay_us(2000);

  status = atca_swi_receive(rx_buf, &rx_len);
  if (status) return status;
  if (memcmp(rx_buf, expected_response, 4)) return ATCA_WAKE_FAILED;

  return ATCA_SUCCESS;
}

ATCA_STATUS atca_swi_idle(void) {
  atca_swi_send_byte(SWI_FLAG_IDLE);
  return ATCA_SUCCESS;
}

#endif

void atca_init(void) {
#ifdef ATCA_INTERFACE_I2C
  atca_i2c_init();
#else
  atca_swi_init();
#endif
}

ATCA_STATUS atca_wake(void) {
#ifdef ATCA_INTERFACE_I2C
  return atca_i2c_wake();
#else
  return atca_swi_wake();
#endif
}

ATCA_STATUS atca_idle(void) {
#ifdef ATCA_INTERFACE_I2C
  return atca_i2c_idle();
#else
  return atca_swi_idle();
#endif
}

ATCA_STATUS atca_send(uint8_t *tx_data, uint8_t tx_len) {
#ifdef ATCA_INTERFACE_I2C
  return atca_i2c_send(tx_data, tx_len);
#else
  return atca_swi_send_bytes(tx_data, tx_len);
#endif
}

ATCA_STATUS atca_receive(uint8_t *rx_data, uint8_t *rx_len) {
#ifdef ATCA_INTERFACE_I2C
  return atca_i2c_receive(rx_data, rx_len);
#else
  return atca_swi_receive(rx_data, rx_len);
#endif
}

void atca_test(void) {
  static uint8_t rand_buffer[32] = {0};
  atca_random(rand_buffer);
  if (rand_buffer[0] != 0x00) {
    display_printf("get randome success\n");
  }
}
