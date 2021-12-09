#ifndef _MENU_PARA_H_
#define _MENU_PARA_H_

char* menu_para_ble_state(void);
char* menu_para_language(void);
char* menu_para_shutdown(void);
char* menu_para_autolock(void);
char* menu_para_eth_eip_switch(void);
char* menu_para_sol_switch(void);
char* menu_para_cfx_cip_switch(void);

void menu_para_set_ble(int index);
void menu_para_set_language(int index);
void menu_para_set_shutdown(int index);
void menu_para_set_sleep(int index);

#endif
