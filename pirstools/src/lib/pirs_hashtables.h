#ifndef PIRS_HASHES_H
#define PIRS_HASHES_H

#include <stdint.h>

pirs_object **pirs_get_hashtables (pirs_t *pirs);
unsigned int pirs_count_hashtables (pirs_t *pirs);
pirs_chunk *pirs_get_hashtables_offsets (pirs_t *pirs);

struct __pears_hash_ent {
	uint32_t A;		/* SHA-1: A	*/
	uint32_t B;		/* SHA-1: B	*/	
	uint32_t C;		/* SHA-1: C	*/
	uint32_t D;		/* SHA-1: D	*/
	uint32_t E;		/* SHA-1: E	*/
	uint32_t unknown0;	/* Unknown 5	*/
};

#endif
