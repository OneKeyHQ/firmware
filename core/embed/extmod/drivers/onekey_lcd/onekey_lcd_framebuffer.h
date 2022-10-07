#ifndef ONEKEY_LCD_FRAMEBUFFER_H
#define ONEKEY_LCD_FRAMEBUFFER_H

#include "onekey_lcd_common.h"
#include "onekey_lcd_dma2d.h"

static inline void fb_swap(
    ONEKEY_LCD_PARAMS *lcd_params)
{
    switch ( lcd_params->fb.current )
    {
    case DISPLAY_MEMORY_FB_A:
        lcd_params->fb.current = DISPLAY_MEMORY_FB_B;
        break;

    case DISPLAY_MEMORY_FB_B:
        lcd_params->fb.current = DISPLAY_MEMORY_FB_A;
        break;

    default:
        lcd_params->fb.current = DISPLAY_MEMORY_FB_A;
        break;
    }
}

void fb_read_pixel(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t x_pos, uint32_t y_pos,
    uint32_t *color)
{
    if ( lcd_params->pixel_format.ltdc == LTDC_PIXEL_FORMAT_ARGB8888 )
    {
        /* Read data value from SDRAM memory */
        *color = *(uint32_t *)(lcd_params->fb.current + (lcd_params->bpp * (lcd_params->res.x * y_pos + x_pos)));
    }
    else // lcd_params->pixel_format.onekey == ONEKEY_PIXEL_FORMAT_RGB565
    {
        *color = *(uint16_t *)(lcd_params->fb.current + (lcd_params->bpp * (lcd_params->res.x * y_pos + x_pos)));
    }
}

void _fb_write_pixel(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t x_pos, uint32_t y_pos,
    uint32_t color)
{
    if ( lcd_params->pixel_format.onekey == ONEKEY_PIXEL_FORMAT_ARGB8888 )
    {
        *(uint32_t *)(lcd_params->fb.current + (lcd_params->bpp * (lcd_params->res.x * y_pos + x_pos))) = color;
    }
    else // lcd_params->pixel_format.onekey == ONEKEY_PIXEL_FORMAT_RGB565
    {
        *(uint16_t *)(lcd_params->fb.current + (lcd_params->bpp * (lcd_params->res.x * y_pos + x_pos))) = color;
    }
}

void fb_write_pixel(
    uint32_t x_pos, uint32_t y_pos,
    uint32_t color)
{
    _fb_write_pixel(
        ptr_lcd_params,
        x_pos, y_pos,
        color);
}

static void _fb_fill_buffer(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t *dest,
    uint32_t x_size, uint32_t y_size,
    uint32_t offset,
    uint32_t color)
{
    uint32_t _color = color;
    if ( lcd_params->pixel_format.onekey == ONEKEY_PIXEL_FORMAT_RGB565 )
    {
        _color = CONVERTRGB5652ARGB8888(color);
    }

    onekey_lcd_dma2d_reconfig(lcd_params, DMA2D_R2M, lcd_params->pixel_format.dma2d_output, offset);

    if ( HAL_DMA2D_Start(lcd_params->handles.dma2d, _color, (uint32_t)dest, x_size, y_size) == HAL_OK )
    {
        HAL_DMA2D_PollForTransfer(lcd_params->handles.dma2d, 25);
    }
}

void fb_fill_buffer(
    uint32_t *dest,
    uint32_t x_size, uint32_t y_size,
    uint32_t offset,
    uint32_t color)
{
    _fb_fill_buffer(
        ptr_lcd_params,
        dest,
        x_size, y_size,
        offset,
        color);
}

void _fb_fill_rect(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t x_pos, uint32_t y_pos,
    uint32_t width, uint32_t height,
    uint32_t color)
{
    /* Get the rectangle start address */
    uint32_t address = lcd_params->fb.current + ((lcd_params->bpp) * (lcd_params->res.x * y_pos + x_pos));

    /* Fill the rectangle */
    fb_fill_buffer((uint32_t *)address, width, height, (lcd_params->res.x - width), color);
}

void fb_fill_rect(
    uint32_t x_pos, uint32_t y_pos,
    uint32_t width, uint32_t height,
    uint32_t color)
{
    _fb_fill_rect(
        ptr_lcd_params,
        x_pos, y_pos,
        width, height,
        color);
}

void _fb_draw_hline(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t x_pos, uint32_t y_pos,
    uint32_t len,
    uint32_t color)
{
    uint32_t address = lcd_params->fb.current + ((lcd_params->bpp) * (lcd_params->res.x * y_pos + x_pos));
    fb_fill_buffer((uint32_t *)address, len, 1, 0, color);
}
void fb_draw_hline(
    uint32_t x_pos, uint32_t y_pos,
    uint32_t len,
    uint32_t color)
{
    _fb_draw_hline(
        ptr_lcd_params,
        x_pos, y_pos,
        len,
        color);
}

void _fb_draw_vline(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t x_pos, uint32_t y_pos,
    uint32_t len,
    uint32_t color)
{
    uint32_t address = lcd_params->fb.current + ((lcd_params->bpp) * (lcd_params->res.x * y_pos + x_pos));
    fb_fill_buffer((uint32_t *)address, 1, len, lcd_params->res.x - 1, color);
}

void fb_draw_vline(
    uint32_t x_pos, uint32_t y_pos,
    uint32_t len,
    uint32_t color)
{
    _fb_draw_vline(
        ptr_lcd_params,
        x_pos, y_pos,
        len,
        color);
}

#endif // ONEKEY_LCD_FRAMEBUFFER_H