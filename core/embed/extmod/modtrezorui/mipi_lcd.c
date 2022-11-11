#include STM32_HAL_H

#include "mipi_lcd.h"

LCD_PARAMS lcd_params;
LCD_TIME_SEQUENCE lcd_time_seq = {.hsync = ST7701S_480X800_HSYNC,
                                  .hfp = ST7701S_480X800_HFP,
                                  .hbp = ST7701S_480X800_HBP,
                                  .vsync = ST7701S_480X800_VSYNC,
                                  .vfp = ST7701S_480X800_VFP,
                                  .vbp = ST7701S_480X800_VBP};

DSI_HandleTypeDef hlcd_dsi;
DMA2D_HandleTypeDef hlcd_dma2d;
LTDC_HandleTypeDef hlcd_ltdc;

static void ltcd_msp_init(LTDC_HandleTypeDef *hltdc) {
  if (hltdc->Instance == LTDC) {
    /** Enable the LTDC clock */
    __HAL_RCC_LTDC_CLK_ENABLE();

    /** Toggle Sw reset of LTDC IP */
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();
  }
}

static void dma2d_msp_init(DMA2D_HandleTypeDef *hdma2d) {
  if (hdma2d->Instance == DMA2D) {
    /** Enable the DMA2D clock */
    __HAL_RCC_DMA2D_CLK_ENABLE();

    /** Toggle Sw reset of DMA2D IP */
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();
  }
}

static void dsi_msp_init(DSI_HandleTypeDef *hdsi) {
  if (hdsi->Instance == DSI) {
    /** Enable DSI Host and wrapper clocks */
    __HAL_RCC_DSI_CLK_ENABLE();

    /** Soft Reset the DSI Host and wrapper */
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_DSI_RELEASE_RESET();
  }
}

#define DSI_FREQ 30000U
#define LTDC_FREQ 27500U

HAL_StatusTypeDef dsi_host_init(DSI_HandleTypeDef *hdsi, uint32_t Width,
                                uint32_t Height, uint32_t PixelFormat) {
  DSI_PLLInitTypeDef PLLInit;
  DSI_VidCfgTypeDef VidCfg;

  hdsi->Instance = DSI;
  hdsi->Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;
  hdsi->Init.TXEscapeCkdiv = 4;
  hdsi->Init.NumberOfLanes = DSI_TWO_DATA_LANES;
  PLLInit.PLLNDIV = 96;
  PLLInit.PLLIDF = DSI_PLL_IN_DIV5;
  PLLInit.PLLODF = DSI_PLL_OUT_DIV2;
  if (HAL_DSI_Init(hdsi, &PLLInit) != HAL_OK) {
    return HAL_ERROR;
  }

  /* Timing parameters for all Video modes */
  /*
  The lane byte clock is set 62500 Khz
  The pixel clock is set to 27429 Khz
  */
  VidCfg.VirtualChannelID = 0;
  VidCfg.ColorCoding = PixelFormat;
  VidCfg.LooselyPacked = DSI_LOOSELY_PACKED_DISABLE;
  VidCfg.Mode = DSI_VID_MODE_BURST;
  VidCfg.PacketSize = Width;
  VidCfg.NumberOfChunks = 0;
  VidCfg.NullPacketSize = 0xFFFU;
  VidCfg.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
  VidCfg.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
  VidCfg.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
  VidCfg.HorizontalSyncActive = (lcd_time_seq.hsync * DSI_FREQ) / LTDC_FREQ;
  VidCfg.HorizontalBackPorch = (lcd_time_seq.hbp * DSI_FREQ) / LTDC_FREQ;
  VidCfg.HorizontalLine =
      ((Width + lcd_time_seq.hsync + lcd_time_seq.hbp + lcd_time_seq.hfp) *
       DSI_FREQ) /
      LTDC_FREQ;
  VidCfg.VerticalSyncActive = lcd_time_seq.vsync;
  VidCfg.VerticalBackPorch = lcd_time_seq.vbp;
  VidCfg.VerticalFrontPorch = lcd_time_seq.vfp;
  VidCfg.VerticalActive = Height;
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

  if (HAL_DSI_ConfigVideoMode(hdsi, &VidCfg) != HAL_OK) {
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef ltdc_clock_config(LTDC_HandleTypeDef *hltdc) {
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLL3.PLL3M = 5U;
  PeriphClkInitStruct.PLL3.PLL3N = 132U;
  PeriphClkInitStruct.PLL3.PLL3P = 2U;
  PeriphClkInitStruct.PLL3.PLL3Q = 2U;
  PeriphClkInitStruct.PLL3.PLL3R = 24U;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLLCFGR_PLL3RGE_2;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0U;
  return HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}

HAL_StatusTypeDef ltdc_init(LTDC_HandleTypeDef *hltdc, uint32_t Width,
                            uint32_t Height) {
  hltdc->Instance = LTDC;
  hltdc->Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc->Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc->Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc->Init.PCPolarity = LTDC_PCPOLARITY_IPC;

  hltdc->Init.HorizontalSync = lcd_time_seq.hsync - 1;
  hltdc->Init.AccumulatedHBP = lcd_time_seq.hsync + lcd_time_seq.hbp - 1;
  hltdc->Init.AccumulatedActiveW =
      lcd_time_seq.hsync + Width + lcd_time_seq.hbp - 1;
  hltdc->Init.TotalWidth =
      lcd_time_seq.hsync + Width + lcd_time_seq.hbp + lcd_time_seq.hfp - 1;
  hltdc->Init.VerticalSync = lcd_time_seq.vsync - 1;
  hltdc->Init.AccumulatedVBP = lcd_time_seq.vsync + lcd_time_seq.vbp - 1;
  hltdc->Init.AccumulatedActiveH =
      lcd_time_seq.vsync + Height + lcd_time_seq.vbp - 1;
  hltdc->Init.TotalHeigh =
      lcd_time_seq.vsync + Height + lcd_time_seq.vbp + lcd_time_seq.vfp - 1;

  hltdc->Init.Backcolor.Blue = 0x00;
  hltdc->Init.Backcolor.Green = 0x00;
  hltdc->Init.Backcolor.Red = 0x00;

  return HAL_LTDC_Init(hltdc);
}

HAL_StatusTypeDef ltdc_layer_config(LTDC_HandleTypeDef *hltdc,
                                    uint32_t layer_index,
                                    LTDC_LAYERCONFIG *config) {
  LTDC_LayerCfgTypeDef pLayerCfg;

  pLayerCfg.WindowX0 = config->x0;
  pLayerCfg.WindowX1 = config->x1;
  pLayerCfg.WindowY0 = config->y0;
  pLayerCfg.WindowY1 = config->y1;
  pLayerCfg.PixelFormat = config->pixel_format;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = config->address;
  pLayerCfg.ImageWidth = (config->x1 - config->x0);
  pLayerCfg.ImageHeight = (config->y1 - config->y0);
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  return HAL_LTDC_ConfigLayer(hltdc, &pLayerCfg, layer_index);
}

int32_t bsp_get_tick(void) { return (int32_t)HAL_GetTick(); }

#define CONVERTRGB5652ARGB8888(Color)                                   \
  ((((((((Color) >> (11U)) & 0x1FU) * 527U) + 23U) >> (6U)) << (16U)) | \
   (((((((Color) >> (5U)) & 0x3FU) * 259U) + 33U) >> (6U)) << (8U)) |   \
   (((((Color)&0x1FU) * 527U) + 23U) >> (6U)) | (0xFF000000U))

void fb_read_pixel(uint32_t x_pos, uint32_t y_pos, uint32_t *color) {
  if (lcd_params.pixel_format == LTDC_PIXEL_FORMAT_ARGB8888) {
    /* Read data value from SDRAM memory */
    *color =
        *(uint32_t *)(lcd_params.fb_base +
                      (lcd_params.bbp * (lcd_params.xres * y_pos + x_pos)));

  } else {
    /*LTDC_PIXEL_FORMAT_RGB565 */
    *color =
        *(uint16_t *)(lcd_params.fb_base +
                      (lcd_params.bbp * (lcd_params.xres * y_pos + x_pos)));
  }
}

void fb_write_pixel(uint32_t x_pos, uint32_t y_pos, uint32_t color) {
  if (lcd_params.pixel_format == LTDC_PIXEL_FORMAT_ARGB8888) {
    *(uint32_t *)(lcd_params.fb_base +
                  (lcd_params.bbp * (lcd_params.xres * y_pos + x_pos))) = color;

  } else {
    /*LTDC_PIXEL_FORMAT_RGB565 */
    *(uint16_t *)(lcd_params.fb_base +
                  (lcd_params.bbp * (lcd_params.xres * y_pos + x_pos))) = color;
  }
}

static void fb_fill_buffer(uint32_t *dest, uint32_t x_size, uint32_t y_size,
                           uint32_t offset, uint32_t color) {
  uint32_t output_color_mode, input_color = color;

  switch (lcd_params.pixel_format) {
    case LTDC_PIXEL_FORMAT_RGB565:
      output_color_mode = DMA2D_OUTPUT_RGB565; /* RGB565 */
      input_color = CONVERTRGB5652ARGB8888(color);
      break;
    case LTDC_PIXEL_FORMAT_RGB888:
    default:
      output_color_mode = DMA2D_OUTPUT_ARGB8888; /* ARGB8888 */
      break;
  }

  /* Register to memory mode with ARGB8888 as color Mode */
  hlcd_dma2d.Init.Mode = DMA2D_R2M;
  hlcd_dma2d.Init.ColorMode = output_color_mode;
  hlcd_dma2d.Init.OutputOffset = offset;

  hlcd_dma2d.Instance = DMA2D;

  /* DMA2D Initialization */
  if (HAL_DMA2D_Init(&hlcd_dma2d) == HAL_OK) {
    if (HAL_DMA2D_ConfigLayer(&hlcd_dma2d, 1) == HAL_OK) {
      if (HAL_DMA2D_Start(&hlcd_dma2d, input_color, (uint32_t)dest, x_size,
                          y_size) == HAL_OK) {
        /* Polling For DMA transfer */
        (void)HAL_DMA2D_PollForTransfer(&hlcd_dma2d, 25);
      }
    }
  }
}

void fb_fill_rect(uint32_t x_pos, uint32_t y_pos, uint32_t width,
                  uint32_t height, uint32_t color) {
  /* Get the rectangle start address */
  uint32_t address = lcd_params.fb_base +
                     ((lcd_params.bbp) * (lcd_params.xres * y_pos + x_pos));

  /* Fill the rectangle */
  fb_fill_buffer((uint32_t *)address, width, height, (lcd_params.xres - width),
                 color);
}

void fb_draw_hline(uint32_t x_pos, uint32_t y_pos, uint32_t len,
                   uint32_t color) {
  uint32_t address = lcd_params.fb_base +
                     ((lcd_params.bbp) * (lcd_params.xres * y_pos + x_pos));
  fb_fill_buffer((uint32_t *)address, len, 1, 0, color);
}

void fb_draw_vline(uint32_t x_pos, uint32_t y_pos, uint32_t len,
                   uint32_t color) {
  uint32_t address = lcd_params.fb_base +
                     ((lcd_params.bbp) * (lcd_params.xres * y_pos + x_pos));
  fb_fill_buffer((uint32_t *)address, 1, len, lcd_params.xres - 1, color);
}

void dma2d_copy_buffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y,
                       uint16_t xsize, uint16_t ysize) {
  uint32_t destination =
      (uint32_t)pDst + (y * lcd_params.xres + x) * (lcd_params.bbp);
  uint32_t source = (uint32_t)pSrc;

  /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
  hlcd_dma2d.Init.Mode = DMA2D_M2M;
  hlcd_dma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hlcd_dma2d.Init.OutputOffset = lcd_params.xres - xsize;
  hlcd_dma2d.Init.AlphaInverted =
      DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/
  hlcd_dma2d.Init.RedBlueSwap =
      DMA2D_RB_REGULAR; /* No Output Red & Blue swap */

  /*##-2- DMA2D Callbacks Configuration ######################################*/
  hlcd_dma2d.XferCpltCallback = NULL;

  /*##-3- Foreground Configuration ###########################################*/
  hlcd_dma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hlcd_dma2d.LayerCfg[1].InputAlpha = 0xFF;
  hlcd_dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hlcd_dma2d.LayerCfg[1].InputOffset = 0;
  hlcd_dma2d.LayerCfg[1].RedBlueSwap =
      DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
  hlcd_dma2d.LayerCfg[1].AlphaInverted =
      DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */

  hlcd_dma2d.Instance = DMA2D;

  /* DMA2D Initialization */
  if (HAL_DMA2D_Init(&hlcd_dma2d) == HAL_OK) {
    if (HAL_DMA2D_ConfigLayer(&hlcd_dma2d, 1) == HAL_OK) {
      if (HAL_DMA2D_Start(&hlcd_dma2d, source, destination, xsize, ysize) ==
          HAL_OK) {
        /* Polling For DMA transfer */
        HAL_DMA2D_PollForTransfer(&hlcd_dma2d, HAL_MAX_DELAY);
      }
    }
  }
}

void dma2d_copy_ycbcr_to_rgb(uint32_t *pSrc, uint32_t *pDst, uint16_t xsize,
                             uint16_t ysize, uint32_t ChromaSampling) {
  uint32_t cssMode = DMA2D_CSS_420, inputLineOffset = 0;

  if (ChromaSampling == JPEG_420_SUBSAMPLING) {
    cssMode = DMA2D_CSS_420;

    inputLineOffset = xsize % 16;
    if (inputLineOffset != 0) {
      inputLineOffset = 16 - inputLineOffset;
    }
  } else if (ChromaSampling == JPEG_444_SUBSAMPLING) {
    cssMode = DMA2D_NO_CSS;

    inputLineOffset = xsize % 8;
    if (inputLineOffset != 0) {
      inputLineOffset = 8 - inputLineOffset;
    }
  } else if (ChromaSampling == JPEG_422_SUBSAMPLING) {
    cssMode = DMA2D_CSS_422;

    inputLineOffset = xsize % 16;
    if (inputLineOffset != 0) {
      inputLineOffset = 16 - inputLineOffset;
    }
  }

  /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/
  hlcd_dma2d.Init.Mode = DMA2D_M2M_PFC;
  hlcd_dma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hlcd_dma2d.Init.OutputOffset = 0;
  hlcd_dma2d.Init.AlphaInverted =
      DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/
  hlcd_dma2d.Init.RedBlueSwap =
      DMA2D_RB_REGULAR; /* No Output Red & Blue swap */

  /*##-2- DMA2D Callbacks Configuration ######################################*/
  hlcd_dma2d.XferCpltCallback = NULL;

  /*##-3- Foreground Configuration ###########################################*/
  hlcd_dma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
  hlcd_dma2d.LayerCfg[1].InputAlpha = 0xFF;
  hlcd_dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_YCBCR;
  hlcd_dma2d.LayerCfg[1].ChromaSubSampling = cssMode;
  hlcd_dma2d.LayerCfg[1].InputOffset = inputLineOffset;
  hlcd_dma2d.LayerCfg[1].RedBlueSwap =
      DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
  hlcd_dma2d.LayerCfg[1].AlphaInverted =
      DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */

  hlcd_dma2d.Instance = DMA2D;

  /*##-4- DMA2D Initialization     ###########################################*/
  HAL_DMA2D_Init(&hlcd_dma2d);
  HAL_DMA2D_ConfigLayer(&hlcd_dma2d, 1);

  HAL_DMA2D_Start(&hlcd_dma2d, (uint32_t)pSrc, (uint32_t)pDst, xsize, ysize);
  HAL_DMA2D_PollForTransfer(
      &hlcd_dma2d, 25); /* wait for the previous DMA2D transfer to ends */
}

void st7701_dsi_write(uint16_t reg, uint8_t *seq, uint16_t len) {
  if (len <= 1) {
    HAL_DSI_ShortWrite(&hlcd_dsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, reg,
                       (uint32_t)seq[0]);
  } else {
    HAL_DSI_LongWrite(&hlcd_dsi, 0, DSI_DCS_LONG_PKT_WRITE, len, reg, seq);
  }
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define st7701_dsi(reg, seq...)              \
  {                                          \
    uint8_t d[] = {seq};                     \
    st7701_dsi_write(reg, d, ARRAY_SIZE(d)); \
  }

void st7701_init_sequence(void) {
  st7701_dsi(MIPI_DCS_SOFT_RESET, 0x00);

  /* We need to wait 5ms before sending new commands */
  HAL_Delay(120);

  st7701_dsi(MIPI_DCS_EXIT_SLEEP_MODE, 0x00);

  HAL_Delay(120);

  /* Command2, BK1 */
  st7701_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 0x11);
  st7701_dsi(0xD1, 0x11);

  /* Command2, BK0 */
  st7701_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 0x10);
  st7701_dsi(0xC0, 0x63, 0x00);
  st7701_dsi(0xC1, lcd_time_seq.vbp, lcd_time_seq.vfp);
  st7701_dsi(0xC2, 0x31, 0x08);
  st7701_dsi(0xB0, 0x00, 0x11, 0x19, 0x0C, 0x10, 0x06, 0x07, 0x0A, 0x09, 0x22,
             0x04, 0x10, 0x0E, 0x28, 0x30, 0x1C);
  st7701_dsi(0xB1, 0x00, 0x12, 0x19, 0x0D, 0x10, 0x04, 0x06, 0x07, 0x08, 0x23,
             0x04, 0x12, 0x11, 0x28, 0x30, 0x1C);

  /* Command2, BK1 */
  st7701_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 0x11);
  st7701_dsi(0xB0, 0x4D);
  st7701_dsi(0xB1, 0x4A);
  st7701_dsi(0xB2, 0x07);
  st7701_dsi(0xB3, 0x80);
  st7701_dsi(0xB5, 0x47);
  st7701_dsi(0xB7, 0x8A);
  st7701_dsi(0xB8, 0x21);
  st7701_dsi(0xC1, 0x78);
  st7701_dsi(0xC2, 0x78);
  st7701_dsi(0xD0, 0x88);

  HAL_Delay(100);

  st7701_dsi(0xE0, 0x00, 0x00, 0x02);
  st7701_dsi(0xE1, 0x01, 0xA0, 0x03, 0xA0, 0x02, 0xA0, 0x04, 0xA0, 0x00, 0x44,
             0x44);
  st7701_dsi(0xE2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00);
  st7701_dsi(0xE3, 0x00, 0x00, 0x33, 0x33);
  st7701_dsi(0xE4, 0x44, 0x44);
  st7701_dsi(0xE5, 0x01, 0x26, 0xA0, 0xA0, 0x03, 0x28, 0xA0, 0xA0, 0x05, 0x2A,
             0xA0, 0xA0, 0x07, 0x2C, 0xA0, 0xA0);
  st7701_dsi(0xE6, 0x00, 0x00, 0x33, 0x33);
  st7701_dsi(0xE7, 0x44, 0x44);
  st7701_dsi(0xE8, 0x02, 0x26, 0xA0, 0xA0, 0x04, 0x28, 0xA0, 0xA0, 0x06, 0x2A,
             0xA0, 0xA0, 0x08, 0x2C, 0xA0, 0xA0);
  st7701_dsi(0xEB, 0x00, 0x00, 0xE4, 0xE4, 0x44, 0x00, 0x40);
  st7701_dsi(0xED, 0xFF, 0xF7, 0x65, 0x4F, 0x0B, 0xA1, 0xCF, 0xFF, 0xFF, 0xFC,
             0x1A, 0xB0, 0xF4, 0x56, 0x7F, 0xFF);

  /* disable Command2 */
  st7701_dsi(0xFF, 0x77, 0x01, 0x00, 0x00, 00);
  st7701_dsi(MIPI_DCS_SET_TEAR_ON, 0x00);
  HAL_Delay(100);
  st7701_dsi(MIPI_DCS_SET_PIXEL_FORMAT, 0x50);
  st7701_dsi(MIPI_DCS_SET_DISPLAY_BRIGHTNESS, 0xFF);
  st7701_dsi(MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x2C);
  st7701_dsi(MIPI_DCS_WRITE_POWER_SAVE, 0x92);
  st7701_dsi(MIPI_DCS_SET_CABC_MIN_BRIGHTNESS, 0xFF);
  st7701_dsi(MIPI_DCS_SET_DISPLAY_ON, 0x00);
  HAL_Delay(20);
}

#define LED_PWM_TIM_PERIOD (100)

void display_set_backlight(int val) {
  TIM1->CCR1 = (LED_PWM_TIM_PERIOD - 1) * val / 255;
}

void lcd_init(uint32_t lcd_width, uint32_t lcd_height, uint32_t pixel_format) {
  __HAL_RCC_LTDC_FORCE_RESET();
  __HAL_RCC_LTDC_RELEASE_RESET();

  __HAL_RCC_DMA2D_FORCE_RESET();
  __HAL_RCC_DMA2D_RELEASE_RESET();

  // RESET PIN
  GPIO_InitTypeDef gpio_init_structure;

  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* Configure the GPIO Reset pin */
  gpio_init_structure.Pin = LCD_RESET_PIN;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RESET_GPIO_PORT, &gpio_init_structure);

  /* Activate XRES active low */
  HAL_GPIO_WritePin(LCD_RESET_GPIO_PORT, LCD_RESET_PIN, GPIO_PIN_RESET);
  HAL_Delay(20); /* wait 20 ms */
  HAL_GPIO_WritePin(LCD_RESET_GPIO_PORT, LCD_RESET_PIN,
                    GPIO_PIN_SET); /* Deactivate XRES */
  HAL_Delay(10);

  /* LCD_TE_CTRL GPIO configuration */
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  gpio_init_structure.Pin = LCD_TE_PIN;
  gpio_init_structure.Mode = GPIO_MODE_INPUT;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(LCD_TE_GPIO_PORT, &gpio_init_structure);
  /* Assert back-light LCD_BL_CTRL pin */
  HAL_GPIO_WritePin(LCD_TE_GPIO_PORT, LCD_TE_PIN, GPIO_PIN_SET);

  /* Initializes peripherals instance value */
  hlcd_ltdc.Instance = LTDC;
  hlcd_dma2d.Instance = DMA2D;
  hlcd_dsi.Instance = DSI;

  uint32_t ctrl_pixel_format, ltdc_pixel_format, dsi_pixel_format;

  if (pixel_format == LCD_PIXEL_FORMAT_RGB565) {
    ltdc_pixel_format = LTDC_PIXEL_FORMAT_RGB565;
    dsi_pixel_format = DSI_RGB565;
    ctrl_pixel_format = ST7701S_FORMAT_RBG565;
    lcd_params.bbp = 2;

  } else {
    ltdc_pixel_format = LCD_PIXEL_FORMAT_ARGB8888;
    dsi_pixel_format = DSI_RGB888;
    ctrl_pixel_format = ST7701S_FORMAT_RGB888;
    lcd_params.bbp = 4;
  }

  lcd_params.pixel_format = ltdc_pixel_format;
  lcd_params.xres = lcd_width;
  lcd_params.yres = lcd_height;
  lcd_params.fb_base = DISPLAY_MEMORY_BASE;

  ltcd_msp_init(&hlcd_ltdc);

  dma2d_msp_init(&hlcd_dma2d);

  dsi_msp_init(&hlcd_dsi);

  dsi_host_init(&hlcd_dsi, lcd_width, lcd_height, dsi_pixel_format);

  ltdc_clock_config(&hlcd_ltdc);

  ltdc_init(&hlcd_ltdc, lcd_width, lcd_height);

  LTDC_LAYERCONFIG config;

  config.x0 = 0;
  config.x1 = lcd_width;
  config.y0 = 0;
  config.y1 = lcd_height;
  config.pixel_format = ltdc_pixel_format;
  config.address = DISPLAY_MEMORY_BASE;

  ltdc_layer_config(&hlcd_ltdc, 0, &config);

  /* Enable the DSI host and wrapper after the LTDC initialization
        To avoid any synchronization issue, the DSI shall be started after
     enabling the LTDC */
  (void)HAL_DSI_Start(&hlcd_dsi);

  /* Enable the DSI BTW for read operations */
  (void)HAL_DSI_ConfigFlowControl(&hlcd_dsi, DSI_FLOW_CONTROL_BTA);

  (void)ctrl_pixel_format;
  st7701_init_sequence();
}

void lcd_pwm_init(void) {
  GPIO_InitTypeDef gpio_init_structure = {0};
  /* LCD_BL_CTRL GPIO configuration */
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_TIM1_CLK_ENABLE();
  // LCD_PWM/PA7 (backlight control)
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
  gpio_init_structure.Alternate = GPIO_AF1_TIM1;
  gpio_init_structure.Pin = LCD_BL_CTRL_PIN;
  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);

  // enable PWM timer
  TIM_HandleTypeDef TIM1_Handle;
  TIM1_Handle.Instance = TIM1;
  TIM1_Handle.Init.Period = LED_PWM_TIM_PERIOD - 1;
  // TIM1/APB2 source frequency equals to SystemCoreClock in our configuration,
  // we want 1 MHz
  TIM1_Handle.Init.Prescaler = SystemCoreClock / 2 / 1000000 - 1;
  TIM1_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TIM1_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM1_Handle.Init.RepetitionCounter = 0;
  HAL_TIM_PWM_Init(&TIM1_Handle);

  TIM_OC_InitTypeDef TIM_OC_InitStructure;
  TIM_OC_InitStructure.Pulse = LED_PWM_TIM_PERIOD - 1;
  TIM_OC_InitStructure.OCMode = TIM_OCMODE_PWM2;
  TIM_OC_InitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStructure.OCFastMode = TIM_OCFAST_DISABLE;
  TIM_OC_InitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  TIM_OC_InitStructure.OCIdleState = TIM_OCIDLESTATE_SET;
  TIM_OC_InitStructure.OCNIdleState = TIM_OCNIDLESTATE_SET;
  HAL_TIM_PWM_ConfigChannel(&TIM1_Handle, &TIM_OC_InitStructure, TIM_CHANNEL_1);

  HAL_TIM_PWM_Start(&TIM1_Handle, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&TIM1_Handle, TIM_CHANNEL_1);
}

void lcd_para_init(uint32_t lcd_width, uint32_t lcd_height,
                   uint32_t pixel_format) {
  uint32_t ltdc_pixel_format;

  if (pixel_format == LCD_PIXEL_FORMAT_RGB565) {
    ltdc_pixel_format = LTDC_PIXEL_FORMAT_RGB565;
    lcd_params.bbp = 2;

  } else {
    ltdc_pixel_format = LCD_PIXEL_FORMAT_ARGB8888;
    lcd_params.bbp = 4;
  }

  lcd_params.pixel_format = ltdc_pixel_format;
  lcd_params.xres = lcd_width;
  lcd_params.yres = lcd_height;
  lcd_params.fb_base = DISPLAY_MEMORY_BASE;

  // dma2d_copy_buffer((uint32_t *)_acgoogle1, (uint32_t *)DISPLAY_MEMORY_BASE,
  // 50,
  //                   400, 352, 110);
  // HAL_Delay(1000);
}

void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {}

void display_set_orientation(int degrees) {}

void display_clear_save(void) {}
const char *display_save(const char *prefix) { return NULL; }
void display_refresh(void) {}
