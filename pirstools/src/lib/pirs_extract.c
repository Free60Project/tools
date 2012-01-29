#include <stdio.h>

#include "pirs_main.h"
#include "pirs_datatypes.h"
#include "pirs_filetable.h"
#include "pirs_extract.h"
#include "pirs_hashtables.h"
#include "pirs_alloc.h"
#include "util.h"

pirs_chunk *pirs_extract_chunks(pirs_t * pirs, const char *data, unsigned int offset, unsigned int size)
{
    pirs_chunk *hash_chunks = pirs_get_hashtables_offsets(pirs);
    pirs_chunk *hash_tmp;
    pirs_chunk *data_chunks;
    pirs_chunk *data_tmp;

    hash_tmp = hash_chunks;

    data_chunks = data_tmp = NULL;

    while (1) {
        /* Look if it overlaps */
        while (hash_tmp) {
            if (hash_tmp->start > offset && hash_tmp->start < offset + size) {  /* Hash table start inside the data */
                if (data_tmp == NULL) {
                    data_tmp =
                        pirs_alloc_chunk((unsigned long) pirs->buf + offset,
                                         (unsigned long) pirs->buf + hash_tmp->start);
                    data_chunks = data_tmp;
                } else {
                    data_tmp->next =
                        pirs_alloc_chunk((unsigned long) pirs->buf + offset,
                                         (unsigned long) pirs->buf + hash_tmp->start);
                    data_tmp = data_tmp->next;
                }

                size -= hash_tmp->start - offset;
                offset = hash_tmp->end;

                break;
            }

            hash_tmp = hash_tmp->next;
        }

	/* Last chunk */
        if (hash_tmp == NULL) {
            if (data_tmp == NULL) {
                data_tmp =
                    pirs_alloc_chunk((unsigned long) pirs->buf + offset, (unsigned long) pirs->buf + (offset + size));
                data_chunks = data_tmp;
            } else {
                data_tmp->next =
                    pirs_alloc_chunk((unsigned long) pirs->buf + offset, (unsigned long) pirs->buf + (offset + size));
            }

            break;
        }
    }

    return data_chunks;
}


int pirs_extract_all(pirs_t * pirs, const char *basepath, pirs_extract_callback callback)
{
    pirs_object *obj = pirs_get_filetable(pirs);
    pirs_object *dirs = pirs_get_directories(pirs);
    int i, j;

    for (i = j = 0; i < obj->table->nent / obj->table->width; i++) {
        const char *filetype, *filename, *data;
        const char *pirs_dir, *output_dir;
        unsigned long size;
        unsigned long offset;
        pirs_chunk *chunks;

        filename = (const char *) obj->table->entries[i * obj->table->width]->content;
        filetype = (const char *) obj->table->entries[i * obj->table->width + 1]->content;

        if (strcmp(filetype, "File") != 0)
            continue;

        pirs_dir =
            pirs_resolve_directory(dirs->table,
                                   (unsigned long) obj->table->entries[i * obj->table->width + 4]->content);

	if (strcmp (pirs_dir, "/") == 0)
		output_dir = strdup (basepath);
	else
		output_dir = str_concatenate(basepath, pirs_dir, 0);

        offset = obj->table->entries[i * obj->table->width + 3]->content;
        size = (unsigned long) obj->table->entries[i * obj->table->width + 5]->content;

        data = pirs->buf + offset;

        chunks = pirs_extract_chunks(pirs, data, offset, size);

        callback(pirs, output_dir, filename, chunks);

        free(output_dir);
        j++;
    }

    return j;
}
