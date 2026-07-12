/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-O2 -mbmi2" } */
/* { dg-final { check-function-bodies "**" "" "" { target *-*-linux* *-*-gnu* } } } */

/* %edx is fixed by the ISA (implicit mulx source) and %edi by the ABI
   (first argument).  The highpart MULX pattern uses the same operand
   for both result registers, and that result is the return value, so
   both are %rax.  Only the magic-constant register is chosen by the
   register allocator, so match it generically.  */
/*
**div7:
**	movabsq	\$2635249153617166336, (%r[a-z0-9]+)
**	movl	%edi, %edx
**	mulx	\1, %rax, %rax
**	ret
**...
*/

unsigned int
div7 (unsigned int x)
{
  return x / 7;
}
