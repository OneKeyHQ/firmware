#ifndef _ST7701S_H_
#define _ST7701S_H_

#define ST7701S_FORMAT_RGB888 0x70 /* Pixel  RGB888 : 24 bpp*/
#define ST7701S_FORMAT_RBG565 0x50 /* Pixel  RGB565 : 16 bpp */

#define ST7701S_480X800_HSYNC 4 /* Horizontal synchronization */
#define ST7701S_480X800_HBP 38  /* Horizontal back porch      */
#define ST7701S_480X800_HFP 38  /* Horizontal front porch     */
#define ST7701S_480X800_VSYNC 4 /* Vertical synchronization   */
#define ST7701S_480X800_VBP 9   /* Vertical back porch        */
#define ST7701S_480X800_VFP 5   /* Vertical front porch       */

// #define ST7701S_480X800_HSYNC 2 /* Horizontal synchronization */
// #define ST7701S_480X800_HBP 34  /* Horizontal back porch      */
// #define ST7701S_480X800_HFP 34  /* Horizontal front porch     */
// #define ST7701S_480X800_VSYNC 1 /* Vertical synchronization   */
// #define ST7701S_480X800_VBP 15  /* Vertical back porch        */
// #define ST7701S_480X800_VFP 16  /* Vertical front porch       */

#endif
