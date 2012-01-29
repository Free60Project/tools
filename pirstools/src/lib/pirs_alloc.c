#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "pirs_datatypes.h"
#include "pirs_alloc.h"

pirs_dt_table *
pirs_alloc_table (unsigned int width, unsigned int nent, const char *name)
{
  pirs_dt_table *table = malloc (sizeof (pirs_dt_table));

  table->width = width;
  table->nent = nent;
  table->name = strdup (name);
  table->entries = malloc (sizeof (pirs_dt_table_ent *) * nent);
  table->header = malloc (sizeof (char *) * width);
  table->noheader = 0;

  return table;
}

pirs_object *
pirs_alloc_object (pirs_dt_object type, void *ptr)
{
  pirs_object *object = malloc (sizeof (pirs_object));

  object->type = type;
  object->ptr = ptr;

  return object;
}

pirs_dt_table_ent *
pirs_alloc_table_ent (pirs_dt_field type, unsigned long content)
{
  pirs_dt_table_ent *ent = malloc (sizeof (pirs_dt_table_ent));

  ent->type = type;
  ent->content = content;

  return ent;
}

pirs_chunk *
pirs_alloc_chunk (unsigned long start, unsigned long end)
{
  pirs_chunk *chunk = malloc (sizeof (pirs_chunk));

  chunk->start = start;
  chunk->end = end;
  chunk->next = NULL;

  return chunk;
}
