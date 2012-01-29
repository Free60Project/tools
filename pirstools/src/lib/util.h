#ifndef SWISSKNIFE_H
#define SWISSKNIFE_H

#include "pirs_main.h"

#define bs32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
#define bs16(x) ((((x) & 0x0000ff00) >> 8) | (((x) & 0x000000ff) << 8))
#define round(n, r) ((n) + ((r) - ((n) % (r)) == (r) ? 0 : (r) - ((n) % (r))))
#define bit_get(number, i) (((number) & (1 << (i))) > 0)

inline long get_dword (pirs_t *pirs, unsigned int offset);

void pirs_warning (char *fmt, ...);
void pirs_error (char *fmt, ...);

int utf8_to_iso (const char *utf8string, char *isobuffer);
const char *str_concatenate (const char *s1, ...);
const char *strdup_printf (char *fmt, ...);

#define MAX_STRDUP_PRINTF_BUF 2048

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#endif
