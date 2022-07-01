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
#include "emmc.h"
#include "ff.h"
#include "flash.h"
#include "mpu.h"
#include "random_delays.h"
#include "usart.h"
#ifdef SYSTEM_VIEW
#include "systemview.h"
#endif
#include "rng.h"
// #include "sdcard.h"
#include "atca_api.h"
#include "atca_hal.h"
#include "device.h"
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

static void copyflash2sdram(void) {
  extern int _flash2_load_addr, _flash2_start, _flash2_end;
  volatile uint32_t *dst = (volatile uint32_t *)&_flash2_start;
  volatile uint32_t *end = (volatile uint32_t *)&_flash2_end;
  volatile uint32_t *src = (volatile uint32_t *)&_flash2_load_addr;

  while (dst < end) {
    *dst++ = *src++;
  }
}

PARTITION VolToPart[FF_VOLUMES] = {
    {0, 1},
    {0, 2},
};

int main(void) {
  SystemCoreClockUpdate();

  // Enable MPU
  mpu_config_firmware();

  qspi_flash_init();
  qspi_flash_config();
  qspi_flash_memory_mapped();

  copyflash2sdram();
  lcd_para_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
  random_delays_init();

#ifdef RDI
  rdi_start();
#endif

  // reinitialize HAL for Trezor One
#if defined TREZOR_MODEL_1
  HAL_Init();
#endif

  collect_hw_entropy();

#ifdef SYSTEM_VIEW
  enable_systemview();
#endif

  ble_usart_init();

  device_para_init();

#if defined TREZOR_MODEL_T
#if PRODUCTION
  // check_and_replace_bootloader();
#endif
#endif

  // Init peripherals
  pendsv_init();

#if defined TREZOR_MODEL_1
  display_init();
  button_init();
#endif
  display_clear();

#if defined TREZOR_MODEL_T
  // display_init_seq();
  // sdcard_init();
  touch_init();
  touch_power_on();
  spi_slave_init();

  atca_init();
  atca_config_init();

  emmc_init();

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

// Hard fault handler
#if defined SYSTEM_VIEW
enum { r0, r1, r2, r3, r12, lr, pc, psr };
void STACK_DUMP(unsigned int *stack) {
  display_printf("[STACK DUMP]\n");
  display_printf("R0 = 0x%08x\n", stack[r0]);
  display_printf("R1 = 0x%08x\n", stack[r1]);
  display_printf("R2 = 0x%08x\n", stack[r2]);
  display_printf("R3 = 0x%08x\n", stack[r3]);
  display_printf("R12 = 0x%08x\n", stack[r12]);
  display_printf("LR = 0x%08x\n", stack[lr]);
  display_printf("PC = 0x%08x\n", stack[pc]);
  display_printf("PSR = 0x%08x\n", stack[psr]);
  display_printf("BFAR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED38))));
  display_printf("CFSR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED28))));
  display_printf("HFSR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED2C))));
  display_printf("DFSR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED30))));
  display_printf("AFSR = 0x%08x\n", (*((volatile unsigned int *)(0xE000ED3C))));
  exit(0);
  return;
}

__attribute__((naked)) void HardFault_Handler(void) {
  __asm volatile(
      " tst lr, #4    \n"  // Test Bit 3 to see which stack pointer we should
                           // use.
      " ite eq        \n"  // Tell the assembler that the nest 2 instructions
                           // are if-then-else
      " mrseq r0, msp \n"  // Make R0 point to main stack pointer
      " mrsne r0, psp \n"  // Make R0 point to process stack pointer
      " b STACK_DUMP \n"   // Off to C land
  );
}
#else
void HardFault_Handler(void) {
  error_shutdown("Internal error", "(HF)", NULL, NULL);
}
#endif

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
    case SVC_RESET_SYSTEM:
      HAL_NVIC_SystemReset();
      while (1)
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
