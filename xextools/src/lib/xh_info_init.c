#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "swissknife.h"
#include "xh_info_init.h"
#include "xex_datatypes.h"

static struct xh_info_init_handler _xh_info_init_handlers[] = {
	{ INIT_CONTENT_MAPPING, xh_info_init_content_mapping },
	{ INIT_OFFSET_OR_CHECKSUMS, xh_info_init_offset_or_checksums },
	{ INIT_EXECUTABLE_PATH, xh_info_init_executable_path },
	{ 0 }
};

struct xh_info_init_handler *xh_info_init_handlers = _xh_info_init_handlers;

xex_object *xh_info_init_content_mapping (xex_t *xex, int offset, int size) {
	int real_size = (size == 0xFF ? get_dword (xex, offset) : size);
	int asczl = 0;
	int height, i, j, k;

	xex_dt_table *table;

	offset += sizeof (DWORD);
	real_size -= sizeof (DWORD);

	/* Calculate the height of the table */
	for (i = j = 0; i < real_size; i += (asczl + 8)) {
		asczl = 8;
		j++;
	}

	height = j;

	table = xex_alloc_table (3, height * 3, "Init: Resource content mapping");

	table->header[0] = strdup ("base address");
	table->header[1] = strdup ("size");
	table->header[2] = strdup ("name");

	for (i = j = 0; i < real_size; i += asczl + 8) {
		DWORD size, base;
		const char *ptr = xex->buf + offset + i;
		char *ascz;
		
		for (k = 0; k < 8; k++)
			if (ptr[k] == '\0')
				break;

		asczl = 8;
		ascz = malloc (asczl + 1);
		memcpy (ascz, ptr, asczl);
		ascz[asczl] = '\0';

		base = get_dword (xex, offset + i + asczl);
		size = get_dword (xex, offset + i + asczl + 4);

		table->entries[j++] = xex_alloc_table_ent (Hexa32, base);
		table->entries[j++] = xex_alloc_table_ent (Hexa32, size);
		table->entries[j++] = xex_alloc_table_ent (String, (unsigned long)ascz);
	}

	return xex_alloc_object (Table, table);
}

xex_object *xh_info_init_offset_or_checksums (xex_t *xex, int offset, int size) {
	int real_size = (size == 0xFF ? get_dword (xex, offset) : size);
	xex_dt_table *table;
	int i;

	real_size -= 4;
	offset += 4;

	table = xex_alloc_table (4, real_size / 4, "Init: Offset or checksum table");

	table->noheader = 1;

	for (i = 0; i < real_size / 4; i++) {
		DWORD dword = get_dword (xex, offset + i*4);
		table->entries[i] = xex_alloc_table_ent (Hexa32, dword);
	}

	return xex_alloc_object (Table, table);
}

xex_object *xh_info_init_executable_path (xex_t *xex, int offset, int size) {
	xex_dt_table *table;
	char *pathname;

	pathname = strdup (xex->buf + offset + 4);

	table = xex_alloc_table (1, 1, "Init: Bound pathname");

	table->noheader = 1;

	table->entries[0] = xex_alloc_table_ent (String, (unsigned long)pathname);

	return xex_alloc_object (Table, table);
}
