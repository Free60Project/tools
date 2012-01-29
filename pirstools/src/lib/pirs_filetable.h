#ifndef PIRS_FILETABLET_H
#define PIRS_FILETABLET_H

#include <stdint.h>

pirs_object *pirs_get_filetable (pirs_t *pirs);
pirs_object *pirs_get_directories (pirs_t *pirs);
const char *pirs_resolve_directory (pirs_dt_table *dirs, unsigned long id);

struct __pears_file_ent {
	char filename[40];		/* Filename			*/
	uint32_t unknown0;		/* Unknown 0			*/
	struct {
		uint8_t unknown0;	/* Offset: Unknown 0		*/
		uint8_t unknown1;	/* Offset: Unknown 1		*/
		uint8_t unknown2;	/* Offset: Unknown 2		*/
		uint8_t base0:4;	/* Offset multiplication factor	*/
		uint8_t base1:4;	/* Base offset (Upper 4 bits)	*/
		uint8_t base2;		/* Base offset (Lower 8 bits)	*/
	} offset;
	uint8_t unknown6;		/* Unknown 6			*/
	uint16_t diridx;		/* Index in the Directory tree	*/
	uint32_t size;			/* Size of the file		*/
	uint32_t unknown4;		/* Unknown 4			*/
	uint32_t unknown5;		/* Unknown 5			*/
};

#endif
