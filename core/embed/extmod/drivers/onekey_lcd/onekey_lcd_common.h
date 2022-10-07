#ifndef ONEKEY_LCD_COMMON_H
#define ONEKEY_LCD_COMMON_H

#include STM32_HAL_H

typedef struct // onekey_lcd_params
{
    struct // handles
    {
        LTDC_HandleTypeDef *ltdc;
        DSI_HandleTypeDef *dsi;
        DMA2D_HandleTypeDef *dma2d;
    } handles;

    struct // res
    {
        uint32_t x;
        uint32_t y;
    } res;

    uint32_t xres;
    uint32_t yres;

    uint32_t bpp;

    struct // framebuffer
    {
        uint32_t a;
        uint32_t b;
        uint32_t current;
    } fb;

    struct // pixel_format
    {
        uint32_t ltdc;
        uint32_t dsi;
        uint32_t dma2d_input;
        uint32_t dma2d_output;
        uint32_t lcd_cog; // COG: Controller On Glass (the built-in tft controller e.g. st7701)

        uint32_t onekey; // used to set other pixel_formats
    } pixel_format;

    struct // gpio
    {
        uint16_t bl_pin;
        GPIO_TypeDef *bl_port;

        uint16_t reset_pin;
        GPIO_TypeDef *reset_port;

        uint16_t te_pin;
        GPIO_TypeDef *te_port;
    } gpio;

    struct // timing
    {
        int hsync;
        int hbp;
        int hfp;
        int vsync;
        int vbp;
        int vfp;
    } timing;

    struct // display_layer
    {
        struct // one
        {
            uint32_t x0;
            uint32_t x1;
            uint32_t y0;
            uint32_t y1;
            uint32_t pixel_format;
            uint32_t address;
        } one;

        struct // two
        {
            uint32_t x0;
            uint32_t x1;
            uint32_t y0;
            uint32_t y1;
            uint32_t pixel_format;
            uint32_t address;
        } two;

    } display_layer;

} ONEKEY_LCD_PARAMS;

// bit ugly solution, but leave it for now
extern ONEKEY_LCD_PARAMS *ptr_lcd_params;

// template
// HAL_StatusTypeDef onekey_lcd_xxx_init(
//     XXX_TypeDef *instance,
//     XXX_HandleTypeDef *hxxx)
// {
//     // MSP

//     // RCC

//     // INIT
//     hxxx->Instance = instance;
//     if ( HAL_XXX_Init(xxx) != HAL_OK )
//         return HAL_ERROR;

//     // DONE
//     return HAL_OK;
// }

#endif // ONEKEY_LCD_COMMON_H