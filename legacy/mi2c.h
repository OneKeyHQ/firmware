#ifndef _mi2c_H_
#define _mi2c_H_

#include <stdint.h>
#include <string.h>

#include "sys.h"
#include "usart.h"

#define MI2C_TIMEOUT (40000)
#define MI2C_BUF_MAX_LEN (1024 + 512)
#define MI2C_SEND_MAX_LEN (1024 + 512)

#define MI2CX I2C1

// master I2C gpio
#define GPIO_MI2C_PORT GPIOB

//#define MI2C_COMBUS     GPIO2
#define GPIO_MI2C_SCL GPIO8
#define GPIO_MI2C_SDA GPIO9

// master I2C addr
#define MI2C_ADDR 0x10
#define MI2C_READ 0x01
#define MI2C_WRITE 0x00

#define MI2C_XOR_LEN (1)

//#define	GET_MI2C_COMBUS	        (gpio_get(GPIO_MI2C_PORT, MI2C_COMBUS))

extern uint8_t g_ucMI2cRevBuf[MI2C_BUF_MAX_LEN];
extern uint8_t g_ucMI2cSendBuf[MI2C_BUF_MAX_LEN];

extern uint16_t g_usMI2cRevLen;

#define CLA (g_ucMI2cSendBuf[0])
#define INS (g_ucMI2cSendBuf[1])
#define P1 (g_ucMI2cSendBuf[2])
#define P2 (g_ucMI2cSendBuf[3])
#define P3 (g_ucMI2cSendBuf[4])

#define SH_IOBUFFER (g_ucMI2cSendBuf + 5)
#define SH_CMDHEAD (g_ucMI2cSendBuf)

#if !EMULATOR
extern void vMI2CDRV_Init(void);
extern bool bMI2CDRV_ReceiveData(uint8_t *pucStr, uint16_t *pusRevLen);
extern bool bMI2CDRV_SendData(uint8_t *pucStr, uint16_t usStrLen);
#else
#define vMI2CDRV_Init(...)
#define bMI2CDRV_SendData(...) true
#define bMI2CDRV_ReceiveData(...) true
#endif

#endif
