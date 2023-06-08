#include "menu_core.h"
#include "bitmaps.h"
#include "buttons.h"
#include "config.h"
#include "gettext.h"
#include "layout2.h"

static struct menu *currentMenu;

void menu_init(struct menu *menu) {
  currentMenu = menu;
  currentMenu->current = currentMenu->start;
}

void menu_display(struct menu *menu) {
  char desc[64] = "";
  char previous_desc[64] = "";
  char pre_previous_desc[64] = "";
  char pre_pre_previous_desc[64] = "";
  char next_desc[64] = "";
  char next_next_desc[64] = "";
  char next_next_next_desc[64] = "";
  const BITMAP *bmp_yes = NULL;
  char *text_yes = NULL;

  strcat(desc, _(menu->items[menu->current].name));
  if (menu->items[menu->current].name2) {
    strcat(desc, " ");
    strcat(desc, _(menu->items[menu->current].name2));
  }
  if (menu->items[menu->current].para != NULL) {
    strcat(desc, "      ");
    strcat(desc, menu->items[menu->current].para());
  }

  if (menu->current > 0) {
    strcat(previous_desc, _(menu->items[menu->current - 1].name));
    if (menu->items[menu->current - 1].name2) {
      strcat(previous_desc, " ");
      strcat(previous_desc, _(menu->items[menu->current - 1].name2));
    }
  }
  if (menu->current > 1) {
    strcat(pre_previous_desc, _(menu->items[menu->current - 2].name));
    if (menu->items[menu->current - 2].name2) {
      strcat(pre_previous_desc, " ");
      strcat(pre_previous_desc, _(menu->items[menu->current - 2].name2));
    }
  }
  if (menu->current > 2) {
    strcat(pre_pre_previous_desc, _(menu->items[menu->current - 3].name));
    if (menu->items[menu->current - 3].name2) {
      strcat(pre_pre_previous_desc, " ");
      strcat(pre_pre_previous_desc, _(menu->items[menu->current - 3].name2));
    }
  }
  if (menu->current < menu->counts - 1) {
    strcat(next_desc, _(menu->items[menu->current + 1].name));
    if (menu->items[menu->current + 1].name2) {
      strcat(next_desc, " ");
      strcat(next_desc, _(menu->items[menu->current + 1].name2));
    }
  }
  if (menu->current < menu->counts - 2) {
    strcat(next_next_desc, _(menu->items[menu->current + 2].name));
    if (menu->items[menu->current + 2].name2) {
      strcat(next_next_desc, " ");
      strcat(next_next_desc, _(menu->items[menu->current + 2].name2));
    }
  }
  if (menu->current < menu->counts - 3) {
    strcat(next_next_next_desc, _(menu->items[menu->current + 3].name));
    if (menu->items[menu->current + 3].name2) {
      strcat(next_next_next_desc, " ");
      strcat(next_next_next_desc, _(menu->items[menu->current + 3].name2));
    }
  }

  switch (menu->button_type) {
    case BTN_TYPE_NEXT:
      bmp_yes = &bmp_bottom_right_arrow;
      text_yes = _("Next");
      break;
    case BTN_TYPE_YES:
    default:
      bmp_yes = &bmp_bottom_right_confirm;
      text_yes = _("Okay");
      break;
  }

  layoutMenuItemsEx(
      text_yes, bmp_yes, menu->current + 1, menu->counts,
      menu->title ? _(menu->title) : NULL, desc,
      _(menu->items[menu->current].name),
      menu->items[menu->current].name2 ? _(menu->items[menu->current].name2)
                                       : NULL,
      menu->items[menu->current].para ? menu->items[menu->current].para()
                                      : NULL,
      menu->current > 0 ? previous_desc : NULL,
      menu->current > 1 ? pre_previous_desc : NULL,
      menu->current > 2 ? pre_pre_previous_desc : NULL,
      menu->current < menu->counts - 1 ? next_desc : NULL,
      menu->current < menu->counts - 2 ? next_next_desc : NULL,
      menu->current < menu->counts - 3 ? next_next_next_desc : NULL);
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
    if ((currentMenu->items[currentMenu->current].para != NULL) &&
        (currentMenu->items[currentMenu->current].index != NULL)) {
      int index = currentMenu->items[currentMenu->current].index();
      currentMenu = currentMenu->items[currentMenu->current].sub_menu;
      currentMenu->current = index;
    } else {
      currentMenu = currentMenu->items[currentMenu->current].sub_menu;
      currentMenu->current = currentMenu->start;
    }
  } else if (currentMenu->items[currentMenu->current].func != NULL) {
    currentMenu->items[currentMenu->current].func(currentMenu->current);
    if (layoutLast != layoutHome) layoutLast = menu_run;
    if (currentMenu->previous &&
        currentMenu->items[currentMenu->current].go_prev) {
      currentMenu = currentMenu->previous;
    }
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
#if EMULATOR
  if (!config_isInitialized()) {
    layoutLast = onboarding;
  }
#endif
}
