#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "stm32h7xx_hal.h"
#include "sys.h"

TIM_HandleTypeDef TIM8_Handle;

void buzzer_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_TIM8_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  TIM8_Handle.Instance = TIM8;
  TIM8_Handle.Init.Period = PERIOD_VALUE;
  TIM8_Handle.Init.Prescaler = (uint32_t)(SystemCoreClock / (2 * 8000000)) - 1;
  TIM8_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TIM8_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM8_Handle.Init.RepetitionCounter = 0;
  HAL_TIM_PWM_Init(&TIM8_Handle);

  TIM_OC_InitTypeDef TIM_OC_InitStructure;
  TIM_OC_InitStructure.Pulse = PULSE1_VALUE;
  TIM_OC_InitStructure.OCMode = TIM_OCMODE_PWM1;
  TIM_OC_InitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStructure.OCFastMode = TIM_OCFAST_DISABLE;
  TIM_OC_InitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  TIM_OC_InitStructure.OCIdleState = TIM_OCIDLESTATE_SET;
  TIM_OC_InitStructure.OCNIdleState = TIM_OCNIDLESTATE_SET;
  HAL_TIM_PWM_ConfigChannel(&TIM8_Handle, &TIM_OC_InitStructure, TIM_CHANNEL_1);
}

void buzzer_ctrl(bool start) {
  if (start) {
    HAL_TIM_PWM_Start(&TIM8_Handle, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&TIM8_Handle, TIM_CHANNEL_1);
  } else {
    HAL_TIM_PWM_Stop(&TIM8_Handle, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&TIM8_Handle, TIM_CHANNEL_1);
  }
}

void motor_init(void) {
  GPIO_InitTypeDef gpio;

  __HAL_RCC_GPIOK_CLK_ENABLE();

  // PK2, PK3
  gpio.Pin = (GPIO_PIN_2 | GPIO_PIN_3);
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpio.Alternate = 0;
  HAL_GPIO_Init(GPIOK, &gpio);
}

void motor_ctrl(enum MOTOR_STA sta) {
  switch (sta) {
    case MOTOR_REVERSE:
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_2, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_SET);
      break;
    case MOTOR_FORWARD:
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_2, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_RESET);
      break;
    case MOTOR_BRAKE:
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_2, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_SET);
      break;
    case MOTOR_COAST:
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_2, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_RESET);
      break;
    default:
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_2, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_RESET);
  }
}
