/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include STM32_HAL_H

#include <string.h>

#include "common.h"
#include "secbool.h"

#include "display.h"
#include "touch.h"

static I2C_HandleTypeDef i2c_handle;

#if defined(STM32H747xx)

#define TOUCH_ADDRESS \
  (0x1AU << 1)  // the HAL requires the 7-bit address to be shifted by one bit
#define TOUCH_PACKET_SIZE 7U
#define EVENT_PRESS_NONE -1
#define EVENT_PRESS_DOWN 0x00U
#define EVENT_CONTACT 0x80U
#define EVENT_LIFT_UP 0x40U
#define EVENT_NO_EVENT 0xC0U
#define GESTURE_NO_GESTURE 0x00U
#define X_POS_MSB (touch_data[1])
#define X_POS_LSB ((touch_data[3] >> 4) & 0x0FU)
#define Y_POS_MSB (touch_data[2])
#define Y_POS_LSB (touch_data[3] & 0x0FU)

void _i2c_msp_init(void) {
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure the GPIO Reset pin */
  GPIO_InitStructure.Pin = GPIO_PIN_1;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Activate XRES active low */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); /* Deactivate XRES */
  HAL_Delay(10);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
  HAL_Delay(10);                                      /* wait 300 ms */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); /* Deactivate XRES */
  HAL_Delay(300);

  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Speed =
      GPIO_SPEED_FREQ_LOW;  // I2C is a KHz bus and low speed is still good into
                            // the low MHz
  GPIO_InitStructure.Alternate = GPIO_AF4_I2C2;
  GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_11;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  __HAL_RCC_I2C2_CLK_ENABLE();
  __HAL_RCC_I2C2_FORCE_RESET();
  __HAL_RCC_I2C2_RELEASE_RESET();
}

static void _i2c_init(void) {
  if (i2c_handle.Instance) {
    return;
  }

  i2c_handle.Instance = I2C2;
  i2c_handle.Init.Timing = 0x70B03140;
  i2c_handle.Init.OwnAddress1 = 0;  // master
  i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  i2c_handle.Init.OwnAddress2 = 0;
  i2c_handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  i2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_OK != HAL_I2C_Init(&i2c_handle)) {
    ensure(secfalse, NULL);
    return;
  }

  if (HAL_I2CEx_ConfigAnalogFilter(&i2c_handle, I2C_ANALOGFILTER_ENABLE) !=
      HAL_OK) {
    ensure(secfalse, NULL);
    return;
  } else {
    if (HAL_I2CEx_ConfigDigitalFilter(&i2c_handle, 0) != HAL_OK) {
      ensure(secfalse, NULL);
      return;
    }
  }
}

static void _i2c_deinit(void) {
  if (i2c_handle.Instance) {
    HAL_I2C_DeInit(&i2c_handle);
    i2c_handle.Instance = NULL;
  }
}

void touch_init(void) {
  uint8_t id;
  _i2c_msp_init();
  _i2c_init();
  if (HAL_I2C_Mem_Read(&i2c_handle, TOUCH_ADDRESS, 0xa8, 1, &id, 1, 1000) ==
      HAL_OK) {
  }
}

void touch_power_on(void) { return; }

void touch_power_off(void) { _i2c_deinit(); }

void touch_sensitivity(uint8_t value) {
  // set panel threshold (TH_GROUP) - default value is 0x12
  uint8_t touch_panel_threshold[] = {0x80, value};
  ensure(sectrue *
             (HAL_OK == HAL_I2C_Master_Transmit(
                            &i2c_handle, TOUCH_ADDRESS, touch_panel_threshold,
                            sizeof(touch_panel_threshold), 10)),
         NULL);
}

uint32_t touch_is_detected(void) {
  // check the interrupt line coming in from the CTPM.
  // the line goes low when a touch event is actively detected.
  // reference section 1.2 of "Application Note for FT6x06 CTPM".
  // we configure the touch controller to use "interrupt polling mode".
  return GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
}
#if 0
uint32_t touch_read(void) {
  static uint8_t touch_data[TOUCH_PACKET_SIZE];
  uint8_t previous_touch_data[TOUCH_PACKET_SIZE];
  uint32_t xy;
  if (!touch_is_detected()) {
    return 0;
  }
  display_printf("touch detected\n");
  if (HAL_I2C_Mem_Read(&i2c_handle, TOUCH_ADDRESS, 0xD000, 2, touch_data, 7,
                       1000) != HAL_OK) {
    return 0;
  }
  if (touch_data[6] != 0xAB) {
    return 0;
  }
  display_printf("\n %02x %02x %02x %02x %02x %02x %02x \n", touch_data[0],
                 touch_data[1], touch_data[2], touch_data[3], touch_data[4],
                 touch_data[5], touch_data[6]);
  if (touch_data[0] == 0x06 && touch_data[5] == 0x01) {
    HAL_Delay(5);
    while (1) {
      if (HAL_I2C_Mem_Read(&i2c_handle, TOUCH_ADDRESS, 0xD000, 2, touch_data, 7,
                           1000) != HAL_OK) {
        return 0;
      }
      display_printf("\n %02x %02x %02x %02x %02x %02x %02x \n", touch_data[0],
                     touch_data[1], touch_data[2], touch_data[3], touch_data[4],
                     touch_data[5], touch_data[6]);
      if (touch_data[6] != 0xAB) {
        return 0;
      }
      if (touch_data[5] > 1) {
        return 0;
      }
      if (touch_data[0] == 0x06) {
        memcpy(previous_touch_data, touch_data, TOUCH_PACKET_SIZE);
      }
      if (touch_data[0] == 0xAB) {
        break;
      }
    }
    memcpy(touch_data, previous_touch_data, TOUCH_PACKET_SIZE);
    // touch points
    if (touch_data[5] == 0x01) {
      xy = touch_pack_xy((X_POS_MSB << 4) | X_POS_LSB,
                         (Y_POS_MSB << 4) | Y_POS_LSB);
      // display_printf("x= %d,y=%d \n", (X_POS_MSB << 4) | X_POS_LSB,
      //                (Y_POS_MSB << 4) | Y_POS_LSB);
      return TOUCH_END | xy;
    }
  }

  return 0;
}
#else

uint32_t touch_num_detected(void) {
  uint8_t touch_data[TOUCH_PACKET_SIZE] = {0};
  if (HAL_I2C_Mem_Read(&i2c_handle, TOUCH_ADDRESS, 0xD000, 2, touch_data, 7,
                       1000) != HAL_OK) {
    return 0;
  }
  if (touch_data[6] != 0xAB) {
    return 0;
  }
  if (touch_data[0] == 0x06) {
    return touch_data[5] & 0x0F;
  }
  return 0;
}

uint32_t touch_read(void) {
  static uint8_t touch_data[TOUCH_PACKET_SIZE],
      previous_touch_data[TOUCH_PACKET_SIZE];
  static uint32_t xy;
  static int touching;

  int last_packet = 0;
  // if (!touch_is_detected()) {
  //   if (touching) {
  //     last_packet = 1;
  //   } else {
  //     return 0;
  //   }
  // }

  memset(touch_data, 0x00, sizeof(touch_data));
  if (HAL_I2C_Mem_Read(&i2c_handle, TOUCH_ADDRESS, 0xD000, 2, touch_data, 7,
                       1000) != HAL_OK) {
    return 0;
  }
  if (touch_data[6] != 0xAB) {
    return 0;
  }

  if (touch_data[0] == 0xAB) {
    if (touching) {
      last_packet = 1;
    } else {
      return 0;
    }
  }

  if (touch_data[0] == 0x06 && touch_data[5] == 0x01) {
    if (0 == memcmp(previous_touch_data, touch_data, TOUCH_PACKET_SIZE)) {
    } else {
      memcpy(previous_touch_data, touch_data, TOUCH_PACKET_SIZE);
    }
    xy = touch_pack_xy((X_POS_MSB << 4) | X_POS_LSB,
                       (Y_POS_MSB << 4) | Y_POS_LSB);
    if (touching == 0) {
      touching = 1;
      return TOUCH_START | xy;
    } else {
      return TOUCH_MOVE | xy;
    }
  }
  if (last_packet) {
    touching = 0;
    return TOUCH_END | xy;
  }
  return 0;
}
#endif
uint32_t touch_click(void) { return touch_read(); }

void touch_test(void) {
  int pos;
  int i = 0;
  while (1) {
    pos = touch_read();
    if (pos & TOUCH_START) {
      display_printf(" %d touch start x= %d,y=%d \n", i, (pos >> 12) & 0xfff,
                     pos & 0xfff);
    }
    if (pos & TOUCH_MOVE) {
      display_printf(" %d touch move x= %d,y=%d \n", i, (pos >> 12) & 0xfff,
                     pos & 0xfff);
    }
    if (pos & TOUCH_END) {
      display_printf(" %d touch end x= %d,y=%d \n", i, (pos >> 12) & 0xfff,
                     pos & 0xfff);
    }
    i++;
  }
}

#else

#define TOUCH_ADDRESS \
  (0x1AU << 1)  // the HAL requires the 7-bit address to be shifted by one bit
#define TOUCH_PACKET_SIZE 7U
#define EVENT_PRESS_DOWN 0x00U
#define EVENT_CONTACT 0x80U
#define EVENT_LIFT_UP 0x40U
#define EVENT_NO_EVENT 0xC0U
#define GESTURE_NO_GESTURE 0x00U
#define X_POS_MSB (touch_data[3] & 0x0FU)
#define X_POS_LSB (touch_data[4])
#define Y_POS_MSB (touch_data[5] & 0x0FU)
#define Y_POS_LSB (touch_data[6])

static void touch_default_pin_state(void) {
  // set power off and other pins as per section 3.5 of FT6236 datasheet
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,
                    GPIO_PIN_SET);  // CTP_ON/PB10 (active low) i.e.- CTPM power
                                    // off when set/high/log 1
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);  // CTP_I2C_SCL/PB6
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);  // CTP_I2C_SDA/PB7
  HAL_GPIO_WritePin(
      GPIOC, GPIO_PIN_4,
      GPIO_PIN_RESET);  // CTP_INT/PC4 normally an input, but drive low as an
                        // output while powered off
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,
                    GPIO_PIN_RESET);  // CTP_REST/PC5 (active low) i.e.- CTPM
                                      // held in reset until released

  // set above pins to OUTPUT / NOPULL
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.Pin = GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  // in-case power was on, or CTPM was active make sure to wait long enough
  // for these changes to take effect. a reset needs to be low for
  // a minimum of 5ms. also wait for power circuitry to stabilize (if it
  // changed).
  HAL_Delay(100);  // 100ms (being conservative)
}

static void touch_active_pin_state(void) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);  // CTP_ON/PB10
  HAL_Delay(10);  // we need to wait until the circuit fully kicks-in

  GPIO_InitTypeDef GPIO_InitStructure;

  // configure CTP I2C SCL and SDA GPIO lines (PB6 & PB7)
  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed =
      GPIO_SPEED_FREQ_LOW;  // I2C is a KHz bus and low speed is still good into
                            // the low MHz
  GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  // PC4 capacitive touch panel module (CTPM) interrupt (INT) input
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Pin = GPIO_PIN_4;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);  // release CTPM reset
  HAL_Delay(310);  // "Time of starting to report point after resetting" min is
                   // 300ms, giving an extra 10ms
}

void touch_init(void) { touch_default_pin_state(); }

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
  // enable I2C clock
  __HAL_RCC_I2C1_CLK_ENABLE();
  // GPIO have already been initialised by touch_init
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
  __HAL_RCC_I2C1_CLK_DISABLE();
}

static void _i2c_init(void) {
  if (i2c_handle.Instance) {
    return;
  }

  i2c_handle.Instance = I2C1;

  i2c_handle.Init.ClockSpeed = 200000;
  i2c_handle.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
  i2c_handle.Init.OwnAddress1 = 0xFE;  // master
  i2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  i2c_handle.Init.OwnAddress2 = 0;
  i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  i2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_OK != HAL_I2C_Init(&i2c_handle)) {
    ensure(secfalse, NULL);
    return;
  }
}

static void _i2c_deinit(void) {
  if (i2c_handle.Instance) {
    HAL_I2C_DeInit(&i2c_handle);
    i2c_handle.Instance = NULL;
  }
}

static void _i2c_ensure_pin(uint16_t GPIO_Pin, GPIO_PinState PinState) {
  HAL_GPIO_WritePin(GPIOB, GPIO_Pin, PinState);
  while (HAL_GPIO_ReadPin(GPIOB, GPIO_Pin) != PinState)
    ;
}

// I2C cycle described in section 2.9.7 of STM CD00288116 Errata sheet
//
// https://www.st.com/content/ccc/resource/technical/document/errata_sheet/7f/05/b0/bc/34/2f/4c/21/CD00288116.pdf/files/CD00288116.pdf/jcr:content/translations/en.CD00288116.pdf

static void _i2c_cycle(void) {
  // PIN6 is SCL, PIN7 is SDA

  // 1. Disable I2C peripheral
  _i2c_deinit();

  // 2. Configure SCL/SDA as GPIO OUTPUT Open Drain
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_Delay(50);

  // 3. Check SCL and SDA High level
  _i2c_ensure_pin(GPIO_PIN_6, GPIO_PIN_SET);
  _i2c_ensure_pin(GPIO_PIN_7, GPIO_PIN_SET);
  // 4+5. Check SDA Low level
  _i2c_ensure_pin(GPIO_PIN_7, GPIO_PIN_RESET);
  // 6+7. Check SCL Low level
  _i2c_ensure_pin(GPIO_PIN_6, GPIO_PIN_RESET);
  // 8+9. Check SCL High level
  _i2c_ensure_pin(GPIO_PIN_6, GPIO_PIN_SET);
  // 10+11.  Check SDA High level
  _i2c_ensure_pin(GPIO_PIN_7, GPIO_PIN_SET);

  // 12. Configure SCL/SDA as Alternate function Open-Drain
  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_Delay(50);

  // 13. Set SWRST bit in I2Cx_CR1 register
  __HAL_RCC_I2C1_FORCE_RESET();
  HAL_Delay(50);

  // 14. Clear SWRST bit in I2Cx_CR1 register
  __HAL_RCC_I2C1_RELEASE_RESET();

  // 15. Enable the I2C peripheral
  _i2c_init();
  HAL_Delay(10);
}

void touch_power_on(void) {
  if (i2c_handle.Instance) {
    return;
  }

  // turn on CTP circuitry
  touch_active_pin_state();
  HAL_Delay(50);

  // I2C device interface configuration
  _i2c_init();

  // set register 0xA4 G_MODE to interrupt polling mode (0x00). basically, CTPM
  // keeps this input line (to PC4) low while a finger is on the screen.
  uint8_t touch_panel_config[] = {0xA4, 0x00};
  ensure(
      sectrue * (HAL_OK == HAL_I2C_Master_Transmit(
                               &i2c_handle, TOUCH_ADDRESS, touch_panel_config,
                               sizeof(touch_panel_config), 10)),
      NULL);

  touch_sensitivity(0x06);
}

void touch_power_off(void) {
  _i2c_deinit();
  // turn off CTP circuitry
  HAL_Delay(50);
  touch_default_pin_state();
}

void touch_sensitivity(uint8_t value) {
  // set panel threshold (TH_GROUP) - default value is 0x12
  uint8_t touch_panel_threshold[] = {0x80, value};
  ensure(sectrue *
             (HAL_OK == HAL_I2C_Master_Transmit(
                            &i2c_handle, TOUCH_ADDRESS, touch_panel_threshold,
                            sizeof(touch_panel_threshold), 10)),
         NULL);
}

uint32_t touch_is_detected(void) {
  // check the interrupt line coming in from the CTPM.
  // the line goes low when a touch event is actively detected.
  // reference section 1.2 of "Application Note for FT6x06 CTPM".
  // we configure the touch controller to use "interrupt polling mode".
  return GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4);
}

uint32_t touch_read(void) {
  static uint8_t touch_data[TOUCH_PACKET_SIZE],
      previous_touch_data[TOUCH_PACKET_SIZE];
  static uint32_t xy;
  static int touching;

  int last_packet = 0;
  if (!touch_is_detected()) {
    // only poll when the touch interrupt is active.
    // when it's inactive, we might need to read one last data packet to get to
    // the TOUCH_END event, which clears the `touching` flag.
    if (touching) {
      last_packet = 1;
    } else {
      return 0;
    }
  }

  uint8_t outgoing[] = {0x00};  // start reading from address 0x00
  int result = HAL_I2C_Master_Transmit(&i2c_handle, TOUCH_ADDRESS, outgoing,
                                       sizeof(outgoing), 1);
  if (result != HAL_OK) {
    if (result == HAL_BUSY) _i2c_cycle();
    return 0;
  }

  if (HAL_OK != HAL_I2C_Master_Receive(&i2c_handle, TOUCH_ADDRESS, touch_data,
                                       TOUCH_PACKET_SIZE, 1)) {
    return 0;  // read failure
  }

  if (0 == memcmp(previous_touch_data, touch_data, TOUCH_PACKET_SIZE)) {
    return 0;  // polled and got the same event again
  } else {
    memcpy(previous_touch_data, touch_data, TOUCH_PACKET_SIZE);
  }

  const uint32_t number_of_touch_points =
      touch_data[2] & 0x0F;  // valid values are 0, 1, 2 (invalid 0xF before
                             // first touch) (tested with FT6206)
  const uint32_t event_flag = touch_data[3] & 0xC0;
  if (touch_data[1] == GESTURE_NO_GESTURE) {
    xy = touch_pack_xy((X_POS_MSB << 8) | X_POS_LSB,
                       (Y_POS_MSB << 8) | Y_POS_LSB);
    if ((number_of_touch_points == 1) && (event_flag == EVENT_PRESS_DOWN)) {
      touching = 1;
      return TOUCH_START | xy;
    } else if ((number_of_touch_points == 1) && (event_flag == EVENT_CONTACT)) {
      return TOUCH_MOVE | xy;
    } else if ((number_of_touch_points == 0) && (event_flag == EVENT_LIFT_UP)) {
      touching = 0;
      return TOUCH_END | xy;
    }
  }

  if (last_packet) {
    // interrupt line is inactive, we didn't read valid touch data, and as far
    // as we know, we never sent a TOUCH_END event.
    touching = 0;
    return TOUCH_END | xy;
  }

  return 0;
}

uint32_t touch_click(void) {
  uint32_t r = 0;
  // flush touch events if any
  while (touch_read()) {
  }
  // wait for TOUCH_START
  while ((touch_read() & TOUCH_START) == 0) {
  }
  // wait for TOUCH_END
  while (((r = touch_read()) & TOUCH_END) == 0) {
  }
  // flush touch events if any
  while (touch_read()) {
  }
  // return last touch coordinate
  return r;
}

#endif
