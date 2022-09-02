#ifndef _MENU_PARA_H_
#define _MENU_PARA_H_

char* menu_para_ble_state(void);
char* menu_para_language(void);
char* menu_para_shutdown(void);
char* menu_para_autolock(void);
#if ONEKEY_MINI
char* menu_para_brightness(void);
#endif
char* menu_para_eth_eip_switch(void);
char* menu_para_sol_switch(void);
char* menu_para_cfx_cip_switch(void);
char* menu_para_passphrase(void);

void menu_para_set_ble(int index);
void menu_para_set_language(int index);
void menu_para_set_shutdown(int index);
void menu_para_set_sleep(int index);
#if ONEKEY_MINI
void menu_para_set_brightness(int index);
#endif

#endif
