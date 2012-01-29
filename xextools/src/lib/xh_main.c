#include <stdlib.h>
#include "xex_datatypes.h"
#include "xex_main.h"
#include "swissknife.h"
#include "xh_main.h"

xex_object **xh_get_misc_info (xex_t *xex) {
	xex_object **objects = malloc (sizeof (xex_object *) * 6);
        xex_dt_table *table;
	DWORD dword;

	dword = get_dword (xex, X_OFF_VERSION);

        table = xex_alloc_table (1, 1, "Header: Flags");
        table->noheader = 1;
        table->entries[0] = xex_alloc_table_ent (Bits, dword);
        objects[0] = xex_alloc_object (Table, table); 

	dword = get_dword (xex, X_OFF_CODE);

	table = xex_alloc_table (1, 1, "Header: Code offset");
        table->noheader = 1;
        table->entries[0] = xex_alloc_table_ent (Hexa32, dword);
        objects[1] = xex_alloc_object (Table, table); 

	dword = get_dword (xex, X_OFF_RES0);

	table = xex_alloc_table (1, 1, "Header: Reserved 0");
        table->noheader = 1;
        table->entries[0] = xex_alloc_table_ent (Hexa32, dword);
        objects[2] = xex_alloc_object (Table, table); 

	dword = get_dword (xex, X_OFF_CERT);

	table = xex_alloc_table (1, 1, "Header: Certificate offset");
        table->noheader = 1;
        table->entries[0] = xex_alloc_table_ent (Hexa32, dword);
        objects[3] = xex_alloc_object (Table, table); 

	dword = get_dword (xex, X_OFF_NUMDW);

	table = xex_alloc_table (1, 1, "Header: Number of entries in the generic table");
        table->noheader = 1;
        table->entries[0] = xex_alloc_table_ent (Hexa32, dword);
        objects[4] = xex_alloc_object (Table, table); 

	objects[5] = NULL;

	return objects;
}
