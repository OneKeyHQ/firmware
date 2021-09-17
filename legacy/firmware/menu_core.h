#ifndef _MENU_CORE_H_
#define _MENU_CORE_H_
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

struct menu_item {
  char *name;
  char *name2;  // for time unit
  bool is_function;
  union {
    void (*func)(int index);
    struct menu *sub_menu;
  };
  char *((*para)(void));
  bool go_prev;
};

enum {
  BTN_TYPE_YES = 0,
  BTN_TYPE_NEXT,
};

struct menu {
  int start;    // cursor start position
  int current;  // cursor current position
  int counts;   // items count
  char *title;
  int button_type;  // yes button text and icon
  struct menu_item *items;
  struct menu *previous;
};

#define COUNT_OF(name) (sizeof(name) / sizeof(name[0]))

void menu_init(struct menu *menu);
void menu_display(struct menu *menu);
void menu_up(void);
void menu_down(void);
void menu_enter(void);
void menu_run(uint8_t key, uint32_t time);

#define menu_update(menu, field, value) (menu)->field = value

#endif
