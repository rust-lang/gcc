/* PR target/124798  */
/* { dg-do compile { target int128 } } */
/* { dg-options "-O2 -mtune=corei7 -mtune-ctrl=^prologue_using_move,^epilogue_using_move -fomit-frame-pointer" } */

[[gnu::no_caller_saved_registers]] extern __int128 foo (void);

__int128
qux (void)
{
  int a, b, c, d, e, f;
  asm volatile ("# %0 %1 %2 %3 %4 %5"
		: "=r" (a), "=r" (b), "=r" (c), "=r" (d), "=r" (e), "=r" (f));
#ifdef __x86_64__
  int g, h, i, j, k, l, m, n;
  asm volatile ("# %0 %1 %2 %3 %4 %5 %6 %7"
		: "=r" (g), "=r" (h), "=r" (i), "=r" (j), "=r" (k), "=r" (l), "=r" (m), "=r" (n));
#endif
  __int128 ret = foo ();
  asm volatile ("# %0 %1 %2 %3 %4 %5"
		:: "r" (a), "r" (b), "r" (c), "r" (d), "r" (e), "r" (f));
#ifdef __x86_64__
  asm volatile ("# %0 %1 %2 %3 %4 %5 %6 %7"
		: : "r" (g), "r" (h), "r" (i), "r" (j), "r" (k), "r" (l), "r" (m), "r" (n));
#endif

  return ret;
}

/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%edx, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%ecx, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%esi, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%edi, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %edx" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %ecx" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %esi" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %edi" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r8d, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r9d, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r10d, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r11d, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r8d" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r9d" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r10d" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r11d" } } */
