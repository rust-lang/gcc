/* Verify that no_caller_saved_registers calls still clobber x87 and MMX
   registers.  */
/* { dg-do compile } */
/* { dg-options "-O2 -masm=att -mfpmath=387 -m80387 -mlong-double-80 -mno-sse -fomit-frame-pointer" } */

typedef int v2si __attribute__ ((vector_size (8)));
extern void foo (void) __attribute__ ((no_caller_saved_registers));

long double
bar_x87 (long double x, long double y)
{
  long double z = x + y;
  foo ();
  return z + 1.0L;
}

/* { dg-final { scan-assembler-times "fstpt\[ \t\]+" 1 } } */
