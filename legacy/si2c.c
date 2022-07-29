#include <errno.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>
#include <string.h>

#include "./firmware/chinese.h"
#include "buttons.h"
#include "common.h"
#include "layout.h"
#include "oled.h"
#include "si2c.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"
#include "util.h"

ChannelType host_channel = CHANNEL_NULL;

volatile uint32_t i2c_data_inlen;
volatile bool i2c_recv_done = false;
uint8_t i2c_data_out[SI2C_BUF_MAX_OUT_LEN];
volatile uint32_t i2c_data_outlen, i2c_data_out_pos;
static volatile bool wait_response = true;

trans_fifo i2c_fifo_in = {.p_buf = i2c_data_out,
                          .buf_size = sizeof(i2c_data_out),
                          .over_pre = false,
                          .read_pos = 0,
                          .write_pos = 0,
                          .lock_pos = 0};

trans_fifo i2c_fifo_out = {.p_buf = i2c_data_out,
                           .buf_size = SI2C_BUF_MAX_OUT_LEN,
                           .over_pre = false,
                           .read_pos = 0,
                           .write_pos = 0,
                           .lock_pos = 0};

void i2c_slave_init_irq(void) {
  rcc_periph_clock_enable(RCC_I2C2);
  rcc_periph_clock_enable(RCC_GPIOB);

  i2c_reset(I2C2);

  gpio_set_output_options(GPIO_SI2C_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,
                          GPIO_SI2C_SCL | GPIO_SI2C_SDA);
  gpio_mode_setup(GPIO_SI2C_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  GPIO_SI2C_SCL | GPIO_SI2C_SDA);
  gpio_set_af(GPIO_SI2C_PORT, GPIO_AF4, GPIO_SI2C_SCL | GPIO_SI2C_SDA);
  i2c_peripheral_disable(I2C2);
  /*	//HSI is at 2Mhz */
  i2c_set_fast_mode(I2C2);
  i2c_set_speed(I2C2, i2c_speed_fm_400k, 30);
  /*	//addressing mode*/
  i2c_set_own_7bit_slave_address(I2C2, SI2C_ADDR);
  i2c_enable_ack(I2C2);

  // use interrupt
  i2c_enable_interrupt(I2C2,
                       I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);

  // I2C_CR1(I2C2) |= I2C_CR1_NOSTRETCH;
  i2c_peripheral_enable(I2C2);

  // set NVIC
  nvic_set_priority(NVIC_I2C2_EV_IRQ, 0);
  nvic_enable_irq(NVIC_I2C2_EV_IRQ);

  i2c_enable_ack(I2C2);

  memset(i2c_data_out, 0x00, SI2C_BUF_MAX_OUT_LEN);
}

void i2c_slave_init(void) {
  rcc_periph_clock_enable(RCC_I2C2);
  rcc_periph_clock_enable(RCC_GPIOB);

  i2c_reset(I2C2);

  gpio_set_output_options(GPIO_SI2C_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,
                          GPIO_SI2C_SCL | GPIO_SI2C_SDA);
  gpio_mode_setup(GPIO_SI2C_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  GPIO_SI2C_SCL | GPIO_SI2C_SDA);
  gpio_set_af(GPIO_SI2C_PORT, GPIO_AF4, GPIO_SI2C_SCL | GPIO_SI2C_SDA);
  i2c_peripheral_disable(I2C2);
  /*	//HSI is at 2Mhz */
  i2c_set_fast_mode(I2C2);
  i2c_set_speed(I2C2, i2c_speed_fm_400k, 32);
  /*	//addressing mode*/
  i2c_set_own_7bit_slave_address(I2C2, SI2C_ADDR);
  i2c_enable_ack(I2C2);

  // I2C_CR1(I2C2) |= I2C_CR1_NOSTRETCH;
  i2c_peripheral_enable(I2C2);

  i2c_enable_ack(I2C2);

  memset(i2c_data_out, 0x00, SI2C_BUF_MAX_OUT_LEN);
}

static void i2c_delay(void) {
  volatile uint32_t i = 1000;
  while (i--)
    ;
}

extern uint32_t flash_pos;
void i2c2_ev_isr() {
  uint32_t sr1, sr2;
  static uint8_t dir = 0;  // 0-receive 1-send
  sr1 = I2C_SR1(I2C2);
  if (sr1 & I2C_SR1_ADDR) {  // EV1
    sr2 = I2C_SR2(I2C2);     // clear flag
    dir = sr2 & I2C_SR2_TRA;
  }
  if (sr1 & I2C_SR1_RxNE) {  // EV2
    if (!fifo_put_no_overflow(&i2c_fifo_in, i2c_get_data(I2C2))) {
      layoutError("buffer overflow", "i2c receive");
    }
  }
  if (dir & I2C_SR2_TRA) {
    if (sr1 & I2C_SR1_TxE) {  // EV3 ev3-1
      if (i2c_data_outlen > 0) {
        i2c_send_data(I2C2, i2c_data_out[i2c_data_out_pos++]);
        do {
          i2c_delay();
          sr1 = I2C_SR1(I2C2);
        } while ((!(sr1 & I2C_SR1_BTF)) && (!((sr1 & I2C_SR1_AF))));
        i2c_data_outlen--;
        if (i2c_data_outlen == 0) {
          SET_COMBUS_LOW();
        }
      } else {
        i2c_send_data(I2C2, '#');
      }
    } else if (sr1 & I2C_SR1_BTF) {
      i2c_send_data(I2C2, i2c_data_out[i2c_data_out_pos++]);
      i2c_data_outlen--;
      if (i2c_data_outlen == 0) {
        SET_COMBUS_LOW();
      }
    }
  }
  if (sr1 & I2C_SR1_STOPF) {  // EV4
    I2C_CR1(I2C2) |= I2C_CR1_PE;
    fifo_lockpos_set(&i2c_fifo_in);
    i2c_recv_done = true;
    i2c_data_outlen = 0;  // discard former response
    SET_COMBUS_LOW();
  }
  if (sr1 & I2C_SR1_AF) {  // EV4
    I2C_SR1(I2C2) &= ~I2C_SR1_AF;
  }
}
void i2c2_er_isr(void) {}

void i2c_set_wait(bool flag) { wait_response = flag; }

bool i2c_slave_send(uint32_t data_len) {
  uint32_t len = 0;
  uint32_t i;
  int32_t counter, counter_bak, counter_set;
  bool flag = true;
  bool status = false;

  if (data_len > 64) len = data_len - 64;
  if (len) {
    for (i = 0; i < (len / 64); i++) {
      memmove(i2c_data_out + 64 + i * 63, i2c_data_out + (i + 1) * 64 + 1, 63);
    }
  }
  i2c_data_outlen = (i2c_data_out[5] << 24) + (i2c_data_out[6] << 16) +
                    (i2c_data_out[7] << 8) + i2c_data_out[8] + 9;
  i2c_data_out_pos = 0;
  SET_COMBUS_HIGH();
  if (wait_response) {
    timer_out_set(timer_out_resp, default_resp_time);
    counter = counter_bak = counter_set = default_resp_time / timer1s;
  } else {
    wait_response = true;
    timer_out_set(timer_out_resp, timer1s);
    counter = counter_bak = counter_set = timer1s / timer1s;
  }

  while (1) {
    if (checkButtonOrTimeout(BTN_PIN_NO, timer_out_resp) == true) {
      break;
    }
    if (i2c_data_outlen == 0) {
      status = true;
      break;
    } else {
      counter = timer_out_get(timer_out_resp) / timer1s;
      // show timer count down after 2 seconds
      if (counter <= counter_set - 2) {
        if (counter_bak != counter) {
          if (flag) {
            flag = false;
            oledBufferBak();
            oledClear();
            if (ui_language) {
              oledDrawStringCenterAdapter(OLED_WIDTH / 2, 32, "等待连接...",
                                          FONT_STANDARD);
            } else {
              oledDrawStringCenter(OLED_WIDTH / 2, 32, "Waiting connect...",
                                   FONT_STANDARD);
            }
          }
          char asc_buf[16] = {0};
          uint2str(counter, asc_buf);
          oledBox(0, 20, OLED_WIDTH, 20 + 8, false);
          counter_bak = counter;
          oledDrawStringCenter(OLED_WIDTH / 2, 20, asc_buf, FONT_STANDARD);
          oledRefresh();
        }
      }
    }
  }
  i2c_data_outlen = 0;
  timer_out_set(timer_out_resp, 0);
  SET_COMBUS_LOW();
  if (!flag) {
    oledBufferResume();
    oledRefresh();
  }
  return status;
}

// used in bootloader,no chinese prompt
void i2c_slave_send_ex(uint32_t data_len) {
  uint32_t len = 0;
  uint32_t i;
  int32_t counter, counter_bak, counter_set;
  bool flag = true;

  if (data_len > 64) len = data_len - 64;
  if (len) {
    for (i = 0; i < (len / 64); i++) {
      memmove(i2c_data_out + 64 + i * 63, i2c_data_out + (i + 1) * 64 + 1, 63);
    }
  }
  i2c_data_outlen = (i2c_data_out[5] << 24) + (i2c_data_out[6] << 16) +
                    (i2c_data_out[7] << 8) + i2c_data_out[8] + 9;
  i2c_data_out_pos = 0;
  SET_COMBUS_HIGH();
  if (wait_response) {
    timer_out_set(timer_out_resp, default_resp_time);
    counter = counter_bak = counter_set = default_resp_time / timer1s;
  } else {
    wait_response = true;
    timer_out_set(timer_out_resp, timer1s);
    counter = counter_bak = counter_set = timer1s / timer1s;
  }

  while (1) {
    if (checkButtonOrTimeout(BTN_PIN_NO, timer_out_resp) == true ||
        i2c_data_outlen == 0) {
      break;
    } else {
      counter = timer_out_get(timer_out_resp) / timer1s;
      // show timer count down after 2 seconds
      if (counter <= counter_set - 2) {
        if (counter_bak != counter) {
          if (flag) {
            flag = false;
            oledBufferBak();
            oledClear();
            oledDrawStringCenter(OLED_WIDTH / 2, 32, "Waiting connect...",
                                 FONT_STANDARD);
          }
          char asc_buf[16] = {0};
          uint2str(counter, asc_buf);
          oledBox(0, 20, OLED_WIDTH, 20 + 8, false);
          counter_bak = counter;
          oledDrawStringCenter(OLED_WIDTH / 2, 20, asc_buf, FONT_STANDARD);
          oledRefresh();
        }
      }
    }
  }
  i2c_data_outlen = 0;
  timer_out_set(timer_out_resp, 0);
  SET_COMBUS_LOW();
  if (!flag) {
    oledBufferResume();
    oledRefresh();
  }
}
