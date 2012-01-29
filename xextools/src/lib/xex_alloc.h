#ifndef XEX_ALLOC_H
#define XEX_ALLOC_H

#include "xex_datatypes.h"

xex_dt_table *xex_alloc_table (unsigned int width, unsigned int nent, char *name);
xex_object *xex_alloc_object (xex_dt_object type, void *ptr);
xex_dt_table_ent *xex_alloc_table_ent (xex_dt_field type, unsigned long content);

#endif
