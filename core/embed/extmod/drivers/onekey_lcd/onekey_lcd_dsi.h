#ifndef ONEKEY_LCD_DSI_H
#define ONEKEY_LCD_DSI_H

#include "onekey_lcd_common.h"

HAL_StatusTypeDef onekey_lcd_dsi_init(
    DSI_TypeDef *instance,
    ONEKEY_LCD_PARAMS *lcd_params)
{
    // MSP
    /** Enable DSI Host and wravoidpper clocks */
    __HAL_RCC_DSI_CLK_ENABLE();
    /** Soft Reset the DSI Host and wrapper */
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_DSI_RELEASE_RESET();

    // RCC AND INIT
    DSI_PLLInitTypeDef PLLInit;
    PLLInit.PLLNDIV = 100;
    PLLInit.PLLIDF = DSI_PLL_IN_DIV5;
    PLLInit.PLLODF = DSI_PLL_OUT_DIV1;

    lcd_params->handles.dsi->Instance = instance;
    lcd_params->handles.dsi->Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;
    lcd_params->handles.dsi->Init.TXEscapeCkdiv = 4;
    lcd_params->handles.dsi->Init.NumberOfLanes = DSI_TWO_DATA_LANES;

    if ( HAL_DSI_Init(lcd_params->handles.dsi, &PLLInit) != HAL_OK )
    {
        return HAL_ERROR;
    }

    // VIDEO CONFIG

    /* Timing parameters for all Video modes */
    /*
    The lane byte clock is set 62500 Khz
    The pixel clock is set to 27429 Khz
    */
    DSI_VidCfgTypeDef VidCfg;
    VidCfg.VirtualChannelID = 0;
    VidCfg.ColorCoding = lcd_params->pixel_format.dsi;
    VidCfg.LooselyPacked = DSI_LOOSELY_PACKED_DISABLE;
    VidCfg.Mode = DSI_VID_MODE_BURST;
    VidCfg.PacketSize = lcd_params->res.x;
    VidCfg.NumberOfChunks = 0;
    VidCfg.NullPacketSize = 0xFFFU;
    VidCfg.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
    VidCfg.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
    VidCfg.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
    VidCfg.HorizontalSyncActive =
        (lcd_params->timing.hsync * 62500U) / 27429U;
    VidCfg.HorizontalBackPorch =
        (lcd_params->timing.hbp * 62500U) / 27429U;
    VidCfg.HorizontalLine =
        ((lcd_params->res.x + lcd_params->timing.hsync + lcd_params->timing.hbp + lcd_params->timing.hfp) * 62500U) / 27429U;
    VidCfg.VerticalSyncActive = lcd_params->timing.vsync;
    VidCfg.VerticalBackPorch = lcd_params->timing.vbp;
    VidCfg.VerticalFrontPorch = lcd_params->timing.vfp;
    VidCfg.VerticalActive = lcd_params->res.y;
    VidCfg.LPCommandEnable = DSI_LP_COMMAND_DISABLE;
    VidCfg.LPLargestPacketSize = 0;
    VidCfg.LPVACTLargestPacketSize = 0;

    VidCfg.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;
    VidCfg.LPHorizontalBackPorchEnable = DSI_LP_HBP_ENABLE;
    VidCfg.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;
    VidCfg.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;
    VidCfg.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;
    VidCfg.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE;
    VidCfg.FrameBTAAcknowledgeEnable = DSI_FBTAA_DISABLE;

    if ( HAL_DSI_ConfigVideoMode(lcd_params->handles.dsi, &VidCfg) != HAL_OK )
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef onekey_lcd_dsi_write(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t cmd,
    uint8_t *bytes_write, uint32_t bytes_len)
{
    if ( bytes_len <= 1 )
        return HAL_DSI_ShortWrite(lcd_params->handles.dsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, cmd, (uint32_t)bytes_write[0]);
    else
        return HAL_DSI_LongWrite(lcd_params->handles.dsi, 0, DSI_DCS_LONG_PKT_WRITE, bytes_len, cmd, bytes_write);
}

// NOTE: this function do not handel dsi parmaters
HAL_StatusTypeDef onekey_lcd_dsi_read(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t cmd,
    uint8_t *bytes_read, uint32_t bytes_len)
{
    return HAL_DSI_Read(
        lcd_params->handles.dsi, 0,
        bytes_read, bytes_len,
        DSI_GEN_SHORT_PKT_READ_P0,
        cmd, NULL);
}

#endif // ONEKEY_LCD_DSI_H