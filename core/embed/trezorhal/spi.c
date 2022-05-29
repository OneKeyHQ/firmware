#include STM32_HAL_H

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "irq.h"
#include "spi.h"
#include "timer.h"

SPI_HandleTypeDef spi;
static uint8_t recv_buf[SPI_PKG_SIZE];
static int32_t volatile spi_rx_event = 0;
static int32_t volatile spi_tx_event = 0;
ChannelType host_channel = CHANNEL_NULL;

uint8_t spi_data_in[SPI_BUF_MAX_IN_LEN];
uint8_t spi_data_out[SPI_BUF_MAX_OUT_LEN];

trans_fifo spi_fifo_in = {.p_buf = spi_data_in,
                          .buf_size = SPI_BUF_MAX_IN_LEN,
                          .over_pre = false,
                          .read_pos = 0,
                          .write_pos = 0,
                          .lock_pos = 0};

secbool spi_can_write(void) {
  if (spi_tx_event == 0)
    return sectrue;
  else
    return secfalse;
}

int32_t wait_spi_rx_event(int32_t timeout) {
  int32_t tickstart = HAL_GetTick();

  while (spi_rx_event == 1) {
    if ((HAL_GetTick() - tickstart) > timeout) {
      return -1;
    }
  }
  return 0;
}

int32_t wait_spi_tx_event(int32_t timeout) {
  int32_t tickstart = HAL_GetTick();

  while (spi_tx_event == 1) {
    if ((HAL_GetTick() - tickstart) > timeout) {
      return -1;
    }
  }
  return 0;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
  if (!fifo_write_no_overflow(&spi_fifo_in, recv_buf, hspi->RxXferSize)) {
  }

  if (spi_rx_event) {
    spi_rx_event = 0;
  }

  memset(recv_buf, 0, SPI_PKG_SIZE);
  HAL_SPI_Receive_IT(&spi, recv_buf, SPI_PKG_SIZE);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  if (spi_tx_event) {
    spi_tx_event = 0;
  }

  memset(recv_buf, 0, SPI_PKG_SIZE);
  HAL_SPI_Receive_IT(&spi, recv_buf, SPI_PKG_SIZE);
}

void SPI2_IRQHandler(void) { HAL_SPI_IRQHandler(&spi); }

int32_t spi_slave_init() {
  GPIO_InitTypeDef gpio;

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_SPI2_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  gpio.Pin = GPIO_PIN_13;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOJ, &gpio);
  HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);

  gpio.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOK, &gpio);
  HAL_GPIO_WritePin(GPIOK, GPIO_PIN_6, GPIO_PIN_SET);

  gpio.Pin = GPIO_PIN_5;
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOK, &gpio);

  // SPI2: PB12(NSS),PB13(SCK)
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF5_SPI2;
  gpio.Pin = GPIO_PIN_12 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOB, &gpio);

  // SPI2: PC2(MISO), PC3(MOSI)
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF5_SPI2;
  gpio.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  HAL_GPIO_Init(GPIOC, &gpio);

  spi.Instance = SPI2;
  spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  spi.Init.Direction = SPI_DIRECTION_2LINES;
  spi.Init.CLKPhase = SPI_PHASE_1EDGE;
  spi.Init.CLKPolarity = SPI_POLARITY_LOW;
  spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  spi.Init.CRCPolynomial = 7;
  spi.Init.DataSize = SPI_DATASIZE_8BIT;
  spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  spi.Init.NSS = SPI_NSS_HARD_INPUT;
  spi.Init.TIMode = SPI_TIMODE_DISABLE;
  spi.Init.Mode = SPI_MODE_SLAVE;

  if (HAL_OK != HAL_SPI_Init(&spi)) {
    return -1;
  }

  NVIC_SetPriority(SPI2_IRQn, IRQ_PRI_SPI);
  HAL_NVIC_EnableIRQ(SPI2_IRQn);

  memset(recv_buf, 0, SPI_PKG_SIZE);
  spi_rx_event = 1;

  /* start SPI receive */
  if (HAL_SPI_Receive_IT(&spi, recv_buf, SPI_PKG_SIZE) != HAL_OK) {
    return -1;
  }

  return 0;
}

int32_t spi_slave_send(uint8_t *buf, uint32_t size, int32_t timeout) {
  uint32_t msg_size;

  msg_size = size < SPI_PKG_SIZE ? SPI_PKG_SIZE : size;

  SET_COMBUS_LOW();
  SET_COMBUS_LOW1();

  if (HAL_SPI_Abort_IT(&spi) != HAL_OK) {
    SET_COMBUS_HIGH();
    SET_COMBUS_HIGH1();
    return -1;
  }

  spi_tx_event = 1;
  if (HAL_SPI_Transmit_IT(&spi, buf, msg_size) != HAL_OK) {
    SET_COMBUS_HIGH();
    SET_COMBUS_HIGH1();
    return -1;
  }

  if (wait_spi_tx_event(timeout) != 0) {
    SET_COMBUS_HIGH();
    SET_COMBUS_HIGH1();
    return -1;
  }

  SET_COMBUS_HIGH();
  SET_COMBUS_HIGH1();

  return msg_size;
}

uint32_t spi_slave_poll(uint8_t *buf) {
  volatile uint32_t total_len, len, ret;

  if (buf == NULL) return 0;

  total_len = fifo_lockdata_len(&spi_fifo_in);
  if (total_len == 0) {
    return 0;
  }

  len = total_len > SPI_PKG_SIZE ? SPI_PKG_SIZE : total_len;
  ret = fifo_read_lock(&spi_fifo_in, buf, len);
  return ret;
}

uint32_t spi_read_retry(uint8_t *buf) {
  spi_rx_event = 1;

  for (int retry = 0;; retry++) {
    int r = wait_spi_rx_event(500);
    if (r == -1) {  // reading failed
      if (r == -1 && retry < 2) {
        // only timeout => let's try again
      } else {
        // error
        error_shutdown("Error reading", "from SPI.", "Try to", "reset.");
      }
    }

    if (r == 0) {
      return spi_slave_poll(buf);
    }
  }
}
