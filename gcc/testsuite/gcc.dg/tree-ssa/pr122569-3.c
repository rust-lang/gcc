/* PR tree-optimization/122569 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-forwprop1-details" } */

/* Test that the forwprop DeBruijn CLZ matcher recognizes the variant
   idiom that isolates the MSB as a power of two via (s - (s >> 1))
   after the OR-cascade.  This pattern uses a CTZ-style DeBruijn magic
   applied to 2^MSB, not to the all-bits-below value directly.
   Reported on PR 122569 as a second reproducer.

   The table element type here is unsigned long (64-bit on LP64 targets)
   rather than int, which exercises the relaxed element-type check.  */

typedef unsigned long long uint64_t;

void
get_msb_index (unsigned long *result, uint64_t value)
{
  static const unsigned long deBruijnTable64[64] = {
    63, 0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54,
    33, 42, 3,  61, 51, 37, 40, 49, 18, 28, 20, 55, 30,
    34, 11, 43, 14, 22, 4,  62, 57, 46, 52, 38, 26, 32,
    41, 50, 36, 17, 19, 29, 10, 13, 21, 56, 45, 25, 31,
    35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5
  };

  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  value |= value >> 32;

  *result = deBruijnTable64[((value - (value >> 1))
			     * (uint64_t) 0x07EDD5E59A4E28C2ULL) >> 58];
}

/* { dg-final { scan-tree-dump "__builtin_clz|\\.CLZ" "forwprop1" { target { clzll && { lp64 || llp64 } } } } } */
