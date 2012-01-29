#ifndef PIRS_DESC_H
#define PIRS_DESC_H

typedef struct {
	unsigned int offset;
	const char *language;
} pirs_desc_languages_t;

pirs_object *pirs_get_descriptions (pirs_t *pirs);

#endif
