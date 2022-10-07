#ifndef ONEKEY_LCD_H
#define ONEKEY_LCD_H

// defines
#define ONEKEY_PIXEL_FORMAT_ARGB8888 0x00000000U /*!< ARGB8888 LTDC pixel format */
#define ONEKEY_PIXEL_FORMAT_RGB888 0x00000001U   /*!< RGB888 LTDC pixel format   */
#define ONEKEY_PIXEL_FORMAT_RGB565 0x00000002U   /*!< RGB565 LTDC pixel format   */
#define DISPLAY_MEMORY_BASE FMC_SDRAM_LTDC_BUFFER_ADDRESS
#define DISPLAY_MEMORY_FB_A DISPLAY_MEMORY_BASE
#define DISPLAY_MEMORY_FB_B DISPLAY_MEMORY_BASE + LTDC_PIXEL_DATA_LAYER_SIZE

// declears
void lcd_reset_pin_init();
void lcd_te_pin_init();
void lcd_reset();

void onekey_lcd_init(
    uint32_t lcd_width, uint32_t lcd_height,
    uint32_t pixel_format);

#define LED_PWM_TIM_PERIOD (100) // TODO: see if there is other way
void display_set_backlight(int val);
void lcd_pwm_init(void);

// exposed funcs

extern void fb_write_pixel(
    uint32_t x_pos, uint32_t y_pos,
    uint32_t color);

extern void fb_fill_buffer(
    uint32_t *dest,
    uint32_t x_size, uint32_t y_size,
    uint32_t offset,
    uint32_t color);

extern void fb_fill_rect(
    uint32_t x_pos, uint32_t y_pos,
    uint32_t width, uint32_t height,
    uint32_t color);

extern void fb_draw_hline(
    uint32_t x_pos, uint32_t y_pos,
    uint32_t len,
    uint32_t color);

extern void fb_draw_vline(
    uint32_t x_pos, uint32_t y_pos,
    uint32_t len,
    uint32_t color);

extern void onekey_lcd_dma2d_copy_buffer(
    uint32_t *pSrc, uint32_t *pDst,
    uint16_t x, uint16_t y,
    uint16_t xsize, uint16_t ysize);

// NOTE: this function will NOT USE the pDst
extern void dma2d_copy_buffer(
    uint32_t *pSrc, uint32_t *pDst,
    uint16_t x, uint16_t y,
    uint16_t xsize, uint16_t ysize);

// TODO: cleanup

// int32_t bsp_get_tick(void)
// {
//     return (int32_t)HAL_GetTick();
// }

// static inline uint32_t ltdc_to_dma2d_format_convert(uint32_t ldtc)

void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void display_set_orientation(int degrees);
void display_clear_save(void);
const char *display_save(const char *prefix);
void display_refresh(void);

#endif // ONEKEY_LCD_H
