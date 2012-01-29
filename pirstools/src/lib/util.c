#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "pirs_main.h"
#include "pirs_datatypes.h"
#include "util.h"

inline long get_dword(pirs_t * pirs, unsigned int offset)
{
    return bs32(*((DWORD *) (pirs->buf + offset)));
}

void pirs_warning(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "libpirs: ***WARNING*** ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

void pirs_error(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "libpirs: ***ERROR*** ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

int utf8_to_iso(const char *utf8string, char *isobuffer)
{
    int i, j;

    for (i = j = 0; utf8string[i]; i += 2)
        isobuffer[j++] = utf8string[i];

    isobuffer[j] = '\0';

    return j;
}

const char *str_concatenate(const char *s1, ...)
{
    va_list ap;
    const char *s;
    char *buf, *ptr;
    int len = 0;

    /* Calculate the total length */
    va_start(ap, s1);

    len += strlen(s1);

    while (1) {
        s = va_arg(ap, const char *);
        if (s == NULL)
            break;
        len += strlen(s);
    }

    va_end(ap);

    /* Allocate the memory */
    buf = malloc(len + 1);

    /* Concatenate everything */
    va_start(ap, s1);

    ptr = buf;

    strcpy(ptr, s1);
    ptr += strlen(s1);

    while (1) {
        s = va_arg(ap, const char *);
        if (s == NULL)
            break;

        strcpy(ptr, s);
        ptr += strlen(s);
    }

    va_end(ap);

    *ptr = '\0';

    return (const char *) buf;
}

const char *strdup_printf(char *fmt, ...)
{
    va_list ap;

    char buffer[MAX_STRDUP_PRINTF_BUF];

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    return strdup(buffer);
}
