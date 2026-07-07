/* { dg-do compile } */
/* { dg-options "-funroll-loops -fdump-rtl-loop2_unroll-details -fno-tree-vectorize" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" "-Os" "-Oz" } { "" } } */

/* An explicit -funroll-loops turns off the default -munroll-only-small-loops,
   so a loop exceeding the small-loop instruction threshold is unrolled
   normally.  */

void
large_loop_explicit (int *a, int *b, int *c, int *d, int n)
{
  int i;
  for (i = 0; i < n; i++)
    {
      a[i] = b[i] + c[i];
      d[i] = a[i] * b[i] - c[i];
      b[i] = c[i] + d[i] + a[i];
      c[i] = a[i] - d[i] + b[i];
    }
}

/* { dg-final { scan-rtl-dump "Unrolled loop" "loop2_unroll" } } */
