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
  const BITMAP *bmp_yes = NULL;
  char *text_yes = NULL;
#if ONEKEY_MINI
  int cnt = 0;
  bool align = false;
  char *text_no = NULL;
#endif

  strcat(desc, _(menu->items[menu->current].name));
  if (menu->items[menu->current].name2) {
    strcat(desc, " ");
    strcat(desc, _(menu->items[menu->current].name2));
  }
  if (menu->items[menu->current].para != NULL) {
#if ONEKEY_MINI
    strcat(desc, "    ");
#else
    strcat(desc, "      ");
#endif
    strcat(desc, menu->items[menu->current].para());
  }

  switch (menu->button_type) {
    case BTN_TYPE_NEXT:
#if ONEKEY_MINI
      bmp_yes = NULL;
      text_yes = NULL;
#else
      bmp_yes = &bmp_btn_forward;
      text_yes = _("Next");
#endif
      break;
    case BTN_TYPE_YES:
    default:
#if ONEKEY_MINI
      bmp_yes = &bmp_button_forward;
      text_yes = _("OK");
      text_no = _("BACK");
#else
      bmp_yes = &bmp_btn_confirm;
      text_yes = _("Okay");
#endif
      break;
  }

#if ONEKEY_MINI
  for (int i = 0; i < menu->counts; i++) {
    cnt += menu->items[i].is_function ? 1 : 0;
  }

  if (cnt < menu->counts)
    align = false;
  else
    align = true;

  layoutMenuItemsEx(text_yes, text_no, bmp_yes, menu->current + 1, menu->counts,
                    align, menu->title ? _(menu->title) : NULL, desc,
                    0 < menu->counts ? _(menu->items[0].name) : NULL,
                    1 < menu->counts ? _(menu->items[1].name) : NULL,
                    2 < menu->counts ? _(menu->items[2].name) : NULL,
                    3 < menu->counts ? _(menu->items[3].name) : NULL,
                    4 < menu->counts ? _(menu->items[4].name) : NULL,
                    5 < menu->counts ? _(menu->items[5].name) : NULL);
#else
  layoutMenuItems(
      text_yes, bmp_yes, menu->current + 1, menu->counts,
      menu->title ? _(menu->title) : NULL, desc,
      menu->current > 0 ? _(menu->items[menu->current - 1].name) : NULL,
      menu->current < menu->counts - 1 ? _(menu->items[menu->current + 1].name)
                                       : NULL);
#endif
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
    if (layoutLast != layoutHome) layoutLast = menu_run;
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

  if (layoutLast != menu_run) {
    refresh = true;
    layoutLast = menu_run;
  }

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
