/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
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

#include "buttons.h"
#include "common.h"
#include "oled.h"
#include "util.h"

#include "layout.h"

struct buttonState button = {0};
static volatile bool btn_up_long = false, btn_down_long = false;
#define SHAKE_DELAY 3

#if !EMULATOR
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/syscfg.h>
#include "ble.h"

#include <libopencm3/cm3/scb.h>

#if !EMULATOR
uint8_t change_ble_sta_flag = 0;
#endif

static volatile int button_timer_enable = 0;
static volatile uint32_t button_timer_counter = 0;
static volatile uint32_t up_btn_timer_counter = 0;
static volatile int up_btn_timer_enable = 0;
static volatile uint32_t down_btn_timer_counter = 0;
static volatile int down_btn_timer_enable = 0;
static volatile int long_press_enable = false;

uint16_t buttonRead(void) {
  uint16_t tmp = 0x00;
  tmp |= gpio_get(BTN_PORT, BTN_PIN_YES);
  tmp |= gpio_get(BTN_PORT, BTN_PIN_UP);
  tmp |= gpio_get(BTN_PORT, BTN_PIN_DOWN);
  tmp |= gpio_get(BTN_PORT_NO, BTN_PIN_NO);
  return tmp;
}

void buttonsIrqInit(void) {
  // enable SYSCFG	clock
  rcc_periph_clock_enable(RCC_SYSCFG);

  // remap EXTI0 to GPIOC
  SYSCFG_EXTICR1 = 0x20;

  // set EXTI
  exti_select_source(BTN_PIN_NO, BTN_PORT_NO);
  exti_set_trigger(BTN_PIN_NO, EXTI_TRIGGER_BOTH);
  exti_enable_request(BTN_PIN_NO);

#if FEITIAN_PCB_V1_3
  // set NVIC
  nvic_set_priority(NVIC_EXTI1_IRQ, 0);
  nvic_enable_irq(NVIC_EXTI1_IRQ);
#else
  // set NVIC
  nvic_set_priority(NVIC_EXTI0_IRQ, 0);
  nvic_enable_irq(NVIC_EXTI0_IRQ);
#endif
}
#if FEITIAN_PCB_V1_3
void exti1_isr(void) {
  if (exti_get_flag_status(BTN_PIN_NO)) {
    SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;  // exit sleep mode
    exti_reset_request(BTN_PIN_NO);
    if (gpio_get(BTN_PORT_NO, BTN_PIN_NO)) {
      button_timer_enable = 1;
      button_timer_counter = 0;
    }
  }
}

void clear_button_irq(void) {
  if (exti_get_flag_status(BTN_PIN_NO)) {
    exti_reset_request(BTN_PIN_NO);
  }
}
#else
void exti0_isr(void) {
  if (exti_get_flag_status(BTN_PIN_NO)) {
    SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;  // exit sleep mode
    exti_reset_request(BTN_PIN_NO);
    if (gpio_get(BTN_PORT_NO, BTN_PIN_NO)) {
      button_timer_enable = 1;
      button_timer_counter = 0;
    }
  }
}
#endif

void enableLongPress(bool on) { long_press_enable = on; }

bool getLongPressStatus(void) { return long_press_enable; }

bool isLongPress(uint8_t key) {
  if (key == KEY_UP_OR_DOWN) return btn_up_long || btn_down_long;
  if (key == KEY_UP) return btn_up_long;
  if (key == KEY_DOWN) return btn_down_long;
  return false;
}

void buttonsTimer(void) {
  if (button_timer_enable) {
    button_timer_counter++;
    if (button_timer_counter > 1) {
      // buttonUpdateIrq();
      if (gpio_get(BTN_PORT_NO, BTN_PIN_NO) == 0) {  // key up
        button_timer_enable = 0;
      }
    }
    if (button_timer_counter > 2) {  // long press
      sys_shutdown();
    }
  }
}

void longPressTimer(void) {
  if ((buttonRead() & BTN_PIN_UP) == 0 && up_btn_timer_enable == 0) {
    up_btn_timer_counter++;
    if (up_btn_timer_counter > 2) {
      up_btn_timer_enable = 1;
      btn_up_long = true;
    }
  } else {
    up_btn_timer_counter = 0;
    up_btn_timer_enable = 0;
  }
  if ((buttonRead() & BTN_PIN_DOWN) == 0 && down_btn_timer_enable == 0) {
    down_btn_timer_counter++;
    if (down_btn_timer_counter > 2) {
      down_btn_timer_enable = 1;
      btn_down_long = true;
    }
  } else {
    down_btn_timer_counter = 0;
    down_btn_timer_enable = 0;
  }
}

bool checkButtonOrTimeout(uint8_t btn, TimerOut type) {
  bool flag = false;
  buttonUpdate();
  if (timer_out_get(type) == 0) flag = true;
  switch (btn) {
    case BTN_PIN_YES:
      if (button.YesUp) flag = true;
      break;
    case BTN_PIN_NO:
      if (button.NoUp) flag = true;
      break;
    case BTN_PIN_UP:
      if (button.UpUp) flag = true;
      break;
    case BTN_PIN_DOWN:
      if (button.DownUp) flag = true;
      break;
    default:
      break;
  }
  if (true == flag) timer_out_set(type, 0);
  return flag;
}

bool waitButtonResponse(uint8_t btn, uint32_t time_out) {
  bool flag = false;
  timer_out_set(timer_out_oper, time_out);
  while (1) {
    if (timer_out_get(timer_out_oper) == 0) {
      flag = false;
      break;
    }
    buttonUpdate();
    if (button.YesUp) {
      if (btn == BTN_PIN_YES)
        flag = true;
      else
        flag = false;
      break;
    } else if (button.NoUp) {
      if (btn == BTN_PIN_NO)
        flag = true;
      else
        flag = false;
      break;
    } else if (button.UpUp) {
      if (btn == BTN_PIN_UP)
        flag = true;
      else
        flag = false;
      break;
    } else if (button.DownUp) {
      if (btn == BTN_PIN_DOWN)
        flag = true;
      else
        flag = false;
      break;
    }
  }
  timer_out_set(timer_out_oper, 0);
  return flag;
}
#endif

void buttonUpdate() {
  static uint16_t last_state =
      (BTN_PIN_YES | BTN_PIN_UP | BTN_PIN_DOWN) & (~BTN_PIN_NO);

  uint16_t state = buttonRead();

  if ((state & BTN_PIN_YES) == 0) {         // Yes button is down
    if ((last_state & BTN_PIN_YES) == 0) {  // last Yes was down
      if (button.YesDown < 2000000000) button.YesDown++;
      button.YesUp = false;
    } else {  // last Yes was up
      button.YesDown = 0;
      button.YesUp = false;
    }
  } else {  // Yes button is up
    if ((last_state & BTN_PIN_YES) == 0 &&
        button.YesDown > SHAKE_DELAY) {  // last Yes was down
      button.YesDown = 0;
      button.YesUp = true;
    } else {  // last Yes was up
      button.YesDown = 0;
      button.YesUp = false;
    }
  }
#if !EMULATOR
  if ((state & BTN_PIN_NO)) {         // No button is down
    if ((last_state & BTN_PIN_NO)) {  // last No was down
      if (button.NoDown < 2000000000) button.NoDown++;
      button.NoUp = false;
    } else {  // last No was up
      button.NoDown = 0;
      button.NoUp = false;
    }
  } else {  // No button is up
    if ((last_state & BTN_PIN_NO) &&
        button.NoDown > SHAKE_DELAY) {  // last No was down
      button.NoDown = 0;
      button.NoUp = true;
    } else {  // last No was up
      button.NoDown = 0;
      button.NoUp = false;
    }
  }
#else
  if ((state & BTN_PIN_NO) == 0) {         // No button is down
    if ((last_state & BTN_PIN_NO) == 0) {  // last No was down
      if (button.NoDown < 2000000000) button.NoDown++;
      button.NoUp = false;
    } else {  // last No was up
      button.NoDown = 0;
      button.NoUp = false;
    }
  } else {                                 // No button is up
    if ((last_state & BTN_PIN_NO) == 0) {  // last No was down
      button.NoDown = 0;
      button.NoUp = true;
    } else {  // last No was up
      button.NoDown = 0;
      button.NoUp = false;
    }
  }
#endif
  if ((state & BTN_PIN_UP) == 0) {         // UP button is down
    if ((last_state & BTN_PIN_UP) == 0) {  // last UP was down
      if (button.UpDown < 2000000000) button.UpDown++;
      button.UpUp = false;
    } else {  // last UP was up
      button.UpDown = 0;
      button.UpUp = false;
    }
  } else {  // UP button is up
    if ((last_state & BTN_PIN_UP) == 0 &&
        button.UpDown > SHAKE_DELAY) {  // last UP was down
      if (btn_up_long) {
        btn_up_long = false;
        button.UpUp = false;
      } else {
        button.UpUp = true;
      }
      button.UpDown = 0;
    } else {  // last UP was up
      button.UpDown = 0;
      button.UpUp = false;
      btn_up_long = false;
    }
  }

  if ((state & BTN_PIN_DOWN) == 0) {         // down button is down
    if ((last_state & BTN_PIN_DOWN) == 0) {  // last down was down
      if (button.DownDown < 2000000000) button.DownDown++;
      button.DownUp = false;
    } else {  // last down was up
      button.DownDown = 0;
      button.DownUp = false;
    }
  } else {  // down button is up
    if ((last_state & BTN_PIN_DOWN) == 0 &&
        button.DownDown > SHAKE_DELAY) {  // last down was down
      if (btn_down_long) {
        btn_down_long = false;
        button.DownUp = false;
      } else {
        button.DownUp = true;
      }
      button.DownDown = 0;
    } else {  // last down was up
      button.DownDown = 0;
      button.DownUp = false;
      btn_down_long = false;
    }
  }
#if !EMULATOR
  if (button.YesUp || button.NoUp || button.UpUp || button.DownUp) {
    timer_sleep_start_reset();
    unregister_timer("poweroff");
  }
#endif
  last_state = state;
}

bool hasbutton(void) {
  buttonUpdate();
  if (button.YesUp || button.NoUp || button.UpUp || button.DownUp) {
    return true;
  }
  return false;
}

uint8_t keyScan(void) {
  delay_ms(5);
  buttonUpdate();
  if (button.YesUp)
    return KEY_CONFIRM;
  else if (button.NoUp)
    return KEY_CANCEL;
  else if (button.UpUp)
    return KEY_UP;
  else if (button.DownUp)
    return KEY_DOWN;

  return KEY_NULL;
}

uint8_t waitKey(uint32_t time_out, uint8_t mode) {
  uint8_t key = KEY_NULL;
  timer_out_set(timer_out_oper, time_out);
  while (1) {
    if (time_out > 0 && timer_out_get(timer_out_oper) == 0) break;
    key = keyScan();
    if (key != KEY_NULL) {
      if (mode == 0) {
        return key;
      } else {
        if (key == KEY_CONFIRM || key == KEY_CANCEL) return key;
      }
    }
  }
  return KEY_NULL;
}
