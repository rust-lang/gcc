/* { dg-final { check-function-bodies "**" "" "-DCHECK_ASM" } } */
/* { dg-do assemble { target aarch64_asm_ssve-aes_ok } } */
/* { dg-do compile { target { ! aarch64_asm_ssve-aes_ok } } } */

#include "test_sve_acle.h"

#ifdef STREAMING_COMPATIBLE
#pragma GCC target "+ssve-aes"
#else
#pragma GCC target "+sve2-aes"
#endif

/*
** aesmc_u8_tied1:
**	aesmc	z0\.b, z0\.b
**	ret
*/
TEST_UNIFORM_Z (aesmc_u8_tied1, svuint8_t,
		z0 = svaesmc_u8 (z0),
		z0 = svaesmc (z0))

/*
** aesmc_u8_untied:
** (
**	mov	z0\.d, z1\.d
**	aesmc	z0\.b, z0\.b
** |
**	aesmc	z1\.b, z1\.b
**	mov	z0\.d, z1\.d
** )
**	ret
*/
TEST_UNIFORM_Z (aesmc_u8_untied, svuint8_t,
		z0 = svaesmc_u8 (z1),
		z0 = svaesmc (z1))
