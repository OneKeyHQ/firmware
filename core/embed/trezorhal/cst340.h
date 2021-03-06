#ifndef _TOUCH_CST340_H_
#define _TOUCH_CST340_H_

/*register address*/
/********selfcap register address start *****************/

#define HYN_REG_CAP_POWER_MODE 0xA5
#define HYN_REG_CAP_POWER_MODE_SLEEP_VALUE 0x03
#define HYN_REG_CAP_FW_VER 0xA6
#define HYN_REG_CAP_VENDOR_ID 0xA8
#define HYN_REG_CAP_PROJECT_ID 0xA9
#define HYN_REG_CAP_CHIP_ID 0xAA
#define HYN_REG_CAP_CHIP_CHECKSUM 0xAC

#define HYN_REG_CAP_GESTURE_EN 0xD0
#define HYN_REG_CAP_GESTURE_OUTPUT_ADDRESS 0xD3

#define HYN_REG_CAP_PROXIMITY_EN 0xB0
#define HYN_REG_CAP_PROXIMITY_OUTPUT_ADDRESS 0x01

#define HYN_REG_CAP_ESD_SATURATE 0xE0
/********selfcap register address end *****************/

/********mutcap register address start *****************/

// Myabe change
#define HYN_REG_MUT_ESD_VALUE 0xD040
#define HYN_REG_MUT_ESD_CHECKSUM 0xD046
#define HYN_REG_MUT_PROXIMITY_EN 0xD04B
#define HYN_REG_MUT_PROXIMITY_OUTPUT_ADDRESS 0xD04B
#define HYN_REG_MUT_GESTURE_EN 0xD04C
#define HYN_REG_MUT_GESTURE_OUTPUT_ADDRESS 0xD04C

// workmode
#define HYN_REG_MUT_DEBUG_INFO_MODE 0xD101
#define HYN_REG_MUT_RESET_MODE 0xD102
#define HYN_REG_MUT_DEBUG_RECALIBRATION_MODE 0xD104
#define HYN_REG_MUT_DEEP_SLEEP_MODE 0xD105
#define HYN_REG_MUT_DEBUG_POINT_MODE 0xD108
#define HYN_REG_MUT_NORMAL_MODE 0xD109

#define HYN_REG_MUT_DEBUG_RAWDATA_MODE 0xD10A
#define HYN_REG_MUT_DEBUG_DIFF_MODE 0xD10D
#define HYN_REG_MUT_DEBUG_FACTORY_MODE 0xD119
#define HYN_REG_MUT_DEBUG_FACTORY_MODE_2 0xD120

#endif
