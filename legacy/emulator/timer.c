/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2017 Saleem Rashid <trezor@saleemrashid.com>
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

#include <time.h>

#include "timer.h"

void timer_init(void) {}

static uint32_t timer_out_array[timer_out_null];
static void timer_out_decrease(void) {
  uint32_t i = timer_out_null;
  while (i--) {
    if (timer_out_array[i]) timer_out_array[i]--;
  }
}
void timer_out_set(TimerOut type, uint32_t val) { timer_out_array[type] = val; }
uint32_t timer_out_get(TimerOut type) { return timer_out_array[type]; }

uint32_t timer_ms(void) {
  static int counter = 0;
  struct timespec t = {0};
  counter++;
  clock_gettime(CLOCK_MONOTONIC, &t);

  uint32_t msec = t.tv_sec * 1000 + (t.tv_nsec / 1000000);
  if (counter > 1000) {
    counter = 0;
    timer_out_decrease();
  }
  return msec;
}

void delay_ms(uint32_t uiDelay_Ms) { (void)uiDelay_Ms; }
