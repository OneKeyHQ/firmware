#ifndef ONEKEY_LCD_LTDC_H
#define ONEKEY_LCD_LTDC_H

#include "onekey_lcd_common.h"

// NOTE: According to DS, pixel is for LTDC input, LTDC alway use ARGB8888 internally

HAL_StatusTypeDef onekey_lcd_ltdc_init(
    LTDC_TypeDef *instance,
    ONEKEY_LCD_PARAMS *lcd_params)
{
    // MSP
    /** Enable the LTDC clock */
    __HAL_RCC_LTDC_CLK_ENABLE();
    /** Toggle Sw reset of LTDC IP */
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();

    // RCC
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInitStruct;
    RCC_PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    RCC_PeriphClkInitStruct.PLL3.PLL3M = 5U;
    RCC_PeriphClkInitStruct.PLL3.PLL3N = 132U;
    RCC_PeriphClkInitStruct.PLL3.PLL3P = 2U;
    RCC_PeriphClkInitStruct.PLL3.PLL3Q = 2U;
    RCC_PeriphClkInitStruct.PLL3.PLL3R = 24U;
    RCC_PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLLCFGR_PLL3RGE_2;
    RCC_PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    RCC_PeriphClkInitStruct.PLL3.PLL3FRACN = 0U;
    if ( HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInitStruct) != HAL_OK )
        return HAL_ERROR;

    // INIT
    lcd_params->handles.ltdc->Instance = instance;

    lcd_params->handles.ltdc->Init.HSPolarity = LTDC_HSPOLARITY_AL;
    lcd_params->handles.ltdc->Init.VSPolarity = LTDC_VSPOLARITY_AL;
    lcd_params->handles.ltdc->Init.DEPolarity = LTDC_DEPOLARITY_AL;
    lcd_params->handles.ltdc->Init.PCPolarity = LTDC_PCPOLARITY_IPC;

    lcd_params->handles.ltdc->Init.HorizontalSync = lcd_params->timing.hsync - 1;
    lcd_params->handles.ltdc->Init.AccumulatedHBP = lcd_params->timing.hsync + lcd_params->timing.hbp - 1;
    lcd_params->handles.ltdc->Init.AccumulatedActiveW = lcd_params->timing.hsync + lcd_params->res.x + lcd_params->timing.hbp - 1;
    lcd_params->handles.ltdc->Init.TotalWidth = lcd_params->timing.hsync + lcd_params->res.x + lcd_params->timing.hbp + lcd_params->timing.hfp - 1;

    lcd_params->handles.ltdc->Init.VerticalSync = lcd_params->timing.vsync - 1;
    lcd_params->handles.ltdc->Init.AccumulatedVBP = lcd_params->timing.vsync + lcd_params->timing.vbp - 1;
    lcd_params->handles.ltdc->Init.AccumulatedActiveH = lcd_params->timing.vsync + lcd_params->res.y + lcd_params->timing.vbp - 1;
    lcd_params->handles.ltdc->Init.TotalHeigh = lcd_params->timing.vsync + lcd_params->res.y + lcd_params->timing.vbp + lcd_params->timing.vfp - 1;

    lcd_params->handles.ltdc->Init.Backcolor.Blue = 0x00;
    lcd_params->handles.ltdc->Init.Backcolor.Green = 0x00;
    lcd_params->handles.ltdc->Init.Backcolor.Red = 0x00;

    if ( HAL_LTDC_Init(lcd_params->handles.ltdc) != HAL_OK )
        return HAL_ERROR;

    // DONE
    return HAL_OK;
}

// change the ONEKEY_LCD_PARAMS then call this function
HAL_StatusTypeDef onekey_lcd_ltdc_layer_init(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t ltdc_layer)
{
    // LAYER

    if ( !IS_LTDC_LAYER(ltdc_layer) )
        return HAL_ERROR;

    HAL_StatusTypeDef result;
    LTDC_LayerCfgTypeDef LTDC_LayerCfgStruct;

    switch ( ltdc_layer )
    {
    case LTDC_LAYER_1:

        LTDC_LayerCfgStruct.WindowX0 = lcd_params->display_layer.one.x0;
        LTDC_LayerCfgStruct.WindowX1 = lcd_params->display_layer.one.x1;
        LTDC_LayerCfgStruct.WindowY0 = lcd_params->display_layer.one.y0;
        LTDC_LayerCfgStruct.WindowY1 = lcd_params->display_layer.one.y1;
        LTDC_LayerCfgStruct.PixelFormat = lcd_params->display_layer.one.pixel_format;
        LTDC_LayerCfgStruct.Alpha = 255;
        LTDC_LayerCfgStruct.Alpha0 = 0;
        LTDC_LayerCfgStruct.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
        LTDC_LayerCfgStruct.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
        LTDC_LayerCfgStruct.FBStartAdress = lcd_params->display_layer.one.address;
        LTDC_LayerCfgStruct.ImageWidth = (lcd_params->display_layer.one.x1 - lcd_params->display_layer.one.x0);
        LTDC_LayerCfgStruct.ImageHeight = (lcd_params->display_layer.one.y1 - lcd_params->display_layer.one.y0);
        LTDC_LayerCfgStruct.Backcolor.Blue = 0;
        LTDC_LayerCfgStruct.Backcolor.Green = 0;
        LTDC_LayerCfgStruct.Backcolor.Red = 0;
        result = HAL_LTDC_ConfigLayer(lcd_params->handles.ltdc, &LTDC_LayerCfgStruct, LTDC_LAYER_1);
        break;

    case LTDC_LAYER_2:
        LTDC_LayerCfgStruct.WindowX0 = lcd_params->display_layer.two.x0;
        LTDC_LayerCfgStruct.WindowX1 = lcd_params->display_layer.two.x1;
        LTDC_LayerCfgStruct.WindowY0 = lcd_params->display_layer.two.y0;
        LTDC_LayerCfgStruct.WindowY1 = lcd_params->display_layer.two.y1;
        LTDC_LayerCfgStruct.PixelFormat = lcd_params->display_layer.two.pixel_format;
        LTDC_LayerCfgStruct.Alpha = 255;
        LTDC_LayerCfgStruct.Alpha0 = 0;
        LTDC_LayerCfgStruct.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
        LTDC_LayerCfgStruct.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
        LTDC_LayerCfgStruct.FBStartAdress = lcd_params->display_layer.two.address;
        LTDC_LayerCfgStruct.ImageWidth = (lcd_params->display_layer.two.x1 - lcd_params->display_layer.two.x0);
        LTDC_LayerCfgStruct.ImageHeight = (lcd_params->display_layer.two.y1 - lcd_params->display_layer.two.y0);
        LTDC_LayerCfgStruct.Backcolor.Blue = 0;
        LTDC_LayerCfgStruct.Backcolor.Green = 0;
        LTDC_LayerCfgStruct.Backcolor.Red = 0;
        result = HAL_LTDC_ConfigLayer(lcd_params->handles.ltdc, &LTDC_LayerCfgStruct, LTDC_LAYER_2);
        break;

    default:
        result = HAL_ERROR;
    }

    // DONE
    return result;
}

static inline void onekey_lcd_ltdc_layer_fast_swap_buffer(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t ltdc_layer,
    uint32_t buffer_address)
{
    if ( !IS_LTDC_LAYER(ltdc_layer) )
        return;

    __HAL_LTDC_LAYER(lcd_params->handles.ltdc, ltdc_layer)->CFBAR = 0xC0000000;
    __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(lcd_params->handles.ltdc);
}

#endif // ONEKEY_LCD_LTDC_H