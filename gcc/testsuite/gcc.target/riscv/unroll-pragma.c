/* { dg-do compile } */
/* { dg-options "-fdump-rtl-loop2_unroll-details -fno-tree-vectorize" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" "-Os" "-Oz" "-funroll-loops" } { "" } } */

/* An explicit "#pragma GCC unroll" sets loop->unroll, which makes
   riscv_loop_unroll_adjust bypass the -munroll-only-small-loops
   restriction.  Hence a loop exceeding the small-loop instruction
   threshold is still unrolled as requested.  */

void
large_loop_pragma (int *a, int *b, int *c, int *d, int n)
{
#pragma GCC unroll 2
  for (int i = 0; i < n; i++)
    {
      a[i] = b[i] + c[i];
      d[i] = a[i] * b[i] - c[i];
      b[i] = c[i] + d[i] + a[i];
      c[i] = a[i] - d[i] + b[i];
    }
}

/* { dg-final { scan-rtl-dump "Unrolled loop" "loop2_unroll" } } */
