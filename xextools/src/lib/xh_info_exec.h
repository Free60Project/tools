#ifndef XH_INFO_EXEC_H
#define XH_INFO_EXEC_H

#include "xex_datatypes.h"

typedef enum {
	EXEC_ORIGINAL_BASE_ADDR = 0x00,
	EXEC_ENTRY_POINT = 0x01,
	EXEC_LOAD_TEXT_ADDR = 0x02,
	EXEC_UNKNOWN_ADDRESS_TABLE = 0x03,
	EXEC_ID = 0x80,
	EXEC_ORIGINAL_PE_FILENAME = 0x83
} xh_info_exec_t;

struct xh_info_exec_handler {
	xh_info_exec_t id;
	xex_object *(* handler) (xex_t *xex, int offset, int size);
};

struct xh_info_exec_handler *xh_info_exec_handlers;

xex_object *xh_info_exec_original_base_addr (xex_t *xex, int offset, int size);
xex_object *xh_info_exec_entry_point (xex_t *xex, int offset, int size);
xex_object *xh_info_exec_load_text_addr (xex_t *xex, int offset, int size);
xex_object *xh_info_exec_unknown_address_table (xex_t *xex, int offset, int size);
xex_object *xh_info_exec_image_timestamp (xex_t *xex, int offset, int size);
xex_object *xh_info_exec_original_pe_filename (xex_t *xex, int offset, int size);

#endif
