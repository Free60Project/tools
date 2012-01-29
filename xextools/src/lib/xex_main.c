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
#include "swissknife.h"
#include "xex_main.h"


#ifdef __WIN32__
static int oflags = O_RDONLY|_O_BINARY; /* Set the file mode to Binary */
#else
static int oflags = O_RDONLY;
#endif

xex_t *xex_load (const char *pathname)
{
	struct stat statbuf;
	char *ptr;
	int tbw;
	xex_t *xex;

	tbw = 0;
	ptr = NULL;

	xex = malloc (sizeof (xex_t));

	xex->buf = NULL;

	xex->fd = open (pathname, oflags);

	if (xex->fd < 0) {
		xex_error ("Couldn't open %s: %s\n", pathname, strerror (errno));
		goto cleanup;
	}
	
	fstat (xex->fd, &statbuf);

	xex->file_length = statbuf.st_size;

#ifdef __WIN32__
	xex->buf = malloc (xex->file_length);

	tbw = xex->file_length;
	ptr = (char *)xex->buf;

	do {
		int nb = read (xex->fd, ptr, tbw);

		if (nb < 0)
		{
			if (nb == -1 && errno == EINTR)
				continue;

			close (xex->fd);

			xex_error ("Couldn't read %s: %s\n", pathname, strerror (errno));

			goto cleanup;
		}

		tbw -= nb;
		ptr += nb;
	} while (tbw > 0);
#else
	xex->buf = mmap (NULL, xex->file_length, PROT_READ, MAP_PRIVATE, xex->fd, 0);
#endif

	return xex;

cleanup:

	if (xex->buf)
#ifdef __WIN32__
		free ((void *)xex->buf);
#else
		munmap ((void *)xex->buf, xex->file_length);
#endif

	free (xex);

	close (xex->fd);

	return NULL;
}

void xex_unload (xex_t *xex)
{
	if (xex->buf)
#ifdef __WIN32__
		free ((void *)xex->buf);
#else
		munmap ((void *)xex->buf, xex->file_length);
#endif

	free (xex);

	close (xex->fd);

}

int xex_validate (xex_t *xex) {
	DWORD magic = get_dword (xex, X_OFF_MAG);

	if (bs32 (magic) != XEX_MAGIC)
		return 0;

	return 1;
}

