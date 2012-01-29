#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <libpirs.h>

#ifdef __WIN32__
static int oflags = _O_BINARY;  /* Set the file mode to Binary */
#else
static int oflags = 0;
#endif

void usage(const char *pathname)
{
    printf("Usage: %s <pirs file> <output directory>\n", pathname);
}

void pirs_extract(pirs_t * pirs, const char *path, const char *filename, pirs_chunk * chunks)
{
    const char *ptr;
    int tbw, fd;

    printf("Extracting %s/%s\n", path, filename);

    if (mkdir(path, 0755) == -1 && errno != EEXIST) {
        printf("Couldn't mkdir(%s): %s\n", path, strerror(errno));
        return;
    }

    if (chdir(path) == -1) {
        printf("Couldn't chdir(%s): %s\n", path, strerror(errno));
        return;
    }


    if ((fd = open(filename, O_CREAT | O_WRONLY | oflags, 0644)) == -1) {
        printf("Couldn't open %s: %s\n", filename, strerror(errno));
        return;
    }

    while (chunks) {
        tbw = chunks->end - chunks->start;
        ptr = (const char *) chunks->start;

        do {
            int nb = write(fd, ptr, tbw);

            if (nb < 0) {
                if (nb == -1 && errno == EINTR)
                    continue;

                close(fd);

                printf("Couldn't write %s: %s\n", filename, strerror(errno));

                return;
            }

            tbw -= nb;
            ptr += nb;
        } while (tbw > 0);

        chunks = chunks->next;
    }

    close(fd);
}

int main(int argc, char **argv)
{
    pirs_t *pirs;
    char basepath[PATH_MAX];
    int n;

    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    pirs = pirs_load(argv[1]);

    if (pirs == NULL) {
        printf("Couldn't open the pirs file, exiting..\n");
        return 1;
    }

    /* Make sure the file is a PIRS File */

    if (!pirs_validate(pirs)) {
        printf("Wrong file type, not a PIRS file\n");
        goto cleanup;
    }

    if (mkdir(argv[2], 0755) == -1 && errno != EEXIST) {
        printf("Couldn't mkdir(%s): %s\n", argv[1], strerror(errno));
        goto cleanup;
    }

    if (chdir(argv[2]) == -1) {
        printf("Couldn't chdir(%s): %s\n", argv[1], strerror(errno));
        goto cleanup;
    }

    getcwd(basepath, sizeof(basepath));

    n = pirs_extract_all(pirs, basepath, pirs_extract);

    printf("Extracted %d files\n", n);

    pirs_unload(pirs);

    return 0;

  cleanup:
    pirs_unload(pirs);

    return 0;
}
