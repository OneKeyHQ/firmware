#ifndef _BASE64_H_
#define _BASE64_H_

#include <stddef.h>

/** Convert a binary memory block in a base64 null-terminated string.
 * @param dest Destination memory wher to put the base64 null-terminated string.
 * @param src Source binary memory block.
 * @param size Size in bytes of source binary memory block.
 * @return A pointer to the null character of the base64 null-terminated string.
 */
char* bintob64(char* dest, void const* src, size_t size);

/** Convert a base64 string to binary format.
 * @param dest Destination memory block.
 * @param src Source base64 string.
 * @return If success a pointer to the next byte in memory block.
 *         Null if string has a bad format.  */
void* b64tobin(void* dest, char const* src);

/** Convert a base64 string to binary format.
 * @param p Source base64 string and destination memory block.
 * @return If success a pointer to the next byte in memory block.
 *         Null if string has a bad format.  */
static inline void* b64decode(void* p) { return b64tobin(p, (char*)p); }

#endif /* _BASE64_H_ */
