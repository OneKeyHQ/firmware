#ifndef _USART_H_
#define _USART_H_

#include <secbool.h>
#include <stdbool.h>
#include <stdint.h>
#include "trans_fifo.h"

#define BLE_UART USART4
#define UART_BUF_MAX_LEN (512)

typedef struct _usart_msg {
  uint16_t header;
  uint16_t len;
  uint8_t cmd;
  uint8_t *cmd_vale;
  uint8_t xor ;
} usart_msg;

enum {
  VALUE_PREPARE_DFU = 0x01,
  VALUE_ENTER_DFU = 0x02,
  VALUE_ENTER_FAILED = 0x03,
  VALUE_REP_FAILED = 0x04,
  VALUE_UNKNOWN_ERR = 0x05
};

enum {
  UARTSTATE_IDLE,
  UARTSTATE_READ_LEN,
  UARTSTATE_READ_DATA,
  UARTSTATE_READ_FINISHED,
};

void ble_usart_init(void);
void ble_usart_irq_disable(void);
bool ble_read_byte(uint8_t *buf);
void ble_usart_send_byte(uint8_t data);
void ble_usart_send(uint8_t *buf, uint32_t len);
secbool ble_usart_can_read(void);
uint32_t ble_usart_read(uint8_t *buf, uint32_t lenth);
void usart_print(const char *text, int text_len);

#endif
