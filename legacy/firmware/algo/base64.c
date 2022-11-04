/* base64.c -- Encode binary data using printable characters.
   Copyright (C) 1999, 2000, 2001, 2004, 2005, 2006 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Simon Josefsson.  Partially adapted from GNU MailUtils
 * (mailbox/filter_trans.c, as of 2004-11-28).  Improved by review
 * from Paul Eggert, Bruno Haible, and Stepan Kasal.
 *
 * See also RFC 3548 <http://www.ietf.org/rfc/rfc3548.txt>.
 */

/* Get prototype. */
#include "base64.h"

/* C89 compliant way to cast 'char' to 'unsigned char'. */
static inline unsigned char to_uchar(char ch) { return ch; }

/* Base64 encode IN array of size INLEN into OUT array of size OUTLEN.
   If OUTLEN is less than BASE64_LENGTH(INLEN), write as many bytes as
   possible.  If OUTLEN is larger than BASE64_LENGTH(INLEN), also zero
   terminate the output buffer. */
void base64_encode(const char *restrict in, size_t inlen, char *restrict out,
                   size_t outlen) {
  static const char b64str[64] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  while (inlen && outlen) {
    *out++ = b64str[(to_uchar(in[0]) >> 2) & 0x3f];
    if (!--outlen) break;
    *out++ =
        b64str[((to_uchar(in[0]) << 4) + (--inlen ? to_uchar(in[1]) >> 4 : 0)) &
               0x3f];
    if (!--outlen) break;
    *out++ = (inlen ? b64str[((to_uchar(in[1]) << 2) +
                              (--inlen ? to_uchar(in[2]) >> 6 : 0)) &
                             0x3f]
                    : '=');
    if (!--outlen) break;
    *out++ = inlen ? b64str[to_uchar(in[2]) & 0x3f] : '=';
    if (!--outlen) break;
    if (inlen) inlen--;
    if (inlen) in += 3;
  }

  if (outlen) *out = '\0';
}
