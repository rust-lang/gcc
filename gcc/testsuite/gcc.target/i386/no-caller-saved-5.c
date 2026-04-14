/* PR target/124798  */
/* { dg-do compile } */
/* { dg-options "-O2 -mtune=corei7 -msse2 -mtune-ctrl=^prologue_using_move,^epilogue_using_move -fomit-frame-pointer" } */

[[gnu::no_caller_saved_registers]] extern float foo (void);

float
qux (void)
{
  float a, b, c, d, e, f;
  asm volatile ("# %0 %1 %2 %3 %4 %5"
		: "=v" (a), "=v" (b), "=v" (c), "=v" (d), "=v" (e), "=v" (f));
#ifdef __x86_64__
  float g, h, i, j, k, l, m, n, o, p;
  asm volatile ("# %0 %1"
		: "=v" (g), "=v" (h));
  asm volatile ("# %0 %1 %2 %3 %4 %5 %6 %7"
		: "=v" (i), "=v" (j), "=v" (k), "=v" (l), "=v" (m), "=v" (n), "=v" (o), "=v" (p));
#endif
  float ret = foo ();
  asm volatile ("# %0 %1 %2 %3 %4 %5"
		:: "v" (a), "v" (b), "v" (c), "v" (d), "v" (e), "v" (f));
#ifdef __x86_64__
  asm volatile ("# %0 %1"
		:: "v" (g), "v" (h));
  asm volatile ("# %0 %1 %2 %3 %4 %5 %6 %7"
		: : "v" (i), "v" (j), "v" (k), "v" (l), "v" (m), "v" (n), "v" (o), "v" (p));
#endif

  return ret;
}

/* { dg-final { scan-assembler-not "movss\[ \\t\]+%xmm\[0-9\]+, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %xmm\[0-9\]+" } } */
