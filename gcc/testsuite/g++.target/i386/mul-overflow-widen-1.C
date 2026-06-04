/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized -masm=att" } */
/* { dg-final { scan-tree-dump-times " = \\.MUL_OVERFLOW " 8 "optimized" } } */
/* { dg-final { scan-assembler-times "\tmulw\t" 3 } } */
/* { dg-final { scan-assembler-times "\tmull\t" 5 } } */
/* { dg-final { scan-assembler-times "\tseto\t" 5 } } */
/* { dg-final { scan-assembler-times "\tsetno\t" 3 } } */

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#define UINT16_MAX 65535U
#define UINT32_MAX 4294967295U

bool
mul16_gt (uint16_t x, uint16_t y)
{
  return uint32_t (x) * y > UINT16_MAX;
}

bool
mul16_le (uint16_t x, uint16_t y)
{
  return uint32_t (x) * y <= UINT16_MAX;
}

bool
mul16_builtin (uint16_t x, uint16_t y)
{
  uint16_t result;
  return __builtin_mul_overflow (x, y, &result);
}

bool
mul32_gt (uint32_t x, uint32_t y)
{
  return uint64_t (x) * y > UINT32_MAX;
}

bool
mul32_le (uint32_t x, uint32_t y)
{
  return uint64_t (x) * y <= UINT32_MAX;
}

bool
mul32_gt_rev (uint32_t x, uint32_t y)
{
  return UINT32_MAX < uint64_t (x) * y;
}

bool
mul32_le_rev (uint32_t x, uint32_t y)
{
  return UINT32_MAX >= uint64_t (x) * y;
}

bool
mul32_builtin (uint32_t x, uint32_t y)
{
  uint32_t result;
  return __builtin_mul_overflow (x, y, &result);
}
