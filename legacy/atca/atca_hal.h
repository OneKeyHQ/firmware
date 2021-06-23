#ifndef _ATCA_HAL_H_
#define _ATCA_HAL_H_

#include <stdint.h>

#include "atca_status.h"

#define ATCA_FREQUENCY_TIME_MSEC 2
#define ATCA_MAX_TIME_MSEC 2000

#ifdef ATCA_INTERFACE_I2C

#else
enum swi_flag {
  SWI_FLAG_CMD = (uint8_t)0x77,   //!< flag preceding a command
  SWI_FLAG_TX = (uint8_t)0x88,    //!< flag requesting a response
  SWI_FLAG_IDLE = (uint8_t)0xBB,  //!< flag requesting to go into Idle mode
  SWI_FLAG_SLEEP = (uint8_t)0xCC  //!< flag requesting to go into Sleep mode
};
#endif

void atca_init(void);
ATCA_STATUS atca_wake(void);
ATCA_STATUS atca_idle(void);
ATCA_STATUS atca_send(uint8_t *tx_data, uint8_t tx_len);
ATCA_STATUS atca_receive(uint8_t *rx_data, uint8_t *rx_len);

#endif
