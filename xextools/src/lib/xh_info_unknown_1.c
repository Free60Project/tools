#include <stdio.h>
#include "swissknife.h"
#include "xh_info_unknown_1.h"
#include "xex_datatypes.h"

struct xh_info_unknown_1_handler _xh_info_unknown_1_handlers[] = {
	{ UNKNOWN_DWORDS_0,		xh_info_unknown_1_dwords_0 },
	{ UNKNOWN_DWORDS_1,		xh_info_unknown_1_dwords_1 },
	{ 0 }
};

struct xh_info_unknown_1_handler *xh_info_unknown_1_handlers = _xh_info_unknown_1_handlers;

xex_object *xh_info_unknown_1_dwords_0 (xex_t *xex, int offset, int size) {
        int real_size = (size == 0xFF ? get_dword (xex, offset) : size);
        xex_dt_table *table;
        int i;

        table = xex_alloc_table (4, real_size, "Unknown 1: Dwords 0");

        table->noheader = 1;

        for (i = 0; i < real_size; i++) {
                DWORD dword = get_dword (xex, offset + i*4);
                table->entries[i] = xex_alloc_table_ent (Hexa32, dword);
        }

        return xex_alloc_object (Table, table);
}

xex_object *xh_info_unknown_1_dwords_1 (xex_t *xex, int offset, int size) {
	int real_size = (size == 0xFF ? get_dword (xex, offset) : size);
        xex_dt_table *table;
        int i;

        table = xex_alloc_table (4, real_size, "Unknown 1: Dwords 1");

        table->noheader = 1;

        for (i = 0; i < real_size; i++) {
                DWORD dword = get_dword (xex, offset + i*4);
                table->entries[i] = xex_alloc_table_ent (Hexa32, dword);
        }

        return xex_alloc_object (Table, table);
}
