/* { dg-do compile } */
/* { dg-options "-mtune=xt-c9501fdvt -fdump-rtl-loop2_unroll-details -fno-tree-vectorize" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" "-Os" "-Oz" "-funroll-loops" } { "" } } */

/* The xt-c9501fdvt tune model sets small_loop_unroll_factor to 8, so a small
   loop is unrolled by a factor of 8 (rather than the generic default of 2).
   The unroller reports the number of extra copies, i.e. factor - 1, so an
   8x unroll is dumped as "Unrolled loop 7 times".  This verifies the tune
   parameter actually takes effect; with the default factor the dump would
   instead read "Unrolled loop 1 times".  */

int
small_loop (int n)
{
  int sum = 0;
  do
    sum += n;
  while (--n);
  return sum;
}

/* { dg-final { scan-rtl-dump "Unrolled loop 7 times" "loop2_unroll" } } */
