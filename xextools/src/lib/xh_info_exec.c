#include <stdio.h>
#include <string.h>
#include "swissknife.h"
#include "xh_info_exec.h"
#include "xex_datatypes.h"

static struct xh_info_exec_handler _xh_info_exec_handlers[] = {
	{ EXEC_ORIGINAL_BASE_ADDR,	xh_info_exec_original_base_addr },
	{ EXEC_ENTRY_POINT,		xh_info_exec_entry_point },
	{ EXEC_LOAD_TEXT_ADDR,		xh_info_exec_load_text_addr },
	{ EXEC_UNKNOWN_ADDRESS_TABLE,	xh_info_exec_unknown_address_table },
	{ EXEC_ID,			xh_info_exec_image_timestamp },
	{ EXEC_ORIGINAL_PE_FILENAME,	xh_info_exec_original_pe_filename },
	{ 0, 0 }
};

struct xh_info_exec_handler *xh_info_exec_handlers = _xh_info_exec_handlers;

xex_object *xh_info_exec_original_base_addr (xex_t *xex, int offset, int size) {
        xex_dt_table *table;

        table = xex_alloc_table (1, 1, "Exec: Original base address");

        table->noheader = 1;

        table->entries[0] = xex_alloc_table_ent (Hexa32, offset);

        return xex_alloc_object (Table, table);
}

xex_object *xh_info_exec_entry_point (xex_t *xex, int offset, int size) {
        xex_dt_table *table;

        table = xex_alloc_table (1, 1, "Exec: Entry point");

        table->noheader = 1;

        table->entries[0] = xex_alloc_table_ent (Hexa32, offset);

        return xex_alloc_object (Table, table);
}

xex_object *xh_info_exec_load_text_addr (xex_t *xex, int offset, int size) {
        xex_dt_table *table;

        table = xex_alloc_table (1, 1, "Exec: Load text address");

        table->noheader = 1;

        table->entries[0] = xex_alloc_table_ent (Hexa32, offset);

        return xex_alloc_object (Table, table);
}

xex_object *xh_info_exec_unknown_address_table (xex_t *xex, int offset, int size) {
        int real_size = (size == 0xFF ? get_dword (xex, offset) : size);
        xex_dt_table *table; 
        int i;  

        real_size -= 4;
        offset += 4;

        table = xex_alloc_table (4, real_size / 4, "Exec: Unknown address table -- Relocation table");

        table->noheader = 1;

        for (i = 0; i < real_size / 4; i++) {
                DWORD dword = get_dword (xex, offset + i*4); 
                table->entries[i] = xex_alloc_table_ent (Hexa32, dword); 
        }       

        return xex_alloc_object (Table, table); 
}

xex_object *xh_info_exec_image_timestamp (xex_t *xex, int offset, int size) { 
	xex_dt_table *table;

	DWORD dword = get_dword (xex, offset + 4);

        table = xex_alloc_table (1, 1, "Exec: Image timestamp");

        table->noheader = 1;

        table->entries[0] = xex_alloc_table_ent (Hexa32, dword);

        return xex_alloc_object (Table, table);
}

xex_object *xh_info_exec_original_pe_filename (xex_t *xex, int offset, int size) { 
        xex_dt_table *table; 
        char *orig_filename;

        orig_filename = strdup (xex->buf + offset + 4);

        table = xex_alloc_table (1, 1, "Exec: Original PE Filename");

        table->noheader = 1;

        table->entries[0] = xex_alloc_table_ent (String, (unsigned long)orig_filename);

        return xex_alloc_object (Table, table); 
}

