    .syntax unified

#include "version.h"

    .section .header, "a"

    .type g_header, %object
    .size g_header, .-g_header

g_header:
#if ONEKEY_MINI
    .byte 'M','I','N','I'            // magic
#else
    .byte 'T','R','Z','F'            // magic
#endif
    .word reset_handler              // reset handler,  replace later with : .word g_header_end - g_header    // hdrlen
    .word 0                          // expiry
    .word _codelen                   // codelen
    .byte VERSION_MAJOR              // vmajor
    .byte VERSION_MINOR              // vminor
    .byte VERSION_PATCH              // vpatch
    .byte 0                          // vbuild
    .byte FIX_VERSION_MAJOR          // fix_vmajor
    .byte FIX_VERSION_MINOR          // fix_vminor
    .byte FIX_VERSION_PATCH          // fix_vpatch
    .byte 0                          // fix_vbuild
    .byte ONEKEY_VERSION_HEX>>8
    .byte ONEKEY_VERSION_HEX
    . = . + 4                        // reserved
    . = . + 512                      // hash1 ... hash16
#if ONEKEY_MINI
    . = . + 64                       // sig1
    . = . + 64                       // sig2
    . = . + 64                       // sig3
    . = . + 64                       // sig4
    .byte 0                          // sigindex1
    .byte 0                          // sigindex2
    .byte 0                          // sigindex3
    .byte 0                          // sigindex4
    . = . + 155                      // reserved
#else
    . = . + 64                       // sig1
    . = . + 64                       // sig2
    . = . + 64                       // sig3
    .byte 0                          // sigindex1
    .byte 0                          // sigindex2
    .byte 0                          // sigindex3
    . = . + 220                      // reserved
#endif
    . = . + 65                       // reserved
g_header_end:
