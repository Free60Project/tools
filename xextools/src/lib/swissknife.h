#ifndef SWISSKNIFE_H
#define SWISSKNIFE_H

#include "xex_main.h"

#define bs32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
#define bs16(x) ((((x) & 0x0000ff00) >> 8) | (((x) & 0x000000ff) << 8))
#define round(n, r) ((n) + ((r) - ((n) % (r)) == (r) ? 0 : (r) - ((n) % (r))))
#define bit_get(number, i) (((number) & (1 << (i))) > 0)

inline long get_dword (xex_t *xex, unsigned int offset);

void xex_warning (char *fmt, ...);
void xex_error (char *fmt, ...);

#define DWORD long

#endif
