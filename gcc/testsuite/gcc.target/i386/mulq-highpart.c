/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-O2 -mno-bmi2" } */
/* { dg-final { check-function-bodies "**" "" "" { target *-*-linux* *-*-gnu* } } } */

/* %rax, %rdx and %edi are fixed by the ISA (mulq) and the ABI (first
   argument), but the register holding the magic constant is chosen by
   the register allocator, so match it generically.  */
/*
**div7:
**	movabsq	\$2635249153617166336, (%r[a-z0-9]+)
**	movl	%edi, %eax
**	mulq	\1
**	movl	%edx, %eax
**	ret
**...
*/

unsigned int
div7 (unsigned int x)
{
  return x / 7;
}