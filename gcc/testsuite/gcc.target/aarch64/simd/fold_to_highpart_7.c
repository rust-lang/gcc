/* { dg-do compile } */
/* { dg-additional-options "-O3" } */
/* { dg-final { check-function-bodies "**" "" } } */

#include <arm_neon.h>

/* We should fold to the highpart builtin when the multiplying the highpart of a
   128b vector with a uniform vector. Also the uniform vector should be loaded
   as a single wider vector duplicate load (ld1r) using the lower half for the
   lowpart builtin and the full vector duplicate for the highpart builtin.

   Use vdup_n_u8 to create an 8x8 splat vector.  */

/*
** foo:
**	...
**	ld1r	{v([0-9]+).16b}, \[x([0-9]+)\]
**	...
**	umull2	v([0-9]+).8h, v([0-9]+).16b, v([0-9]+).16b
**	...
*/
uint16x8_t foo(uint8_t *a, uint8_t *b) {
    const uint8x8_t uniform_vec = vdup_n_u8(*a);
    const uint8x16_t hipart_vec = vld1q_u8(b);
    return vmull_u8(vget_high_u8(hipart_vec), uniform_vec);
}

/* { dg-final { scan-assembler-not {\tdup\tv} } } */
