/* Check that the loop is unrolled by 2 rather than 4 for small niters.  */
/* { dg-do compile } */
/* { dg-options "-O2 -mtune=neoverse-v2 -mautovec-preference=sve-only" } */

#include <stdint.h>
#include <stdlib.h>

int
foo (uint8_t *p1, uint8_t *p2)
{
  int sum = 0;
  for (int i = 0; i < 20; i++)
    sum += abs (p1[i] - p2[i]);
  return sum;
}

/* { dg-final { scan-assembler-times {\tld1b\t[^\n]*, mul vl} 2 } } */
