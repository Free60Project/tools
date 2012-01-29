#ifndef XH_INFO_LOAD_H
#define XH_INFO_LOAD_H

#include "xex_datatypes.h"

typedef enum {
	LOAD_LIBRARIES = 0x00,
	LOAD_UNKNOWN_DWORDS_0 = 0x01,
	LOAD_UNKNOWN_DWORDS_1 = 0x02
} xh_info_load_t;

struct xh_info_load_handler {
	xh_info_load_t id;
	xex_object *(* handler) (xex_t *xex, int offset, int size);
};

struct xh_info_load_handler *xh_info_load_handlers;

xex_object *xh_info_load_libraries (xex_t *xex, int offset, int size);
xex_object *xh_info_load_dwords_0 (xex_t *xex, int offset, int size);
xex_object *xh_info_load_dwords_1 (xex_t *xex, int offset, int size);

typedef struct {
        unsigned int build:16;
        unsigned int unk0:8; 
        unsigned int micro:8;
} xh_library_version;

#endif
