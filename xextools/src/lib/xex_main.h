#ifndef XEX_MAIN_H
#define XEX_MAIN_H

#include "xex_datatypes.h"

#define XEX_MAGIC00 'X'
#define XEX_MAGIC01 'E'
#define XEX_MAGIC02 'X'
#define XEX_MAGIC03 '2'

#define XEX_MAGIC 0x32584558 /* Little endian*/

#define X_OFF_MAG	0x0000
#define X_OFF_VERSION	0x0004
#define X_OFF_CODE	0x0008
#define X_OFF_RES0	0x000C
#define X_OFF_CERT	0x0010
#define X_OFF_NUMDW	0x0014

typedef struct {
	const char *buf;
	unsigned int file_length;
	int fd;
} xex_t;

xex_t *xex_load (const char *pathname);
int xex_validate (xex_t *xex);
void xex_unload (xex_t *xex);

#endif
