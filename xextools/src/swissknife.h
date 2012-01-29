#ifndef SWISSKNIFE_H
#define SWISSKNIFE_H

inline long get_dword (void *data, unsigned int offset);

void hexdump (unsigned char *data, unsigned int offset, unsigned int size);

/* Need a 36 bytes buffer passed as 'buffer' argument. */
inline void sprint_bits (char *buffer, unsigned int number);

#define bs32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
#define bs16(x) ((((x) & 0x0000ff00) >> 8) | (((x) & 0x000000ff) << 8))
#define round(n, r) ((n) + ((r) - ((n) % (r)) == (r) ? 0 : (r) - ((n) % (r))))
#define bit_get(number, i) (((number) & (1 << (i))) > 0)

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#endif
