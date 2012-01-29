#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifndef __WIN32__
# include <sys/mman.h>
#endif
#include "util.h"
#include "pirs_datatypes.h"
#include "pirs_filetable.h"
#include "pirs_hashtables.h"
#include "pirs_titles.h"
#include "pirs_desc.h"
#include "pirs_main.h"


#ifdef __WIN32__
static int oflags = O_RDONLY | _O_BINARY;       /* Set the file mode to Binary */
#else
static int oflags = O_RDONLY;
#endif

pirs_object **pirs_get_all(pirs_t * pirs)
{
    pirs_object **objects;
    pirs_object **temp;
    int nhashes, j, i = 0;

    nhashes = pirs_count_hashtables(pirs);

    objects = malloc(sizeof(pirs_object) * (nhashes + 5));

    objects[i++] = pirs_get_titles(pirs);

    objects[i++] = pirs_get_descriptions(pirs);

    objects[i++] = pirs_get_filetable(pirs);

    objects[i++] = pirs_get_directories(pirs);

    temp = pirs_get_hashtables(pirs);

    for (j = 0; temp[j]; j++)
        objects[i++] = temp[j];

    objects[i] = NULL;

    return objects;
}

pirs_t *pirs_load(const char *pathname)
{
    struct stat statbuf;
    char *ptr;
    int tbw;
    pirs_t *pirs;

    tbw = 0;
    ptr = NULL;

    pirs = malloc(sizeof(pirs_t));

    pirs->buf = NULL;

    pirs->fd = open(pathname, oflags);

    if (pirs->fd < 0) {
        pirs_error("Couldn't open %s: %s\n", pathname, strerror(errno));
        goto cleanup;
    }

    fstat(pirs->fd, &statbuf);

    pirs->file_length = statbuf.st_size;

#ifdef __WIN32__
    pirs->buf = malloc(pirs->file_length);

    tbw = pirs->file_length;
    ptr = (char *) pirs->buf;

    do {
        int nb = read(pirs->fd, ptr, tbw);

        if (nb < 0) {
            if (nb == -1 && errno == EINTR)
                continue;

            close(pirs->fd);

            pirs_error("Couldn't read %s: %s\n", pathname, strerror(errno));

            goto cleanup;
        }

        tbw -= nb;
        ptr += nb;
    } while (tbw > 0);
#else
    pirs->buf = mmap(NULL, pirs->file_length, PROT_READ, MAP_PRIVATE, pirs->fd, 0);
#endif

    return pirs;

  cleanup:

    if (pirs->buf)
#ifdef __WIN32__
        free((void *) pirs->buf);
#else
        munmap((void *) pirs->buf, pirs->file_length);
#endif

    free(pirs);

    close(pirs->fd);

    return NULL;
}

void pirs_unload(pirs_t * pirs)
{
    if (pirs->buf)
#ifdef __WIN32__
        free((void *) pirs->buf);
#else
        munmap((void *) pirs->buf, pirs->file_length);
#endif

    free(pirs);

    close(pirs->fd);

}

int pirs_validate(pirs_t * pirs)
{
    DWORD magic = get_dword(pirs, P_OFF_MAG);

    if (magic != PIRS_MAGIC && magic != LIVE_MAGIC)
        return 0;

    return 1;
}
