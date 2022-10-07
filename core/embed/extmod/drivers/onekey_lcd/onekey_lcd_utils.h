#ifndef ONEKEY_LCD_UTILS_H
#define ONEKEY_LCD_UTILS_H

#include "onekey_lcd_common.h"
#include "onekey_lcd_st7701s.h"

#define ONEKEY_PIXEL_FORMAT_ARGB8888 0x00000000U /*!< ARGB8888 LTDC pixel format */
#define ONEKEY_PIXEL_FORMAT_RGB888 0x00000001U   /*!< RGB888 LTDC pixel format   */
#define ONEKEY_PIXEL_FORMAT_RGB565 0x00000002U   /*!< RGB565 LTDC pixel format   */

#define CONVERTRGB5652ARGB8888(Color)                                     \
    ((((((((Color) >> (11U)) & 0x1FU) * 527U) + 23U) >> (6U)) << (16U)) | \
     (((((((Color) >> (5U)) & 0x3FU) * 259U) + 33U) >> (6U)) << (8U)) |   \
     (((((Color)&0x1FU) * 527U) + 23U) >> (6U)) | (0xFF000000U))

void color_format_config(ONEKEY_LCD_PARAMS *lcd_params, uint32_t onekey_color_format)
{
    lcd_params->pixel_format.onekey = onekey_color_format;

    switch ( lcd_params->pixel_format.onekey )
    {
    case ONEKEY_PIXEL_FORMAT_ARGB8888:
        lcd_params->pixel_format.ltdc = LTDC_PIXEL_FORMAT_ARGB8888;
        lcd_params->pixel_format.dsi = DSI_RGB888;
        lcd_params->pixel_format.dma2d_input = DMA2D_INPUT_ARGB8888;
        lcd_params->pixel_format.dma2d_output = DMA2D_OUTPUT_ARGB8888;
        lcd_params->pixel_format.lcd_cog = ST7701S_FORMAT_RGB888;
        lcd_params->bpp = 4;
        break;
    case ONEKEY_PIXEL_FORMAT_RGB888:
        lcd_params->pixel_format.ltdc = LTDC_PIXEL_FORMAT_RGB888;
        lcd_params->pixel_format.dsi = DSI_RGB888;
        lcd_params->pixel_format.dma2d_input = DMA2D_INPUT_RGB888;
        lcd_params->pixel_format.dma2d_output = DMA2D_OUTPUT_RGB888;
        lcd_params->pixel_format.lcd_cog = ST7701S_FORMAT_RGB888;
        lcd_params->bpp = 4;
        break;
    case ONEKEY_PIXEL_FORMAT_RGB565:
        lcd_params->pixel_format.ltdc = LTDC_PIXEL_FORMAT_RGB565;
        lcd_params->pixel_format.dsi = DSI_RGB565;
        lcd_params->pixel_format.dma2d_input = DMA2D_INPUT_RGB565;
        lcd_params->pixel_format.dma2d_output = DMA2D_OUTPUT_RGB565;
        lcd_params->pixel_format.lcd_cog = ST7701S_FORMAT_RGB565;
        lcd_params->bpp = 2;
        break;
    default:
        lcd_params->pixel_format.ltdc = LTDC_PIXEL_FORMAT_RGB565;
        lcd_params->pixel_format.dsi = DSI_RGB565;
        lcd_params->pixel_format.dma2d_input = DMA2D_INPUT_RGB565;
        lcd_params->pixel_format.dma2d_output = DMA2D_OUTPUT_RGB565;
        lcd_params->pixel_format.lcd_cog = ST7701S_FORMAT_RGB565;
        lcd_params->bpp = 2;
        break;
    }
}

#endif // ONEKEY_LCD_UTILS_H