/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>,
 * Copyright (C) 2011 Piotr Esden-Tempski <piotr@esden.net>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "usart.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "ble.h"
#include "common.h"
#include "display.h"
#include "stm32h7xx_hal.h"

UART_HandleTypeDef uart;
UART_HandleTypeDef *huart = &uart;

void ble_usart_init(void) {
  GPIO_InitTypeDef gpio;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_CLOSE);
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA1, SYSCFG_SWITCH_PA1_CLOSE);

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_UART4CLKSOURCE_D2PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    display_printf("HAL_RCCEx_PeriphCLKConfig failed\n");
    return;
  }

  __HAL_RCC_UART4_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // UART4: PA0_C(TX), PA1_C(RX)
  gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF8_UART4;
  HAL_GPIO_Init(GPIOA, &gpio);

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

  if (HAL_UART_Init(huart) != HAL_OK) {
    display_printf("HAL_UART_Init failed\n");
    return;
  }

  HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(UART4_IRQn);

  ble_usart_irq_enable();
}

void ble_usart_enable(void) { __HAL_UART_ENABLE(huart); }

void ble_usart_disable(void) { __HAL_UART_DISABLE(huart); }

void ble_usart_irq_enable(void) { __HAL_UART_ENABLE_IT(huart, UART_IT_RXFNE); }

void ble_usart_irq_disable(void) {
  __HAL_UART_DISABLE_IT(huart, UART_IT_RXFNE);
}

void ble_usart_sendByte(uint8_t data) {
  while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE) == RESET)
    ;
  huart->Instance->TDR = data;
}

void ble_usart_send(uint8_t *buf, uint32_t len) {
  for (uint32_t i = 0; i < len; i++) {
    ble_usart_sendByte(buf[i]);
  }
}

bool ble_read_byte(uint8_t *buf) {
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXFNE) != 0) {
    buf[0] = (uint8_t)(huart->Instance->RDR);
    return true;
  }
  return false;
}

void UART4_IRQHandler(void) {
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXFNE) != 0) {
    ble_uart_poll();
  }
}
