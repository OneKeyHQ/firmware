#ifndef _TREZORHAL_THD89_H_
#define _TREZORHAL_THD89_H_

#include "secbool.h"

void thd89_init(void);
secbool thd89_transmit(uint8_t *cmd, uint16_t len, uint8_t *resp,
                       uint16_t *resp_len);
uint16_t thd89_last_error();

#endif
