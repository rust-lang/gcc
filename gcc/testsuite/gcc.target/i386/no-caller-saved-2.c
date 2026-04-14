/* PR target/124798  */
/* { dg-do compile } */
/* { dg-options "-O2 -mtune=corei7 -mtune-ctrl=^prologue_using_move,^epilogue_using_move -fomit-frame-pointer" } */

[[gnu::no_caller_saved_registers]] extern int foo (void);

int
qux (void)
{
  int a, b, c, d, e, f;
  asm volatile ("# %0 %1 %2 %3 %4 %5"
		: "=r" (a), "=r" (b), "=r" (c), "=r" (d), "=r" (e), "=r" (f));
#ifdef __x86_64__
  int g, h, i, j, k, l, m, n, o, p;
  asm volatile ("# %0 %1 %2 %3 %4 %5 %6 %7 %8 %9"
		: "=r" (g), "=r" (h), "=r" (i), "=r" (j), "=r" (k), "=r" (l), "=r" (m), "=r" (n), "=r" (o), "=r" (p));
#endif
  int ret = foo ();
  asm volatile ("# %0 %1 %2 %3 %4 %5"
		:: "r" (a), "r" (b), "r" (c), "r" (d), "r" (e), "r" (f));
#ifdef __x86_64__
  asm volatile ("# %0 %1 %2 %3 %4 %5 %6 %7 %8 %9"
		: : "r" (g), "r" (h), "r" (i), "r" (j), "r" (k), "r" (l), "r" (m), "r" (n), "r" (o), "r" (p));
#endif

  return ret;
}

/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%ecx, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%esi, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%edi, \[0-9\]*\\(%\[re\]?sp\\)" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %ecx" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %esi" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %edi" } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r8d, \[0-9\]*\\(%\[re\]?sp\\)" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r9d, \[0-9\]*\\(%\[re\]?sp\\)" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r10d, \[0-9\]*\\(%\[re\]?sp\\)" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r11d, \[0-9\]*\\(%\[re\]?sp\\)" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r12d, \[0-9\]*\\(%\[re\]?sp\\)" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r13d, \[0-9\]*\\(%\[re\]?sp\\)" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r14d, \[0-9\]*\\(%\[re\]?sp\\)" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+%r15d, \[0-9\]*\\(%\[re\]?sp\\)" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r8d" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r9d" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r10d" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r11d" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r12d" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r13d" { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "mov(l|q)\[ \\t\]+\[0-9\]*\\(%\[re\]?sp\\), %r15d" { target { ! ia32 } } } } */
