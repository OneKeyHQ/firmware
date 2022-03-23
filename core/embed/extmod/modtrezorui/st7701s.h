#ifndef _ST7701S_H_
#define _ST7701S_H_

#define ST7701S_FORMAT_RGB888 0x70 /* Pixel  RGB888 : 24 bpp*/
#define ST7701S_FORMAT_RBG565 0x50 /* Pixel  RGB565 : 16 bpp */

#define ST7701S_480X800_HSYNC 2 /* Horizontal synchronization */
#define ST7701S_480X800_HBP 2   /* Horizontal back porch      */
#define ST7701S_480X800_HFP 2   /* Horizontal front porch     */
#define ST7701S_480X800_VSYNC 2 /* Vertical synchronization   */
#define ST7701S_480X800_VBP 10  /* Vertical back porch        */
#define ST7701S_480X800_VFP 2   /* Vertical front porch       */

// #define ST7701S_480X800_HSYNC 2 /* Horizontal synchronization */
// #define ST7701S_480X800_HBP 34  /* Horizontal back porch      */
// #define ST7701S_480X800_HFP 34  /* Horizontal front porch     */
// #define ST7701S_480X800_VSYNC 1 /* Vertical synchronization   */
// #define ST7701S_480X800_VBP 15  /* Vertical back porch        */
// #define ST7701S_480X800_VFP 16  /* Vertical front porch       */

#endif
