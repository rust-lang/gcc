/* { dg-do compile { target int128 } } */
/* { dg-options "-O2 -fdump-tree-optimized -masm=att" } */
/* { dg-final { scan-tree-dump-times " = \\.MUL_OVERFLOW " 4 "optimized" } } */
/* { dg-final { scan-assembler-times "\tmulq\t" 4 } } */
/* { dg-final { scan-assembler-times "\tseto\t" 3 } } */
/* { dg-final { scan-assembler-times "\tsetno\t" 1 } } */

typedef unsigned long long uint64_t;
typedef unsigned __int128 uint128_t;

#define UINT64_MAX 18446744073709551615ULL

bool
mul64_gt (uint64_t x, uint64_t y)
{
  return uint128_t (x) * y > UINT64_MAX;
}

bool
mul64_le (uint64_t x, uint64_t y)
{
  return uint128_t (x) * y <= UINT64_MAX;
}

bool
mul64_high (uint64_t x, uint64_t y)
{
  return ((__int128) x * y >> 64) != 0;
}

bool
mul64_builtin (uint64_t x, uint64_t y)
{
  uint64_t result;
  return __builtin_mul_overflow (x, y, &result);
}
