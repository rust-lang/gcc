/* PR tree-optimization/124545 */
/* Runtime correctness for the inverse-widening VN rewrite
   (T)A +- CST -> (T)(A +- CST').  The rewrite must never change the
   computed value.  In particular it must NOT fire when CST is not
   representable in the inner type (which would silently drop the bits
   above the inner precision), and it must stay correct for unsigned
   inner types where the narrow operation wraps.  Uses __UINT{32,64}_TYPE__
   rather than unsigned {int,long} so that the narrow-vs-wide contrast is
   independent of ILP32 vs LP64.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* CST = 2^32 does not fit in int: the value must be preserved.
   Before the fix this comparison folded to a constant 1.  */
__attribute__((noipa)) int
oor_eq (int a)
{
  return ((__UINT64_TYPE__) a + 0x100000000ULL) == (__UINT64_TYPE__) a;
}

__attribute__((noipa)) __UINT64_TYPE__
oor_val (int a)
{
  return (__UINT64_TYPE__) a + 0x100000000ULL;
}

/* Unsigned inner: narrow add wraps mod 2^32; the widened add does not.
   The result must match the wide arithmetic for every input.  */
__attribute__((noipa)) int
uns_carry (__UINT32_TYPE__ a)
{
  __UINT32_TYPE__ t = a + 100u;
  __UINT64_TYPE__ w = (__UINT64_TYPE__) a + 100;
  return w == (__UINT64_TYPE__) t;
}

/* Legitimate in-range case (matches the PR): k == j - 1, so the two
   loads are the same address and the rewrite may fire.  */
__attribute__((noipa)) int
inrange_eq (int *p, int j)
{
  int k = j - 1;
  return p[j - 1] == p[k];
}

int
main (void)
{
  if (oor_eq (5) != 0) __builtin_abort ();
  if (oor_eq (-1) != 0) __builtin_abort ();
  if (oor_val (5) != 5ULL + 0x100000000ULL) __builtin_abort ();
  if (uns_carry (0xfffffff0u) != 0) __builtin_abort ();
  if (uns_carry (10) != 1) __builtin_abort ();
  int arr[4] = { 7, 7, 7, 7 };
  if (inrange_eq (arr, 2) != 1) __builtin_abort ();
  return 0;
}
