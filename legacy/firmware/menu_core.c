#include "menu_core.h"
#include "bitmaps.h"
#include "buttons.h"
#include "gettext.h"
#include "layout2.h"

static struct menu *currentMenu;

void menu_init(struct menu *menu) {
  currentMenu = menu;
  currentMenu->current = currentMenu->start;
}

void menu_display(struct menu *menu) {
  char desc[64] = "";

  strcat(desc, _(menu->items[menu->current].name));
  if (menu->items[menu->current].name2) {
    strcat(desc, " ");
    strcat(desc, _(menu->items[menu->current].name2));
  }
  if (menu->items[menu->current].para != NULL) {
    strcat(desc, "      ");
    strcat(desc, menu->items[menu->current].para());
  }

  layoutMenuItems(
      &bmp_btn_up, &bmp_btn_down, menu->current + 1, menu->counts,
      menu->title ? _(menu->title) : NULL, desc,
      menu->current > 0 ? _(menu->items[menu->current - 1].name) : NULL,
      menu->current < menu->counts - 1 ? _(menu->items[menu->current + 1].name)
                                       : NULL);
}

void menu_up(void) {
  if (currentMenu->current > 0) {
    currentMenu->current--;
  }
}

void menu_down(void) {
  if (currentMenu->current < currentMenu->counts - 1) {
    currentMenu->current++;
  }
}

void menu_enter(void) {
  if (!currentMenu->items[currentMenu->current].is_function &&
      currentMenu->items[currentMenu->current].sub_menu) {
    currentMenu = currentMenu->items[currentMenu->current].sub_menu;
    currentMenu->current = currentMenu->start;
  } else if (currentMenu->items[currentMenu->current].func != NULL) {
    currentMenu->items[currentMenu->current].func(currentMenu->current);
    layoutLast = menu_run;
    if (currentMenu->previous) currentMenu = currentMenu->previous;
  }
}

void menu_exit(void) {
  currentMenu->current = currentMenu->start;
  if (currentMenu->previous == NULL) {
    layoutHome();
  } else {
    currentMenu = currentMenu->previous;
  }
}

void menu_run(uint8_t key, uint32_t time) {
  static bool refresh = true;
  static uint32_t wait_time = 0;

  layoutLast = menu_run;

  if (refresh) {
    refresh = false;
    menu_display(currentMenu);
    wait_time = time;
  }
  if (wait_time + timer1s * 30 < time) {
    if (key == KEY_NULL) key = KEY_CANCEL;
  }
  switch (key) {
    case KEY_UP:
      menu_up();
      break;
    case KEY_DOWN:
      menu_down();
      break;
    case KEY_CANCEL:
      menu_exit();
      break;
    case KEY_CONFIRM:
      menu_enter();
      break;
    default:
      break;
  }
  if (key != KEY_NULL) {
    refresh = true;
  }
}
