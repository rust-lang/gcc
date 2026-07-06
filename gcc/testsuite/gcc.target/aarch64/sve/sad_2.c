/* { dg-do compile } */
/* { dg-options "-O3 -msve-vector-bits=128 -mtune=neoverse-v2" } */

#include <stdint.h>

int foo (uint8_t *pix1, uint8_t *pix2)
{
    int sum = 0;
    for (int x = 0; x < 48; x++)
    {
        sum += __builtin_abs (pix1[x] - pix2[x]);
    }
    return sum;
}

/* { dg-final { scan-assembler-times {\tudot\t} 3 } } */
