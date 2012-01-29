#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pirs_main.h"
#include "pirs_datatypes.h"
#include "pirs_alloc.h"
#include "pirs_hashtables.h"
#include "util.h"
#include "sha1.h"

static int
pirs_checksum (uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t E, const char *data, unsigned int len)
{
  uint32_t calculated[SHA1HashSize / 4];

  A = bs32 (A);
  B = bs32 (B);
  C = bs32 (C);
  D = bs32 (D);
  E = bs32 (E);

  SHA1Context sha;

  SHA1Reset (&sha);
  SHA1Input (&sha, data, len);
  SHA1ResultInt32 (&sha, calculated);

  if (A != calculated[0] || B != calculated[1] || C != calculated[2] || D != calculated[3] || E != calculated[4])
    return 0;

  return 1;
}

static unsigned int
pirs_count_hash_ent (pirs_t * pirs, unsigned long offset)
{
  int max = P_SIZ_HASHTABLE / 38;
  struct __pears_hash_ent *hash;
  int hash_off = 0;
  int i;

  for (i = 0; i < max; i++)
    {
      hash = (struct __pears_hash_ent *) &(pirs->buf[offset + hash_off]);

      if (hash->A == 0L && hash->B == 0L && hash->C == 0L && hash->D == 0L && hash->E == 0L)
        break;

      hash_off += sizeof (struct __pears_hash_ent);
    }

  return i;
}

pirs_object *
pirs_get_hashtable (pirs_t * pirs, unsigned long offset, int hashes_of_hashtables)
{
  struct __pears_hash_ent *hash;
  const char *title;
  const char *data;
  int hash_off;
  int data_off;
  int height;
  int i, j;

  pirs_dt_table *table;

  height = pirs_count_hash_ent (pirs, offset);

  title = hashes_of_hashtables ? "Hash tables of Hash tables" : "Hash table";

  table = pirs_alloc_table (7, height * 7, title);

  table->header[0] = "unknown0";
  table->header[1] = "SHA-1 A";
  table->header[2] = "SHA-1 B";
  table->header[3] = "SHA-1 C";
  table->header[4] = "SHA-1 D";
  table->header[5] = "SHA-1 E";
  table->header[6] = "Verified";

  i = j = 0;
  hash_off = 0;
  data_off = 0;

  while (j < height)
    {
      const char *result;

      hash = (struct __pears_hash_ent *) &(pirs->buf[offset + hash_off]);

      if (hashes_of_hashtables)
        {
          unsigned int off = P_OFF_HASHTABLE + (P_SIZ_HASHTABLE + P_SIZ_HASHDATASIZE * 170) * j;

          off += j > 0 ? P_SIZ_HASHESOFHASHTABLE : 0;
          data = (const char *) &(pirs->buf[off]);
        }
      else
        data = (const char *) &(pirs->buf[offset + P_SIZ_HASHTABLE + data_off]);

      if (pirs_checksum (hash->A, hash->B, hash->C, hash->D, hash->E, data, 4096))
        result = "OK";
      else
        result = "BAD";

      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (hash->unknown0));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (hash->A));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (hash->B));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (hash->C));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (hash->D));
      table->entries[i++] = pirs_alloc_table_ent (Hexa32, bs32 (hash->E));
      table->entries[i++] = pirs_alloc_table_ent (String, (unsigned long) strdup (result));

      data_off += 4096;
      hash_off += sizeof (struct __pears_hash_ent);

      j++;
    }

  return pirs_alloc_object (Table, table);
}

unsigned int
pirs_count_hashtables (pirs_t * pirs)
{
  int nhashes = (pirs->file_length - P_OFF_FILETABLE) / (P_SIZ_HASHTABLE + P_SIZ_HASHDATASIZE * 170);

  if (nhashes == 0)
    nhashes = 1;
  else
    nhashes += 2;

  return nhashes;
}

pirs_chunk *
pirs_get_hashtables_offsets (pirs_t * pirs)
{
  int n = pirs_count_hashtables (pirs);
  pirs_chunk *chunks, *tmp;
  long offset;
  int i;

  chunks = malloc (sizeof (pirs_chunk));
  tmp = chunks;

  for (i = 0; i < n; i++)
    {
      if (i == 0)
        {
          tmp->start = P_OFF_HASHTABLE;
          tmp->end = tmp->start + P_SIZ_HASHTABLE;
        }
      else if (i == 1)
        {
          tmp->next = malloc (sizeof (pirs_chunk));
          tmp = tmp->next;
          tmp->start = P_OFF_HASHESOFHASHTABLE;
          tmp->end = tmp->start + P_SIZ_HASHTABLE + P_SIZ_HASHESOFHASHTABLE;
        }
      else
        {
          tmp->next = malloc (sizeof (pirs_chunk));
          tmp = tmp->next;
          tmp->start = offset + P_SIZ_HASHDATASIZE * 170;
          tmp->end = tmp->start + P_SIZ_HASHTABLE;
        }

      offset = tmp->end;
    }

  tmp->next = NULL;

  return chunks;
}

pirs_object **
pirs_get_hashtables (pirs_t * pirs)
{
  pirs_object **objects;
  int nobjs, i;
  int offset = P_OFF_HASHTABLE;

  nobjs = pirs_count_hashtables (pirs);

  objects = malloc (sizeof (pirs_object *) * (nobjs + 1));

  for (i = 0; i < nobjs; i++)
    {
      if (offset == P_OFF_HASHESOFHASHTABLE)
        {
          objects[i] = pirs_get_hashtable (pirs, offset, 1);
          offset += P_SIZ_HASHESOFHASHTABLE;
        }
      else
        {
          objects[i] = pirs_get_hashtable (pirs, offset, 0);
          offset += (P_SIZ_HASHTABLE + P_SIZ_HASHDATASIZE * 170);
        }
    }

  objects[i] = NULL;

  return objects;
}
