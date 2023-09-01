#ifndef _MENU_PARA_H_
#define _MENU_PARA_H_

#include <stdint.h>

char* menu_para_ble_state(void);
char* menu_para_language(void);
char* menu_para_shutdown(void);
char* menu_para_autolock(void);
char* menu_para_eth_eip_switch(void);
char* menu_para_sol_switch(void);
char* menu_para_passphrase(void);
char* menu_para_trezor_comp_mode_state(void);
char* menu_para_safety_checks_state(void);
char* menu_para_usb_lock(void);
char* menu_para_input_direction(void);

int menu_para_ble_index(void);
int menu_para_language_index(void);
int menu_para_shutdown_index(void);
int menu_para_autolock_index(void);
int menu_para_passphrase_index(void);
int menu_para_trezor_comp_mode_index(void);
int menu_para_safety_checks_index(void);
int menu_para_usb_lock_index(void);
int menu_para_input_direction_index(void);

void menu_para_set_ble(int index);
void menu_para_set_language(int index);
void menu_para_set_shutdown(int index);
void menu_para_set_sleep(int index);

char* format_time(uint32_t ms);

#endif
