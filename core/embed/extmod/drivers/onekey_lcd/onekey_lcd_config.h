#ifndef ONEKEY_LCD_CONFIG_H
#define ONEKEY_LCD_CONFIG_H

#include "onekey_lcd_common.h"
#include <sdram.h>

#define DISPLAY_MEMORY_BASE FMC_SDRAM_LTDC_BUFFER_ADDRESS
#define DISPLAY_MEMORY_FB_A DISPLAY_MEMORY_BASE
#define DISPLAY_MEMORY_FB_B DISPLAY_MEMORY_BASE + LTDC_PIXEL_DATA_LAYER_SIZE

/* Back-light control pin */
#define LCD_BL_CTRL_PIN GPIO_PIN_0
#define LCD_BL_CTRL_GPIO_PORT GPIOK

/* LCD reset pin */
#define LCD_RESET_PIN GPIO_PIN_3
#define LCD_RESET_GPIO_PORT GPIOG

/* LCD tearing effect pin */
#define LCD_TE_PIN GPIO_PIN_2
#define LCD_TE_GPIO_PORT GPIOJ


#endif // ONEKEY_LCD_CONFIG_H