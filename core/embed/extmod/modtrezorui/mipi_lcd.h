#ifndef _MIPI_LCD_H_
#define _MIPI_LCD_H_

#include <stdint.h>

#include "mipi_display.h"
#include "sdram.h"
#include "st7701s.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DISPLAY_MEMORY_BASE FMC_SDRAM_LTDC_BUFFER_ADDRESS

/* LCD reset pin */
#define LCD_RESET_PIN GPIO_PIN_3
#define LCD_RESET_GPIO_PORT GPIOG

/* Back-light control pin */
#define LCD_BL_CTRL_PIN GPIO_PIN_0
#define LCD_BL_CTRL_GPIO_PORT GPIOK

/* LCD tearing effect pin */
#define LCD_TE_PIN GPIO_PIN_2
#define LCD_TE_GPIO_PORT GPIOJ

// clang-format off
#define LCD_PIXEL_FORMAT_ARGB8888        0x00000000U   /*!< ARGB8888 LTDC pixel format */
#define LCD_PIXEL_FORMAT_RGB888          0x00000001U   /*!< RGB888 LTDC pixel format   */
#define LCD_PIXEL_FORMAT_RGB565          0x00000002U   /*!< RGB565 LTDC pixel format   */
// clang-format on

typedef struct time_sequence {
  int hsync;
  int hbp;
  int hfp;

  int vsync;
  int vbp;
  int vfp;
} LCD_TIME_SEQUENCE;

typedef struct {
  uint32_t x0;
  uint32_t x1;
  uint32_t y0;
  uint32_t y1;
  uint32_t pixel_format;
  uint32_t address;
} LTDC_LAYERCONFIG;

typedef struct {
  uint32_t xres;
  uint32_t yres;
  uint32_t bbp;
  uint32_t pixel_format;
  uint32_t fb_base;
} LCD_PARAMS;

void fb_write_pixel(uint32_t x_pos, uint32_t y_pos, uint32_t color);
void fb_fill_rect(uint32_t x_pos, uint32_t y_pos, uint32_t width,
                  uint32_t height, uint32_t color);
void fb_draw_hline(uint32_t x_pos, uint32_t y_pos, uint32_t len,
                   uint32_t color);
void fb_draw_vline(uint32_t x_pos, uint32_t y_pos, uint32_t len,
                   uint32_t color);
void dma2d_copy_buffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y,
                       uint16_t xsize, uint16_t ysize);
void dma2d_copy_ycbcr_to_rgb(uint32_t *pSrc, uint32_t *pDst, uint16_t xsize,
                             uint16_t ysize, uint32_t ChromaSampling);
void lcd_init(uint32_t lcd_width, uint32_t lcd_height, uint32_t pixel_format);
void lcd_para_init(uint32_t lcd_width, uint32_t lcd_height,
                   uint32_t pixel_format);
void lcd_pwm_init(void);

#ifdef __cplusplus
}
#endif

#endif
