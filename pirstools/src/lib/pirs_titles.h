#ifndef PIRS_TITLES_H
#define PIRS_TITLES_H

typedef struct {
	unsigned int offset;
	const char *language;
} pirs_title_languages_t;

pirs_object *pirs_get_titles (pirs_t *pirs);

#endif
