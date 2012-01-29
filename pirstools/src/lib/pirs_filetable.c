#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pirs_main.h"
#include "pirs_datatypes.h"
#include "pirs_filetable.h"
#include "pirs_alloc.h"
#include "util.h"

pirs_object *
pirs_get_filetable (pirs_t * pirs)
{
  struct __pears_file_ent *file_entry;
  int offset;
  int height;
  int ndirs;
  int i;

  pirs_dt_table *table;

  offset = 0;
  ndirs = 0;

  for (height = 0;; height++)
    {
      file_entry = (struct __pears_file_ent *) &(pirs->buf[P_OFF_FILETABLE + offset]);

      if (file_entry->filename[0] == '\0')
        break;

      offset += sizeof (struct __pears_file_ent);
    }


  table = pirs_alloc_table (8, height * 8, "File table");

  table->header[0] = "Filename";
  table->header[1] = "Type";
  table->header[2] = "unknown0";
  table->header[3] = "Offset";
  table->header[4] = "DirIndex";
  table->header[5] = "Size";
  table->header[6] = "unknown4";
  table->header[7] = "unknown5";

  i = 0;
  offset = 0;

  while (1)
    {
      int a, off = 0;
      int base;
      const char *type;

      file_entry = (struct __pears_file_ent *) &(pirs->buf[P_OFF_FILETABLE + offset]);

      if (file_entry->filename[0] == '\0')
        break;

      base = file_entry->offset.base0 + 0x10 * (file_entry->offset.base1 + file_entry->offset.base2 * 0x10);

      if (base == 0 && file_entry->size == 0)
        {
          type = "Dir";
        }
      else
        {
          type = "File";
        }

      off = P_OFF_FILETABLE + 0x1000 * base;

      a = base / 170;           /* Calculate the number of hash tables before the offset */

      if (a)                    /* Add the size of these hash tables if there's at least one. */
        off += (a + 1) * 0x1000;

      table->entries[i++] = pirs_alloc_table_ent (String, (unsigned long) strdup (file_entry->filename));
      table->entries[i++] = pirs_alloc_table_ent (String, (unsigned long) strdup (type));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (file_entry->unknown0));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, off);
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs16 (file_entry->diridx));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (file_entry->size));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (file_entry->unknown4));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (file_entry->unknown5));

      offset += sizeof (struct __pears_file_ent);
    }

  return pirs_alloc_object (Table, table);
}

/* FIXME: *CLEANME* Yuck! */

pirs_object *
pirs_get_directories (pirs_t * pirs)
{
  struct __pears_file_ent *file_entry;
  pirs_dt_table *dirs;
  int height;
  int offset;
  int ndirs;
  int i;

  offset = 0;
  ndirs = 0;

  for (height = 0;; height++)
    {
      file_entry = (struct __pears_file_ent *) &(pirs->buf[P_OFF_FILETABLE + offset]);

      if (file_entry->filename[0] == '\0')
        break;

      if (file_entry->offset.base0 == 0 && file_entry->offset.base1 == 0 &&
          file_entry->offset.base2 == 0 && file_entry->size == 0)
        {
          ndirs++;
        }

      offset += sizeof (struct __pears_file_ent);
    }

  ndirs++;

  dirs = pirs_alloc_table (2, ndirs * 2, "Directories");

  dirs->header[0] = "DirIndex";
  dirs->header[1] = "Path";

  i = 0;
  offset = 0;

  dirs->entries[i++] = pirs_alloc_table_ent (Hexa32, 0xffff);
  dirs->entries[i++] = pirs_alloc_table_ent (String, (unsigned long) strdup ("/"));

  while (1)
    {
      int off = 0;
      int base;
      int j;
      const char *type;

      file_entry = (struct __pears_file_ent *) &(pirs->buf[P_OFF_FILETABLE + offset]);

      if (file_entry->filename[0] == '\0')
        break;

      base = file_entry->offset.base0 + 0x10 * (file_entry->offset.base1 + file_entry->offset.base2 * 0x10);

      if (base == 0 && file_entry->size == 0)
        {
          const char *str = NULL;
          type = "Dir";

          for (j = 0; j < i; j += 2)
            {
              if (dirs->entries[j]->content == bs16 (file_entry->diridx))
                {
                  const char *base = (const char *) dirs->entries[j + 1]->content;
                  if (bs16 (file_entry->diridx) == 0xffff)
                    str = str_concatenate (base, file_entry->filename, 0);
                  else
                    str = str_concatenate (base, "/", file_entry->filename, 0);
                  break;
                }
            }

          if (str == NULL)
            str = strdup_printf ("[unresolvable path %d]\n", i / 2);

          dirs->entries[i++] =
            pirs_alloc_table_ent (Hexa32, offset == 0 ? 0 : offset / sizeof (struct __pears_file_ent));
          dirs->entries[i++] = pirs_alloc_table_ent (String, (unsigned long) str);
        }
      else
        {
          type = "File";
        }

      offset += sizeof (struct __pears_file_ent);
    }

  return pirs_alloc_object (Table, dirs);
}

const char *
pirs_resolve_directory (pirs_dt_table * dirs, unsigned long id)
{
  const char *ret = NULL;
  int i;

  for (i = 0; i < dirs->nent; i += 2)
    {
      if (id == dirs->entries[i]->content)
        {
          ret = (const char *) dirs->entries[i + 1]->content;
          break;
        }
    }

  return ret;
}
