#ifndef PIRS_MAIN_H
#define PIRS_MAIN_H

#include "pirs_datatypes.h"

#define PIRS_MAGIC00 'P'
#define PIRS_MAGIC01 'I'
#define PIRS_MAGIC02 'R'
#define PIRS_MAGIC03 'S'

#define LIVE_MAGIC00 'L'
#define LIVE_MAGIC01 'I'
#define LIVE_MAGIC02 'V'
#define LIVE_MAGIC03 'E'

#define PIRS_MAGIC 0x50495253 /* Big endian */
#define LIVE_MAGIC 0x4C495645 /* Big endian */

#define P_OFF_MAG		0x0000
#define P_OFF_HASHTABLE		0xb000
#define P_OFF_FILETABLE		0xc000
#define P_OFF_DATA		0xd000
#define P_OFF_HASHESOFHASHTABLE	0xb6000

/* Titles */
#define P_OFF_TIT_ENGLISH	0x0410
#define P_OFF_TIT_UNKNOWN0	0x0510
#define P_OFF_TIT_GERMAN	0x0610
#define P_OFF_TIT_FRENCH	0x0710
#define P_OFF_TIT_SPANISH	0x0810
#define P_OFF_TIT_ITALIAN	0x0910
#define P_OFF_TIT_UNKNOWN1	0x0a10
#define P_OFF_TIT_UNKNOWN2	0x0b10
#define P_OFF_TIT_PORTUGUESE	0x0c10

#define P_SIZ_TIT		256

/* Descriptions */
#define P_OFF_DESC_ENGLISH	0x0d10
#define P_OFF_DESC_UNKNOWN0	0x0e10
#define P_OFF_DESC_GERMAN	0x0f10
#define P_OFF_DESC_FRENCH	0x1010
#define P_OFF_DESC_SPANISH	0x1110
#define P_OFF_DESC_ITALIAN	0x1210
#define P_OFF_DESC_UNKNOWN1	0x1310
#define P_OFF_DESC_UNKNOWN2	0x1410
#define P_OFF_DESC_PORTUGUESE	0x1510

#define P_SIZ_DESC		256

#define P_OFF_PUBLISHER		0x1610

#define P_SIZ_PUBLISHER		256

#define P_SIZ_HASHTABLE		0x1000
#define P_SIZ_HASHDATASIZE	0x1000
#define P_SIZ_HASHESOFHASHTABLE	0x1000

typedef struct {
	const char *buf;
	unsigned int file_length;
	int fd;
} pirs_t;

pirs_t *pirs_load (const char *pathname);
int pirs_validate (pirs_t *pirs);
void pirs_unload (pirs_t *pirs);
pirs_object **pirs_get_all (pirs_t *pirs);

#endif
