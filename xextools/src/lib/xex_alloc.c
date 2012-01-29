#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swissknife.h"
#include "xex_datatypes.h"
#include "xex_alloc.h"

xex_dt_table *xex_alloc_table (unsigned int width, unsigned int nent, char *name)
{
	xex_dt_table *table = malloc (sizeof (xex_dt_table));

	table->width = width;
	table->nent = nent;
	table->name = strdup (name);
	table->entries = malloc (sizeof (xex_dt_table_ent *) * nent);
	table->header = malloc (sizeof (char *) * width);
	table->noheader = 0;

	return table;
}

xex_object *xex_alloc_object (xex_dt_object type, void *ptr)
{
	xex_object *object = malloc (sizeof (xex_object));

	object->type = type;
	object->ptr = ptr;

	return object;
}

xex_dt_table_ent *xex_alloc_table_ent (xex_dt_field type, unsigned long content)
{
	xex_dt_table_ent *ent = malloc (sizeof (xex_dt_table_ent));

	ent->type = type;
	ent->content = content;

	return ent;
}
