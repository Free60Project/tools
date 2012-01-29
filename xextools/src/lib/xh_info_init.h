#ifndef XH_INFO_INIT_H
#define XH_INFO_INIT_H

#include "xex_datatypes.h"

typedef enum {
	INIT_CONTENT_MAPPING = 0x02,
	INIT_OFFSET_OR_CHECKSUMS = 0x03,
	INIT_EXECUTABLE_PATH = 0x80
} xh_info_init_t;

struct xh_info_init_handler {
	xh_info_init_t id;
	xex_object *(* handler) (xex_t *xex, int offset, int size);
};

struct xh_info_init_handler *xh_info_init_handlers;

xex_object *xh_info_init_content_mapping (xex_t *xex, int offset, int size);
xex_object *xh_info_init_offset_or_checksums (xex_t *xex, int offset, int size);
xex_object *xh_info_init_executable_path (xex_t *xex, int offset, int size);

#endif
