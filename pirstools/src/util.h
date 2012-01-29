#ifndef SWISSKNIFE_H
#define SWISSKNIFE_H

inline void sprint_bits (char *buffer, unsigned int number);

#define bit_get(number, i) (((number) & (1 << (i))) > 0)

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#endif
