#include STM32_HAL_H

// CONFIG
#include "onekey_lcd_config.h"
// UTILS
#include "onekey_lcd_utils.h"

// MODULES
#include "onekey_lcd_common.h"
#include "onekey_lcd_dma2d.h"
#include "onekey_lcd_dsi.h"
#include "onekey_lcd_framebuffer.h"
#include "onekey_lcd_ltdc.h"
#include "onekey_lcd_st7701s.h"

// HEADER
#include "onekey_lcd.h"

LTDC_HandleTypeDef hlcd_ltdc;
DSI_HandleTypeDef hlcd_dsi;
DMA2D_HandleTypeDef hlcd_dma2d;

// this struct contains all lcd related infos
ONEKEY_LCD_PARAMS lcd_params = {0};
ONEKEY_LCD_PARAMS *ptr_lcd_params = NULL;

// this is the function that config everything in the package
static void inline lcd_parms_config(
    ONEKEY_LCD_PARAMS *lcd_params,
    uint32_t onekey_pixel_format)
{
}

void onekey_lcd_init(
    uint32_t lcd_width, uint32_t lcd_height,
    uint32_t onekey_pixel_format)
{
    // lcd_params init
    ptr_lcd_params = &lcd_params;

    // config everything in the package
    // all initialize values should be configured here

    ptr_lcd_params->handles.ltdc = &hlcd_ltdc;
    ptr_lcd_params->handles.dsi = &hlcd_dsi;
    ptr_lcd_params->handles.dma2d = &hlcd_dma2d;

    ptr_lcd_params->res.x = lcd_width;
    ptr_lcd_params->res.y = lcd_height;

    ptr_lcd_params->fb.a = DISPLAY_MEMORY_FB_A;
    ptr_lcd_params->fb.b = DISPLAY_MEMORY_FB_B;
    ptr_lcd_params->fb.current = ptr_lcd_params->fb.a; //using a as default

    ptr_lcd_params->gpio.bl_pin = LCD_BL_CTRL_PIN;
    ptr_lcd_params->gpio.bl_port = LCD_BL_CTRL_GPIO_PORT;
    ptr_lcd_params->gpio.reset_pin = LCD_RESET_PIN;
    ptr_lcd_params->gpio.reset_port = LCD_RESET_GPIO_PORT;
    ptr_lcd_params->gpio.te_pin = LCD_TE_PIN;
    ptr_lcd_params->gpio.te_port = LCD_TE_GPIO_PORT;

    ptr_lcd_params->timing.hsync = ST7701S_480X800_HSYNC;
    ptr_lcd_params->timing.hfp = ST7701S_480X800_HFP;
    ptr_lcd_params->timing.hbp = ST7701S_480X800_HBP;
    ptr_lcd_params->timing.vsync = ST7701S_480X800_VSYNC;
    ptr_lcd_params->timing.vfp = ST7701S_480X800_VFP;
    ptr_lcd_params->timing.vbp = ST7701S_480X800_VBP;

    color_format_config(ptr_lcd_params, onekey_pixel_format);

    ptr_lcd_params->display_layer.one.x0 = 0;
    ptr_lcd_params->display_layer.one.x1 = ptr_lcd_params->res.x;
    ptr_lcd_params->display_layer.one.y0 = 0;
    ptr_lcd_params->display_layer.one.y1 = ptr_lcd_params->res.y;
    ptr_lcd_params->display_layer.one.pixel_format = ptr_lcd_params->pixel_format.ltdc;
    ptr_lcd_params->display_layer.one.address = ptr_lcd_params->fb.current;

    ptr_lcd_params->display_layer.two.x0 = 0;
    ptr_lcd_params->display_layer.two.x1 = ptr_lcd_params->res.x;
    ptr_lcd_params->display_layer.two.y0 = 0;
    ptr_lcd_params->display_layer.two.y1 = ptr_lcd_params->res.y;
    ptr_lcd_params->display_layer.two.pixel_format = ptr_lcd_params->pixel_format.ltdc;
    // if you plan use layer 2 to blend, change both layer to use diffrenet buffer!
    ptr_lcd_params->display_layer.two.address = ptr_lcd_params->fb.current;

    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_FOREGROUND_LAYER].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_FOREGROUND_LAYER].InputAlpha = 0xFF;
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_FOREGROUND_LAYER].InputColorMode = ptr_lcd_params->pixel_format.dma2d_input;
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_FOREGROUND_LAYER].InputOffset = 0;
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_FOREGROUND_LAYER].RedBlueSwap = DMA2D_RB_REGULAR;      /* No ForeGround Red/Blue swap */
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_FOREGROUND_LAYER].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */

    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_BACKGROUND_LAYER].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_BACKGROUND_LAYER].InputAlpha = 0xFF;
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_BACKGROUND_LAYER].InputColorMode = ptr_lcd_params->pixel_format.dma2d_input;
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_BACKGROUND_LAYER].InputOffset = 0;
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_BACKGROUND_LAYER].RedBlueSwap = DMA2D_RB_REGULAR;      /* No ForeGround Red/Blue swap */
    ptr_lcd_params->handles.dma2d->LayerCfg[DMA2D_BACKGROUND_LAYER].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */

    // gpio config and reset lcd
    lcd_reset_pin_init();
    lcd_te_pin_init();
    lcd_reset();

    // ltdc cofig
    onekey_lcd_ltdc_init(LTDC, ptr_lcd_params);
    onekey_lcd_ltdc_layer_init(ptr_lcd_params, LTDC_LAYER_1);
    // onekey_lcd_ltdc_layer_init(ptr_lcd_params, LTDC_LAYER_2);

    // dma2d config
    onekey_lcd_dma2d_init(DMA2D, ptr_lcd_params);
    onekey_lcd_dma2d_layer_init(ptr_lcd_params, DMA2D_FOREGROUND_LAYER);
    // onekey_lcd_dma2d_layer_init(ptr_lcd_params, DMA2D_BACKGROUND_LAYER);

    // dsi host config
    onekey_lcd_dsi_init(DSI, ptr_lcd_params);
    /* Enable the DSI host and wrapper after the LTDC initialization
      To avoid any synchronization issue, the DSI shall be started after
   enabling the LTDC */
    (void)HAL_DSI_Start(ptr_lcd_params->handles.dsi);
    //Enable the DSI BTW for read operations
    (void)HAL_DSI_ConfigFlowControl(ptr_lcd_params->handles.dsi, DSI_FLOW_CONTROL_BTA);

    // lcd cog config
    st7701_init(ptr_lcd_params, &onekey_lcd_dsi_write);
}

//=====================================================================

void display_set_backlight(int val)
{
    TIM1->CCR1 = (LED_PWM_TIM_PERIOD - 1) * val / 255;
}

void lcd_reset_pin_init()
{
    GPIO_InitTypeDef GPIO_InitTypeStruct = {0};

    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitTypeStruct.Pin = ptr_lcd_params->gpio.reset_pin;
    GPIO_InitTypeStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitTypeStruct.Pull = GPIO_PULLUP;
    GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ptr_lcd_params->gpio.reset_port, &GPIO_InitTypeStruct);
}

void lcd_te_pin_init()
{
    GPIO_InitTypeDef GPIO_InitTypeStruct = {0};
    __HAL_RCC_GPIOJ_CLK_ENABLE();

    GPIO_InitTypeStruct.Pin = ptr_lcd_params->gpio.te_pin;
    GPIO_InitTypeStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitTypeStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ptr_lcd_params->gpio.te_port, &GPIO_InitTypeStruct);
    HAL_GPIO_WritePin(ptr_lcd_params->gpio.te_port, ptr_lcd_params->gpio.te_pin, GPIO_PIN_SET);
}

void lcd_reset()
{
    HAL_GPIO_WritePin(ptr_lcd_params->gpio.reset_port, ptr_lcd_params->gpio.reset_pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(ptr_lcd_params->gpio.reset_port, ptr_lcd_params->gpio.reset_pin, GPIO_PIN_SET);
    HAL_Delay(10);
}

void lcd_pwm_init(void)
{
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
    // TIM1/APB2 source frequency equals to SystemCoreClock in our
    // configuration, we want 1 MHz
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

void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {}
void display_set_orientation(int degrees) {}
void display_clear_save(void) {}
const char *display_save(const char *prefix) { return NULL; }
void display_refresh(void) {}