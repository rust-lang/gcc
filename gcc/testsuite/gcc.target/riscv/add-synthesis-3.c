/* { dg-do compile { target rv64 } } */
/* { dg-options "-march=rv64gcb -mabi=lp64d" } */

long F1 (long x) { return x + 0xffffffff; }
long F2 (long x) { return x + (1UL << (sizeof (long) * 8 - 1) ); }

/* { dg-final { scan-assembler-times "\\tadd\\.uw\\t" 1 } } */
/* { dg-final { scan-assembler-times "\\tbinvi\\t" 1 } } */
