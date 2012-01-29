#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swissknife.h"
#include "xh_info.h"
#include "xh_info_exec.h"
#include "xh_info_init.h"
#include "xh_info_load.h"
#include "xh_info_unknown_1.h"
#include "xex_main.h"
#include "xex_datatypes.h"

xex_object *xh_info_execute_entry (xex_t *xex, DWORD flags, DWORD value)
{
	xh_info_t info_ent = (flags & 0x00FF0000) >> 16;
	int id = (flags & 0x0000FF00) >> 8;
	int size = (flags & 0x00000000FF);
	int executed = 0, found = 0;

	xex_object *ret = NULL;

	switch (info_ent) {
		case INFO_INIT:
			xh_info_execute (xh_info_init_handlers, id, xex, value, size, executed, ret);
			break;
		case INFO_EXEC:
			xh_info_execute (xh_info_exec_handlers, id, xex, value, size, executed, ret);
			break;
		case INFO_LOAD:
			xh_info_execute (xh_info_load_handlers, id, xex, value, size, executed, ret);
			break;
		case INFO_UNKNOWN_0:
			xh_info_execute (xh_info_unknown_1_handlers, id, xex, value, size, executed, ret);
			break;
		default:
			if (!found) {
				xex_error ("Unhandled xh info entry type: %x\n", info_ent);
				//abort ();
			}
	}
	
	if (!executed)
		xex_warning ("No handler for info entry type: %x and id %x\n", info_ent, id);

	return ret;
}

xex_object **xh_info_load_generic (xex_t *xex, xex_dt_table *table) {
	xex_object **objects;
	DWORD flags, value;
	int i, j;

	objects = malloc (sizeof (xex_object *) * ((table->nent / table->width) + 1));

        for (i = j = 0; i < (table->nent / table->width); i++) {
		xex_object *obj;

		flags = table->entries[i * table->width + 0]->content;
		value = table->entries[i * table->width + 1]->content;

		obj = xh_info_execute_entry (xex, flags, value);

		if (obj != NULL)
			objects[j++] = obj;
        }

	objects[j] = NULL;

	return objects;
}


xex_object *xh_read_generic_table (xex_t *xex) { 
        xex_dt_table *table; 
        int i, j, nent;

        nent = get_dword (xex, X_OFF_NUMDW);

        table = xex_alloc_table (3, nent * 3, "Generic table");

        table->width = 3;
        table->nent = nent * table->width;

        table->header[0] = strdup ("flags");
        table->header[1] = strdup ("value");
        table->header[2] = strdup ("flags (bits)");

        i = 1;  
        j = 0;  

        while (i < nent * 2) {
                DWORD flags, value;

                flags = get_dword (xex, X_OFF_NUMDW + i++ * 4); 
                value = get_dword (xex, X_OFF_NUMDW + i++ * 4); 

                table->entries[j++] = xex_alloc_table_ent (Hexa32, flags); 
                table->entries[j++] = xex_alloc_table_ent (Hexa32, value); 
                table->entries[j++] = xex_alloc_table_ent (Bits, flags); 
        }       

        return xex_alloc_object (Table, table); 
}
