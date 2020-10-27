#ifndef _usart_H_
#define _usart_H_

#include <stdbool.h>
#include <stdint.h>

#define _SUPPORT_DEBUG_UART_ 0

#if (_SUPPORT_DEBUG_UART_)
extern void usart_setup(void);
extern void vUART_DebugInfo(char *pcMsg, uint8_t *pucSendData,
                            uint16_t usStrLen);
#endif

#define BLE_UART USART2

typedef struct _usart_msg {
  uint16_t header;
  uint16_t len;
  uint8_t cmd;
  uint8_t cmd_len;
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
void ble_usart_irq_set(void);
void ble_usart_init(void);
void ble_usart_enable(void);
void ble_usart_disable(void);
void ble_usart_irq_enable(void);
void ble_usart_irq_disable(void);
bool ble_read_byte(uint8_t *buf);
void ble_usart_sendByte(uint8_t data);
void ble_usart_send(uint8_t *buf, uint32_t len);

#endif
