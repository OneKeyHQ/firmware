#ifndef ONEKEY_LCD_DMA2D_H
#define ONEKEY_LCD_DMA2D_H

#include "onekey_lcd_common.h"

HAL_StatusTypeDef onekey_lcd_dma2d_init(
    DMA2D_TypeDef *instance,
    ONEKEY_LCD_PARAMS *lcd_params)
{
    // MSP
    /** Enable the DMA2D clock */
    __HAL_RCC_DMA2D_CLK_ENABLE();
    /** Toggle Sw reset of DMA2D IP */
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();

    // RCC

    // INIT
    lcd_params->handles.dma2d->Instance = instance;

    lcd_params->handles.dma2d->Init.Mode = DMA2D_M2M;
    lcd_params->handles.dma2d->Init.ColorMode = lcd_params->pixel_format.dma2d_output;
    lcd_params->handles.dma2d->Init.OutputOffset = 0;
    lcd_params->handles.dma2d->Init.AlphaInverted = DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/
    lcd_params->handles.dma2d->Init.RedBlueSwap = DMA2D_RB_REGULAR;      /* No Output Red & Blue swap */

    // DMA2D Callbacks
    // lcd_params->handles.dma2d->XferCpltCallback = NULL;

    if ( HAL_DMA2D_Init(lcd_params->handles.dma2d) == HAL_OK )
        return HAL_ERROR;

    // LAYER

    // DONE
    return HAL_OK;
}

// change the ONEKEY_LCD_PARAMS then call this function
HAL_StatusTypeDef onekey_lcd_dma2d_layer_init(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t dma2d_layer)
{
    // LAYER

    if ( !IS_DMA2D_LAYER(dma2d_layer) )
        return HAL_ERROR;

    HAL_StatusTypeDef result;

    switch ( dma2d_layer )
    {
    case DMA2D_FOREGROUND_LAYER:
        result = HAL_DMA2D_ConfigLayer(lcd_params->handles.dma2d, DMA2D_FOREGROUND_LAYER);
        break;

    case DMA2D_BACKGROUND_LAYER:
        result = HAL_DMA2D_ConfigLayer(lcd_params->handles.dma2d, DMA2D_BACKGROUND_LAYER);
        break;

    default:
        result = HAL_ERROR;
    }

    // DONE
    return result;
}

static inline void onekey_lcd_dma2d_fast_abort(
    ONEKEY_LCD_PARAMS *lcd_params)
{
    // abort
    MODIFY_REG(lcd_params->handles.dma2d->Instance->CR, DMA2D_CR_ABORT | DMA2D_CR_START, DMA2D_CR_ABORT);

    // wait for reset status
    while ( (lcd_params->handles.dma2d->Instance->CR & DMA2D_CR_START) != RESET )
        ;

    // disable interrupt
    CLEAR_BIT(lcd_params->handles.dma2d->Instance->CR, (DMA2D_IT_TC | DMA2D_IT_TE | DMA2D_IT_CE));

    // this is just for the HAL layer, not HW related
    lcd_params->handles.dma2d->Lock = HAL_UNLOCKED;
}

// there is no way to simplify more as long as oter people using hal DMA2D functions
// HAL DMA2D functions are taking params from the ->Init struct
// it must be update to date with your config, thus more things to do before display a new frame
static inline void onekey_lcd_dma2d_reconfig(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t mode, uint32_t dma2d_output_color, uint32_t offset)
{
    if ( !(IS_DMA2D_MODE(mode) && IS_DMA2D_CMODE(dma2d_output_color) && IS_DMA2D_OFFSET(offset)) )
        return;

    // wait for current transfer done
    while ( READ_BIT(lcd_params->handles.dma2d->Instance->CR, DMA2D_CR_START) )
        ;
    // or just kill it
    // onekey_lcd_dma2d_fast_abort(lcd_params);

    //this is just for the HAL layer, not HW related
    ptr_lcd_params->handles.dma2d->Init.Mode = mode;
    ptr_lcd_params->handles.dma2d->Init.ColorMode = dma2d_output_color;
    lcd_params->handles.dma2d->State = HAL_DMA2D_STATE_BUSY;

    MODIFY_REG(
        lcd_params->handles.dma2d->Instance->CR,
        DMA2D_CR_MODE | DMA2D_CR_LOM,
        mode | lcd_params->handles.dma2d->Init.LineOffsetMode);
    MODIFY_REG(
        lcd_params->handles.dma2d->Instance->OPFCCR,
        DMA2D_OPFCCR_CM | DMA2D_OPFCCR_SB,
        dma2d_output_color | lcd_params->handles.dma2d->Init.BytesSwap);
    MODIFY_REG(
        lcd_params->handles.dma2d->Instance->OOR,
        DMA2D_OOR_LO,
        offset);
    MODIFY_REG(
        lcd_params->handles.dma2d->Instance->OPFCCR,
        (DMA2D_OPFCCR_AI | DMA2D_OPFCCR_RBS),
        ((lcd_params->handles.dma2d->Init.AlphaInverted << DMA2D_OPFCCR_AI_Pos) | (lcd_params->handles.dma2d->Init.RedBlueSwap << DMA2D_OPFCCR_RBS_Pos)));

    //this is just for the HAL layer, not HW related
    lcd_params->handles.dma2d->State = HAL_DMA2D_STATE_READY;
}

void _onekey_lcd_dma2d_copy_buffer(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t *pSrc, uint32_t *pDst,
    uint16_t x, uint16_t y,
    uint16_t xsize, uint16_t ysize)
{
    onekey_lcd_dma2d_reconfig(lcd_params, DMA2D_M2M, lcd_params->pixel_format.dma2d_output, 0);

    uint32_t destination = (uint32_t)pDst + (y * lcd_params->res.x + x) * (lcd_params->bpp);
    uint32_t source = (uint32_t)pSrc;

    if ( HAL_DMA2D_Start(lcd_params->handles.dma2d, source, destination, xsize, ysize) == HAL_OK )
    {
        HAL_DMA2D_PollForTransfer(lcd_params->handles.dma2d, HAL_MAX_DELAY);
    }
}

void onekey_lcd_dma2d_copy_buffer(
    uint32_t *pSrc, uint32_t *pDst,
    uint16_t x, uint16_t y,
    uint16_t xsize, uint16_t ysize)
{
    _onekey_lcd_dma2d_copy_buffer(
        ptr_lcd_params,
        pSrc, pDst,
        x, y,
        xsize, ysize);
}

void dma2d_copy_buffer(
    uint32_t *pSrc, uint32_t *pDst,
    uint16_t x, uint16_t y,
    uint16_t xsize, uint16_t ysize)
{
    UNUSED(pDst);

    _onekey_lcd_dma2d_copy_buffer(
        ptr_lcd_params,
        pSrc, (uint32_t *)ptr_lcd_params->fb.current,
        x, y,
        xsize, ysize);
}

#endif // ONEKEY_LCD_DMA2D_H