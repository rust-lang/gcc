/* Verify that no_caller_saved_registers calls still clobber x87 and MMX
   registers.  */
/* { dg-do compile } */
/* { dg-options "-O2 -masm=att -mmmx -mno-sse -fomit-frame-pointer" } */

typedef int v2si __attribute__ ((vector_size (8)));
extern void foo (void) __attribute__ ((no_caller_saved_registers));

void
bar_mmx (void)
{
  v2si x;
  __asm__ volatile ("# %0" : "=y" (x));
  foo ();
  __asm__ volatile ("# %0" :: "y" (x));
}

/* { dg-final { scan-assembler-times "movq\[ \t\]+%mm\[0-7\], \[0-9\]*\\(%\[re\]?sp\\)" 1 } } */
/* { dg-final { scan-assembler-times "movq\[ \t\]+\[0-9\]*\\(%\[re\]?sp\\), %mm\[0-7\]" 1 } } */
