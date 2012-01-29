#include <stdio.h>
#include "swissknife.h"

/* Need a 36 bytes buffer passed as 'buffer' argument. */
inline void sprint_bits (char *buffer, unsigned int number) 
{
        int i, y;

        for (i = y = 0; i < 32; i++) {
                buffer[y++] = '0' + bit_get (number, 31 - i);

                if (!((i + 1) % 8) && i != 31)
                        buffer[y++] = ' ';
        }       

        buffer[y] = '\0'; 
}
