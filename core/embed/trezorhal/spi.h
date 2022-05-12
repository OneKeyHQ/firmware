#ifndef _SPI_H_
#define _SPI_H_

#include <secbool.h>
#include <stdint.h>
#include "trans_fifo.h"

#define SPI_PKG_SIZE 64
#define SPI_BUF_MAX_IN_LEN (16 * 1024)
#define SPI_BUF_MAX_OUT_LEN (3 * 1024)

#define SET_COMBUS_HIGH() HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET)
#define SET_COMBUS_LOW() HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET)

typedef enum _ChannelType {
  CHANNEL_NULL,
  CHANNEL_USB,
  CHANNEL_SLAVE,
} ChannelType;

extern ChannelType host_channel;
extern uint8_t spi_data_out[SPI_BUF_MAX_OUT_LEN];

#if !EMULATOR
int32_t wait_spi_rx_event(int32_t timeout);
int32_t wait_spi_tx_event(int32_t timeout);
int32_t spi_slave_send(uint8_t *buf, uint32_t size, int32_t timeout);
int32_t spi_slave_init();
uint32_t spi_slave_poll(uint8_t *buf);
secbool spi_can_write(void);
uint32_t spi_read_retry(uint8_t *buf);
#endif

#endif
