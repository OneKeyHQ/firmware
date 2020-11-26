#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "gettext.h"
#include "language.h"

extern uint8_t ui_language;

char* gettext(const char* msgid) {
  if (ui_language == 0) return (char*)msgid;

  int lo = 0, hi = LANGUAGE_ITEMS - 1;
  while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;
    int cmp = strcmp(msgid, languages[mid][0]);
    if (cmp == 0) {
      if (strcmp("", languages[mid][1]) == 0)  // non translate
        return (char*)msgid;
      else
        return (char*)languages[mid][1];
    }
    if (cmp > 0) {
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }
  return (char*)msgid;
}
