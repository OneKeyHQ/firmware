#ifndef _SPI_H_
#define _SPI_H_

#include <secbool.h>
#include <stdint.h>
#include "trans_fifo.h"

#define SPI_PKG_SIZE 64
#define SPI_BUF_MAX_IN_LEN (16 * 1024)
#define SPI_BUF_MAX_OUT_LEN (3 * 1024)

#define SET_COMBUS_HIGH() HAL_GPIO_WritePin(GPIOK, GPIO_PIN_6, GPIO_PIN_SET)
#define SET_COMBUS_LOW() HAL_GPIO_WritePin(GPIOK, GPIO_PIN_6, GPIO_PIN_RESET)

#define SET_RX_BUS_IDEL() HAL_GPIO_WritePin(GPIOK, GPIO_PIN_6, GPIO_PIN_SET)
#define SET_RX_BUS_BUSY() HAL_GPIO_WritePin(GPIOK, GPIO_PIN_6, GPIO_PIN_RESET)

#define BLE_RST_PIN_HIGH() HAL_GPIO_WritePin(GPIOK, GPIO_PIN_5, GPIO_PIN_SET)
#define BLE_RST_PIN_LOW() HAL_GPIO_WritePin(GPIOK, GPIO_PIN_5, GPIO_PIN_RESET)

/* Definition for SPIx's DMA */
#define SPIx_TX_DMA_STREAM DMA1_Stream3
#define SPIx_RX_DMA_STREAM DMA1_Stream2

#define SPIx_TX_DMA_REQUEST DMA_REQUEST_SPI2_TX
#define SPIx_RX_DMA_REQUEST DMA_REQUEST_SPI2_RX

/* Definition for SPIx's NVIC */
#define SPIx_DMA_TX_IRQn DMA1_Stream3_IRQn
#define SPIx_DMA_RX_IRQn DMA1_Stream2_IRQn

#define SPIx_DMA_TX_IRQHandler DMA1_Stream3_IRQHandler
#define SPIx_DMA_RX_IRQHandler DMA1_Stream2_IRQHandler

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
uint32_t spi_read_blocking(uint8_t *buf, int timeout);
#endif

#endif
