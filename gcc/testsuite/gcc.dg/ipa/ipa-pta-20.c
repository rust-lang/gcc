/* { dg-do run } */
/* { dg-options "-O2 -fipa-pta -fdump-ipa-pta2-details" } */

#include <stdarg.h>

static int __attribute__((noinline, noclone)) q (int n, ...)
{
  va_list va;
  va_start (va, n);
  int *a = va_arg (va, int *);
  int *b = va_arg (va, int *);
  va_end (va);
  return *a + *b;
}

static int __attribute__((noinline, noclone)) foo (int (*p)(int, ...))
{
  int i = 1, j = 3;
  return p(2, &i, &j);
}

int main()
{
  if (foo (q) != 4)
    __builtin_abort ();
  return 0;
}

/* Verify we properly handle variadic arguments for indirect calls and
   .VA_ARG properly accesses only the variadic part.  */

/* { dg-final { scan-ipa-dump "a_\[0-9\]\+ = { i j }" "pta2" } } */
/* { dg-final { scan-ipa-dump "b_\[0-9\]\+ = { i j }" "pta2" } } */
