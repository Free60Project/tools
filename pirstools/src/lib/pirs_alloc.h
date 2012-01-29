#ifndef PIRS_ALLOC_H
#define PIRS_ALLOC_H

#include "pirs_datatypes.h"

pirs_dt_table *pirs_alloc_table (unsigned int width, unsigned int nent, const char *name);
pirs_object *pirs_alloc_object (pirs_dt_object type, void *ptr);
pirs_dt_table_ent *pirs_alloc_table_ent (pirs_dt_field type, unsigned long content);
pirs_chunk *pirs_alloc_chunk (unsigned long start, unsigned long end);

#endif
