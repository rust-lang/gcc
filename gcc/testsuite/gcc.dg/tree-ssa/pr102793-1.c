/* { dg-do compile } */
/* { dg-options "-O3 -fdump-tree-pre -fdump-tree-optimized --param logical-op-non-short-circuit=1" } */

typedef __UINT64_TYPE__ uint64_t;

int foo(void);

int ccmp(uint64_t* s1, uint64_t* s2)
{
    uint64_t d1, d2, bar;
    d1 = *s1++;
    d2 = *s2++;
    bar = (d1 ^ d2) & 0xabcd;
    if (bar == 0 || d1 != d2)
      return foo();
    return 0;
}

int noccmp0(uint64_t* s1, uint64_t* s2)
{
    uint64_t d1, d2, bar;

    d1 = *s1++;
    d2 = *s2++;
    bar = (d1 ^ d2) & 0xabcd;
    if (bar == 0)
      return foo();
    if (d1 != d2)
      return foo();
    return 0;
}

int noccmp1(uint64_t* s1, uint64_t* s2)
{
    uint64_t d1, d2, d3, d4, bar;
    d1 = *s1++;
    d2 = *s2++;
    d3 = *s1++;
    d4 = *s2++;
    bar = (d1 ^ d2) & 0xabcd;
    if (bar == 0)
      return foo();
    if (d3 != d4)
      return foo();
    return 0;
}

/* Check for condition assignments for noccmp1 and noccmp0.  */
/* { dg-final { scan-tree-dump-times {_\d+ = d\d+_\d+ != d\d+_\d+;\n  _\d+ = bar_\d+ == 0;} 1 "pre" } } */
/* { dg-final { scan-tree-dump-times "optimizing two comparisons " 2 "pre" } } */
/* The conditional in noccmp1 should be optimized in pre */
/* { dg-final { scan-tree-dump-times " if " 1 "pre" } } */
/* { dg-final { scan-tree-dump-times " if " 1 "optimized" } } */
