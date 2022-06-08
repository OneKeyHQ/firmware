#ifndef _SYS_H_
#define _SYS_H_

#include <stdbool.h>
#include <stdint.h>

extern uint8_t battery_cap;
extern uint8_t dev_pwr_sta;

#define PERIOD_VALUE (uint32_t)(1000 - 1)
#define PULSE1_VALUE (uint32_t)(PERIOD_VALUE / 2)

enum MOTOR_STA { MOTOR_COAST = 0x0, MOTOR_REVERSE, MOTOR_FORWARD, MOTOR_BRAKE };

void buzzer_init(void);
void buzzer_ctrl(bool start);
void motor_init(void);
void motor_ctrl(enum MOTOR_STA sta);

#endif
