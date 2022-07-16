#include STM32_HAL_H

#include <stdio.h>
#include <string.h>

#include "ble.h"
#include "common.h"
#include "irq.h"
#include "usart.h"

#define USART_TIMEOUT 0x100000

UART_HandleTypeDef uart;
UART_HandleTypeDef *huart = &uart;

uint8_t usart_fifo[64] = {0};
uint8_t usart_fifo_len = 0;

uint8_t uart_data_in[UART_BUF_MAX_LEN];

trans_fifo uart_fifo_in = {.p_buf = uart_data_in,
                           .buf_size = UART_BUF_MAX_LEN,
                           .over_pre = false,
                           .read_pos = 0,
                           .write_pos = 0,
                           .lock_pos = 0};

void ble_usart_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_CLOSE);
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA1, SYSCFG_SWITCH_PA1_CLOSE);

  __HAL_RCC_UART4_FORCE_RESET();
  __HAL_RCC_UART4_RELEASE_RESET();

  __HAL_RCC_UART4_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // UART4: PA0_C(TX), PA1_C(RX)
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  huart->Instance = UART4;
  huart->Init.BaudRate = 115200;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  if (HAL_UART_Init(huart) != HAL_OK) {
    ensure(secfalse, "uart init failed");
  }

  NVIC_SetPriority(UART4_IRQn, IRQ_PRI_UART);
  HAL_NVIC_EnableIRQ(UART4_IRQn);

  __HAL_UART_ENABLE_IT(huart, UART_IT_RXFNE);
}

void ble_usart_send_byte(uint8_t data) {
  HAL_UART_Transmit(huart, &data, 1, 0xFFFF);
}

void ble_usart_send(uint8_t *buf, uint32_t len) {
  HAL_UART_Transmit(huart, buf, len, 0xFFFF);
}

bool ble_read_byte(uint8_t *buf) {
  if (HAL_UART_Receive(huart, buf, 1, 100) == HAL_OK) {
    return true;
  }
  return false;
}

secbool ble_usart_can_read(void) {
  if (fifo_lockdata_len(&uart_fifo_in)) {
    return sectrue;
  } else {
    return secfalse;
  }
}

void ble_usart_irq_disable(void) { HAL_NVIC_DisableIRQ(UART4_IRQn); }

uint32_t ble_usart_read(uint8_t *buf, uint32_t lenth) {
  uint32_t len = 0;
  fifo_read_peek(&uart_fifo_in, buf, 4);

  len = (buf[2] << 8) + buf[3];

  fifo_read_lock(&uart_fifo_in, buf, len + 3);
  return len + 3;
}

static uint8_t calXor(uint8_t *buf, uint32_t len) {
  uint8_t tmp = 0;
  uint32_t i;
  for (i = 0; i < len; i++) {
    tmp ^= buf[i];
  }
  return tmp;
}

static HAL_StatusTypeDef usart_rev_bytes(uint8_t *buf, uint32_t len,
                                         uint32_t timeout) {
  for (int i = 0; i < len; i++) {
    while (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXFNE) == 0) {
      timeout--;
      if (timeout == 0) {
        return HAL_TIMEOUT;
      }
    }
    buf[i] = (uint8_t)(huart->Instance->RDR);
    timeout = USART_TIMEOUT;
  }
  return HAL_OK;
}

static void usart_rev_package(uint8_t *buf) {
  uint8_t len = 0;
  uint8_t *p_buf = buf;
  if (usart_rev_bytes(p_buf, 2, USART_TIMEOUT) != HAL_OK) {
    return;
  }
  if (p_buf[0] != 0xA5 || p_buf[1] != 0x5A) {
    return;
  }
  p_buf += 2;
  if (usart_rev_bytes(p_buf, 2, USART_TIMEOUT) != HAL_OK) {
    return;
  }
  len = (p_buf[0] << 8) + p_buf[1];
  if (len > 32) {
    return;
  }
  p_buf += 2;
  if (usart_rev_bytes(p_buf, len - 1, USART_TIMEOUT) != HAL_OK) {
    return;
  }
  p_buf += len - 1;
  if (usart_rev_bytes(p_buf, 1, USART_TIMEOUT) != HAL_OK) {
    return;
  }
  uint8_t xor = calXor(buf, len + 3);
  if (xor != *p_buf) {
    return;
  }
  fifo_write_no_overflow(&uart_fifo_in, buf, len + 3);
}

void UART4_IRQHandler(void) {
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) != 0) {
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
  }
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXFNE) != 0) {
    memset(usart_fifo, 0x00, sizeof(usart_fifo));
    usart_rev_package(usart_fifo);
  }
}

void usart_print(const char *text, int text_len) {
  HAL_UART_Transmit(huart, (uint8_t *)text, text_len, 0xFFFF);
}
