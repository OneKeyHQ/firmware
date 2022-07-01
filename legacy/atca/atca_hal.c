#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <string.h>

#include "atca_hal.h"
#include "supervise.h"

#ifdef ATCA_INTERFACE_I2C

#include <libopencm3/stm32/i2c.h>
#include "timer.h"

#define ATCA_I2C I2C1

#define ATCA_SLAVE_ADDR (0xC0 >> 1)

#define ATCA_I2C_PORT GPIOB
#define ATCA_I2C_SCL GPIO6
#define ATCA_I2C_SDA GPIO7

#define RETRIES 500000

uint8_t address = 0;

void atca_i2c_init(void) {
  rcc_periph_clock_enable(RCC_I2C1);
  rcc_periph_clock_enable(RCC_GPIOB);

  i2c_reset(ATCA_I2C);

  gpio_set_output_options(ATCA_I2C_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,
                          ATCA_I2C_SCL | ATCA_I2C_SDA);
  gpio_set_af(ATCA_I2C_PORT, GPIO_AF4, ATCA_I2C_SCL | ATCA_I2C_SDA);
  gpio_mode_setup(ATCA_I2C_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  ATCA_I2C_SCL | ATCA_I2C_SDA);
  i2c_peripheral_disable(ATCA_I2C);

  // 100k
  i2c_set_speed(ATCA_I2C, i2c_speed_sm_100k, 30);
  i2c_peripheral_enable(ATCA_I2C);
}

ATCA_STATUS atca_i2c_send_byte(uint8_t tx_data) {
  int retries = RETRIES;

  while ((I2C_SR2(ATCA_I2C) & I2C_SR2_BUSY)) {
  }

  i2c_send_start(ATCA_I2C);

  /* Wait for the end of the start condition, master mode selected, and BUSY bit
   * set */
  while (!((I2C_SR1(ATCA_I2C) & I2C_SR1_SB) &&
           (I2C_SR2(ATCA_I2C) & I2C_SR2_MSL) &&
           (I2C_SR2(ATCA_I2C) & I2C_SR2_BUSY)))
    ;

  i2c_send_7bit_address(ATCA_I2C, ATCA_SLAVE_ADDR, I2C_WRITE);

  /* Waiting for address is transferred. */
  while ((!(I2C_SR1(ATCA_I2C) & I2C_SR1_ADDR)) && (retries--))
    ;
  if (retries <= 0) {
    return ATCA_COMM_FAIL;
  }

  /* Clearing ADDR condition sequence. */
  (void)I2C_SR2(ATCA_I2C);

  i2c_send_data(ATCA_I2C, tx_data);
  while (!(I2C_SR1(ATCA_I2C) & (I2C_SR1_BTF)))
    ;
  i2c_send_stop(ATCA_I2C);

  return ATCA_SUCCESS;
}

ATCA_STATUS atca_i2c_send(uint8_t *tx_data, uint8_t tx_len) {
  int retries = RETRIES;

  while ((I2C_SR2(ATCA_I2C) & I2C_SR2_BUSY)) {
  }

  i2c_send_start(ATCA_I2C);

  /* Wait for the end of the start condition, master mode selected, and BUSY bit
   * set */
  while (!((I2C_SR1(ATCA_I2C) & I2C_SR1_SB) &&
           (I2C_SR2(ATCA_I2C) & I2C_SR2_MSL) &&
           (I2C_SR2(ATCA_I2C) & I2C_SR2_BUSY)))
    ;

  i2c_send_7bit_address(ATCA_I2C, ATCA_SLAVE_ADDR, I2C_WRITE);

  /* Waiting for address is transferred. */
  while ((!(I2C_SR1(ATCA_I2C) & I2C_SR1_ADDR)) && (retries--))
    ;
  if (retries <= 0) {
    return ATCA_COMM_FAIL;
  }

  /* Clearing ADDR condition sequence. */
  (void)I2C_SR2(ATCA_I2C);

  for (uint8_t i = 0; i < tx_len; i++) {
    i2c_send_data(ATCA_I2C, tx_data[i]);
    while (!(I2C_SR1(ATCA_I2C) & (I2C_SR1_BTF)))
      ;
  }
  i2c_send_stop(ATCA_I2C);

  return ATCA_SUCCESS;
}

ATCA_STATUS atca_i2c_receive(uint8_t *rx_data, uint8_t *rx_len) {
  volatile int retries = RETRIES;
  uint8_t rx_max_len = *rx_len;

  i2c_send_start(ATCA_I2C);
  i2c_enable_ack(ATCA_I2C);

  /* Wait for the end of the start condition, master mode selected, and BUSY bit
   * set */
  while (!((I2C_SR1(ATCA_I2C) & I2C_SR1_SB) &&
           (I2C_SR2(ATCA_I2C) & I2C_SR2_MSL) &&
           (I2C_SR2(ATCA_I2C) & I2C_SR2_BUSY)))
    ;

  i2c_send_7bit_address(ATCA_I2C, ATCA_SLAVE_ADDR, I2C_READ);

  /* Waiting for address is transferred. */
  while ((!(I2C_SR1(ATCA_I2C) & I2C_SR1_ADDR)) && (retries--))
    ;
  if (retries <= 0) {
    i2c_send_stop(ATCA_I2C);
    return 0x01;
  }
  /* Clearing ADDR condition sequence. */
  (void)I2C_SR2(ATCA_I2C);

  // receive first count bytes
  while ((!(I2C_SR1(ATCA_I2C) & I2C_SR1_RxNE)) && (retries--))
    ;
  if (retries <= 0) {
    return 0x02;
  }
  rx_data[0] = i2c_get_data(ATCA_I2C);

  if (rx_data[0] < ATCA_RSP_SIZE_MIN) {
    i2c_send_stop(ATCA_I2C);
    return ATCA_INVALID_SIZE;
  }
  if (rx_data[0] > rx_max_len) {
    i2c_send_stop(ATCA_I2C);
    return ATCA_SMALL_BUFFER;
  }

  for (uint8_t i = 1; i < rx_data[0] - 1; ++i) {
    retries = RETRIES;
    if (i == rx_data[0] - 1) {
      i2c_disable_ack(ATCA_I2C);
    }
    while ((!(I2C_SR1(ATCA_I2C) & I2C_SR1_RxNE)) && (retries--))
      ;
    if (retries <= 0) {
      return 0x03;
    }
    rx_data[i] = i2c_get_data(ATCA_I2C);
  }
  i2c_send_stop(ATCA_I2C);

  *rx_len = rx_data[0];

  return ATCA_SUCCESS;
}

ATCA_STATUS atca_i2c_wake(void) {
  uint8_t rx_buf[4] = {0};
  uint8_t rx_len = 4;
  volatile int retries = RETRIES;
  volatile uint32_t sr1 = 0;
  ATCA_STATUS status;

  const uint8_t expected_response[4] = {0x04, 0x11, 0x33, 0x43};

  while ((I2C_SR2(ATCA_I2C) & I2C_SR2_BUSY)) {
  }

  i2c_send_start(ATCA_I2C);

  /* Wait for the end of the start condition, master mode selected, and BUSY
  bit
   * set */
  while (!((I2C_SR1(ATCA_I2C) & I2C_SR1_SB) &&
           (I2C_SR2(ATCA_I2C) & I2C_SR2_MSL) &&
           (I2C_SR2(ATCA_I2C) & I2C_SR2_BUSY)))
    ;

  i2c_send_data(ATCA_I2C, 0x00);

  delay_ms(2);

  I2C_SR1(ATCA_I2C) = 0X00;

  status = atca_i2c_receive(rx_buf, &rx_len);
  if (status) return status;
  if (memcmp(rx_buf, expected_response, 4)) return ATCA_WAKE_FAILED;

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

#include <libopencm3/stm32/timer.h>

#define ATCA_TIMER TIM2
static uint32_t timer_frequency = 0;

#define ATCA_SWI_PORT GPIOB
#define ATCA_SWI_PIN GPIO7

#define ATCA_SWI_START_PLUS_TIMEOUT 1000  //>2ms
#define ATCA_SWI_ZERO_PLUS_TIMEOUT 50

typedef enum { PIN_DIRECTION_OUT = 0, PIN_DIRECTION_IN } SWI_PIN_DIRECTION;

static void atca_delay_us(uint32_t delay_us) {
  timer_disable_counter(ATCA_TIMER);
  timer_set_counter(ATCA_TIMER, 29);
  timer_enable_counter(ATCA_TIMER);
  while (delay_us--) {
    while (!timer_get_flag(ATCA_TIMER, TIM_SR_UIF))
      ;
    timer_clear_flag(ATCA_TIMER, TIM_SR_UIF);
    timer_set_counter(ATCA_TIMER, 29);
  }
  timer_disable_counter(ATCA_TIMER);
}

#define DELAY_1_BIT atca_delay_us(4)
#define DELAY_5_BITS atca_delay_us(26)
#define DELAY_7_BITS atca_delay_us(35)

static void atca_pin_direction(SWI_PIN_DIRECTION direction) {
  if (PIN_DIRECTION_OUT == direction) {
    gpio_mode_setup(ATCA_SWI_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP,
                    ATCA_SWI_PIN);

  } else {
    gpio_mode_setup(ATCA_SWI_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                    ATCA_SWI_PIN);
  }
}

static void atca_pin_out_level(bool high) {
  if (high) {
    gpio_set(ATCA_SWI_PORT, ATCA_SWI_PIN);
  } else {
    gpio_clear(ATCA_SWI_PORT, ATCA_SWI_PIN);
  }
}

static bool atca_swi_get_pin(void) {
  return gpio_get(ATCA_SWI_PORT, ATCA_SWI_PIN);
}

void atca_swi_init(void) {
  gpio_set_output_options(ATCA_SWI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                          ATCA_SWI_PIN);
  atca_pin_direction(PIN_DIRECTION_OUT);

  // enable TIM2 clock APB1 30Mhz
  rcc_periph_clock_enable(RCC_TIM2);
  timer_set_mode(ATCA_TIMER, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
                 TIM_CR1_DIR_DOWN);
  timer_disable_counter(ATCA_TIMER);
  timer_disable_preload(ATCA_TIMER);
  timer_frequency = 30 * 1000 * 1000;
}

ATCA_STATUS atca_swi_send_bytes(uint8_t *buffer, uint8_t len) {
  atca_pin_direction(PIN_DIRECTION_OUT);

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
  return ATCA_SUCCESS;
}

void atca_swi_send_byte(uint8_t byte) { atca_swi_send_bytes(&byte, 1); }

ATCA_STATUS atac_swi_receive_bytes(uint8_t *buffer, uint8_t len) {
  ATCA_STATUS status = ATCA_SUCCESS;
  uint32_t timeout = 0;
  uint8_t plus_count = 0;

  atca_pin_direction(PIN_DIRECTION_IN);
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
  return atca_swi_idle();
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
