#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "gettext.h"
#include "language.h"

extern uint8_t ui_language;

// static int stricmp(const char* s1, const char* s2) {
//   char c1, c2;
//   do {
//     c1 = *s1++;
//     c2 = *s2++;
//     if (c1 >= 'A' && c1 <= 'Z') {
//       c1 += 0x20;
//     }
//     if (c2 >= 'A' && c2 <= 'Z') {
//       c2 += 0x20;
//     }
//   } while (c1 && (c1 == c2));
//   return (c1 - c2);
// }

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