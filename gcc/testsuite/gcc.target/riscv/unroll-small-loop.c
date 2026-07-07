/* { dg-do compile } */
/* { dg-options "-fdump-rtl-loop2_unroll-details -fno-tree-vectorize" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" "-Os" "-Oz" "-funroll-loops" } { "" } } */

/* Small loop unrolling is enabled by default at -O2 (-funroll-loops with
   -munroll-only-small-loops), so a loop whose body is within the small-loop
   instruction threshold is unrolled without any explicit option.  */

int
small_loop (int n)
{
  int sum = 0;
  do
    sum += n;
  while (--n);
  return sum;
}

/* { dg-final { scan-rtl-dump "Unrolled loop" "loop2_unroll" } } */
