#include <stdint.h>

#include "rtt_log.h"

void rtt_log_init(void) {
  if (DEBUG_RTT) SEGGER_RTT_Init();
}

void rtt_log_hexdump(uint8_t *buf, uint32_t len) {
  uint8_t temp[4] = {0};
  uint8_t m;
  uint32_t i;

  for (i = 0; i < len; i++) {
    m = (buf[i] >> 4) & 0x0f;
    if (m <= 9)
      m += 0x30;
    else
      m += 55;
    temp[0] = m;
    m = buf[i] & 0x0f;
    if (m <= 9)
      m += 0x30;
    else
      m += 55;
    temp[1] = m;
    temp[2] = ' ';
    SEGGER_RTT_printf(0, (char *)temp);
  }
  SEGGER_RTT_printf(0, "\r\n");
}