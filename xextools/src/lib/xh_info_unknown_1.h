#ifndef XH_INFO_UNKNOWN_1_H
#define XH_INFO_UNKNOWN_1_H

#include "xex_datatypes.h"

typedef enum {
	UNKNOWN_DWORDS_0 = 0x00,
	UNKNOWN_DWORDS_1 = 0x04
} xh_info_unknown_1_t;

struct xh_info_unknown_1_handler {
	xh_info_unknown_1_t id;
	xex_object *(* handler) (xex_t *xex, int offset, int size);
};

struct xh_info_unknown_1_handler *xh_info_unknown_1_handlers;

xex_object *xh_info_unknown_1_dwords_0 (xex_t *xex, int offset, int size);
xex_object *xh_info_unknown_1_dwords_1 (xex_t *xex, int offset, int size);

#endif
