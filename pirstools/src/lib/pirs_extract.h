#ifndef PIRS_EXTRACT_H
#define PIRS_EXTRACT_H

#include "pirs_main.h"
#include "pirs_datatypes.h"

typedef void (* pirs_extract_callback)(pirs_t *pirs, const char *path, const char *filename, pirs_chunk *chunks);

int pirs_extract_all (pirs_t *pirs, const char *basepath, pirs_extract_callback callback);

#endif
