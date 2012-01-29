#include <stdio.h>
#include <stdarg.h>
#include "xex_main.h"
#include "swissknife.h"

inline long get_dword (xex_t *xex, unsigned int offset)
{
	return bs32(*((DWORD *)(xex->buf + offset)));
}

void xex_warning (char *fmt, ...)
{
	va_list ap;

	va_start (ap, fmt);
	fprintf (stderr, "libxex: ***WARNING*** ");
	vfprintf (stderr, fmt, ap);
	fprintf (stderr, "\n");
	va_end (ap);
}

void xex_error (char *fmt, ...)
{	va_list ap;

	va_start (ap, fmt);
	fprintf (stderr, "libxex: ***ERROR*** ");
	vfprintf (stderr, fmt, ap);
	fprintf (stderr, "\n");
	va_end (ap);
}
