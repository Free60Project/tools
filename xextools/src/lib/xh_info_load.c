#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "swissknife.h"
#include "xh_info_load.h"
#include "xex_datatypes.h"

static struct xh_info_load_handler _xh_info_load_handlers[] = {
	{ LOAD_LIBRARIES,		xh_info_load_libraries },
	{ LOAD_UNKNOWN_DWORDS_0,	xh_info_load_dwords_0 },
	{ LOAD_UNKNOWN_DWORDS_1,	xh_info_load_dwords_1 },
	{ 0 }
};

struct xh_info_load_handler *xh_info_load_handlers = _xh_info_load_handlers;

xex_object *xh_info_load_libraries (xex_t *xex, int offset, int size) {
	int real_size = (size == 0xFF ? get_dword (xex, offset) : size);
        int asczl = 0;
        int height, i, j;

	xh_library_version ver;
        xex_dt_table *table; 

	asczl = 0;

        offset += sizeof (DWORD);
        real_size -= sizeof (DWORD);

        /* Calculate the height of the table */
        for (i = j = 0; i < real_size; i += (asczl + 8)) { 
                asczl = 8;
                j++;    
        }       

        height = j;

	table = xex_alloc_table (3, height * 3, "Load: library versions");

        table->header[0] = strdup ("flags");
        table->header[1] = strdup ("version");
        table->header[2] = strdup ("library");

	for (i = j = 0; i < real_size; i += asczl + 8) {
		char *version = malloc (40);
		DWORD flags;
		char *ascz;

		ascz = malloc(9);
		memcpy (ascz, xex->buf + offset + i, 8);
		ascz[8] = '\0';
		asczl = 8;

		flags = get_dword (xex, offset + i + asczl);
		*(DWORD *)&ver = bs32(get_dword (xex, offset + i + asczl + 4));

		/* Swap bytes */
		ver.build = bs16(ver.build);

		snprintf (version, 40, "%#x.%d.%-2d", ver.unk0, ver.build, ver.micro);

		table->entries[j++] = xex_alloc_table_ent (Hexa32, flags);
                table->entries[j++] = xex_alloc_table_ent (String, (unsigned long)version);
                table->entries[j++] = xex_alloc_table_ent (String, (unsigned long)ascz);
	}

        return xex_alloc_object (Table, table); 
}

xex_object *xh_info_load_dwords_0 (xex_t *xex, int offset, int size) {
        int real_size = (size == 0xFF ? get_dword (xex, offset) : size);
        xex_dt_table *table;
        int i;

	/* FIXME: Unknown size */

        table = xex_alloc_table (4, real_size, "Load: Dwords 0");

        table->noheader = 1;

        for (i = 0; i < real_size; i++) {
                DWORD dword = get_dword (xex, offset + i*4);
                table->entries[i] = xex_alloc_table_ent (Hexa32, dword);
        }

        return xex_alloc_object (Table, table);
}

xex_object *xh_info_load_dwords_1 (xex_t *xex, int offset, int size) {
        xex_dt_table *table; 

        table = xex_alloc_table (1, 1, "Load: Dwords 1");

        table->noheader = 1;

        table->entries[0] = xex_alloc_table_ent (Hexa32, offset);

        return xex_alloc_object (Table, table); 
}
