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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/repl.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "shared/runtime/pyexec.h"

#include "ports/stm32/gccollect.h"
#include "ports/stm32/pendsv.h"

#include "bl_check.h"
#include "button.h"
#include "common.h"
#include "compiler_traits.h"
#include "display.h"
#include "flash.h"
#include "mpu.h"
#include "random_delays.h"
#ifdef SYSTEM_VIEW
#include "systemview.h"
#endif
#include "rng.h"
// #include "sdcard.h"
#include "atca_api.h"
#include "atca_hal.h"
#include "mipi_lcd.h"
#include "qspi_flash.h"
#include "spi.h"
#include "supervise.h"
#include "timer.h"
#include "touch.h"
#ifdef USE_SECP256K1_ZKP
#include "zkp_context.h"
#endif

// from util.s
extern void shutdown_privileged(void);

int main(void) {
  SystemCoreClockUpdate();
  lcd_para_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
  random_delays_init();

#ifdef RDI
  rdi_start();
#endif

  // reinitialize HAL for Trezor One
#if TREZOR_MODEL == 1
  HAL_Init();
#endif

  collect_hw_entropy();

#ifdef SYSTEM_VIEW
  enable_systemview();
#endif

#if TREZOR_MODEL == T
#if PRODUCTION
  check_and_replace_bootloader();
#endif
  // Enable MPU
  mpu_config_firmware();
#endif

  // Init peripherals
  pendsv_init();

#if TREZOR_MODEL == 1
  display_init();
  button_init();
#endif
  display_clear();

#if TREZOR_MODEL == T
  // display_init_seq();
  // sdcard_init();
  touch_init();
  touch_power_on();
  spi_slave_init();
  qspi_flash_init();
  qspi_flash_config();
  qspi_flash_memory_mapped();

  atca_init();
  atca_config_init();

  // timer_init();

  // jump to unprivileged mode
  // http://infocenter.arm.com/help/topic/com.arm.doc.dui0552a/CHDBIBGJ.html
  // __asm__ volatile("msr control, %0" ::"r"(0x1));
  // __asm__ volatile("isb");

#endif

#ifdef USE_SECP256K1_ZKP
  ensure(sectrue * (zkp_context_init() == 0), NULL);
#endif
  printf("CORE: Preparing stack\n");
  // Stack limit should be less than real stack size, so we have a chance
  // to recover from limit hit.
  mp_stack_set_top(&_estack);
  mp_stack_set_limit((char *)&_estack - (char *)&_heap_end - 1024);

#if MICROPY_ENABLE_PYSTACK
  static mp_obj_t pystack[1024];
  mp_pystack_init(pystack, &pystack[MP_ARRAY_SIZE(pystack)]);
#endif

  // GC init
  printf("CORE: Starting GC\n");
  gc_init(&_heap_start, &_heap_end);

  // Interpreter init
  printf("CORE: Starting interpreter\n");
  mp_init();
  mp_obj_list_init(mp_sys_argv, 0);
  mp_obj_list_init(mp_sys_path, 0);
  mp_obj_list_append(
      mp_sys_path,
      MP_OBJ_NEW_QSTR(MP_QSTR_));  // current dir (or base dir of the script)

  // Execute the main script
  printf("CORE: Executing main script\n");
  pyexec_frozen_module("main.py");
  // Clean up
  printf("CORE: Main script finished, cleaning up\n");
  mp_deinit();

  return 0;
}

// MicroPython default exception handler

void __attribute__((noreturn)) nlr_jump_fail(void *val) {
  error_shutdown("Internal error", "(UE)", NULL, NULL);
}

// interrupt handlers

void NMI_Handler(void) {
  // Clock Security System triggered NMI
  // if ((RCC->CIR & RCC_CIR_CSSF) != 0)
  { error_shutdown("Internal error", "(CS)", NULL, NULL); }
}

void hard_fault_handler(unsigned int *hardfault_args) {
  unsigned int stacked_r0;
  unsigned int stacked_r1;
  unsigned int stacked_r2;
  unsigned int stacked_r3;
  unsigned int stacked_r12;
  unsigned int stacked_lr;
  unsigned int stacked_pc;
  unsigned int stacked_psr;

  stacked_r0 = ((unsigned long)hardfault_args[0]);
  stacked_r1 = ((unsigned long)hardfault_args[1]);
  stacked_r2 = ((unsigned long)hardfault_args[2]);
  stacked_r3 = ((unsigned long)hardfault_args[3]);
  stacked_r12 = ((unsigned long)hardfault_args[4]);
  stacked_lr = ((unsigned long)hardfault_args[5]);
  stacked_pc = ((unsigned long)hardfault_args[6]);
  stacked_psr = ((unsigned long)hardfault_args[7]);
  display_printf("[Hard fault handler]\n");
  display_printf("R0 = %x\n", stacked_r0);
  display_printf("R1 = %x\n", stacked_r1);
  display_printf("R2 = %x\n", stacked_r2);
  display_printf("R3 = %x\n", stacked_r3);
  display_printf("R12 = %x\n", stacked_r12);
  display_printf("LR = %x\n", stacked_lr);
  display_printf("PC = %x\n", stacked_pc);
  display_printf("PSR = %x\n", stacked_psr);
  display_printf("BFAR = %x\n", (*((volatile unsigned int *)(0xE000ED38))));
  display_printf("CFSR = %x\n", (*((volatile unsigned int *)(0xE000ED28))));
  display_printf("HFSR = %x\n", (*((volatile unsigned int *)(0xE000ED2C))));
  display_printf("DFSR = %x\n", (*((volatile unsigned int *)(0xE000ED30))));
  display_printf("AFSR = %x\n", (*((volatile unsigned int *)(0xE000ED3C))));
  exit(0);
  return;
}

// void HardFault_Handler(void) {
//   error_shutdown("Internal error", "(HF)", NULL, NULL);
// }

void MemManage_Handler(void) {
  error_shutdown("Internal error", "(MM)", NULL, NULL);
}

void BusFault_Handler(void) {
  error_shutdown("Internal error", "(BF)", NULL, NULL);
}

void UsageFault_Handler(void) {
  error_shutdown("Internal error", "(UF)", NULL, NULL);
}

void SVC_C_Handler(uint32_t *stack) {
  uint8_t svc_number = ((uint8_t *)stack[6])[-2];
  switch (svc_number) {
    case SVC_ENABLE_IRQ:
      HAL_NVIC_EnableIRQ(stack[0]);
      break;
    case SVC_DISABLE_IRQ:
      HAL_NVIC_DisableIRQ(stack[0]);
      break;
    case SVC_SET_PRIORITY:
      NVIC_SetPriority(stack[0], stack[1]);
      break;
#ifdef SYSTEM_VIEW
    case SVC_GET_DWT_CYCCNT:
      cyccnt_cycles = *DWT_CYCCNT_ADDR;
      break;
#endif
    case SVC_SHUTDOWN:
      shutdown_privileged();
      for (;;)
        ;
      break;
    default:
      stack[0] = 0xffffffff;
      break;
  }
}

__attribute__((naked)) void SVC_Handler(void) {
  __asm volatile(
      " tst lr, #4    \n"    // Test Bit 3 to see which stack pointer we should
                             // use.
      " ite eq        \n"    // Tell the assembler that the nest 2 instructions
                             // are if-then-else
      " mrseq r0, msp \n"    // Make R0 point to main stack pointer
      " mrsne r0, psp \n"    // Make R0 point to process stack pointer
      " b SVC_C_Handler \n"  // Off to C land
  );
}

// MicroPython builtin stubs

mp_import_stat_t mp_import_stat(const char *path) {
  return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
