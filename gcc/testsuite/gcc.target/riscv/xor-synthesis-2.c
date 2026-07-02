/* { dg-do compile { target { rv64 } } } */
/* { dg-options "-march=rv64gb -mabi=lp64d" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" } } */

unsigned long foo(unsigned long src) { return src ^ 0x8800000000000007; }

/* { dg-final { scan-assembler-times "\\sbinvi\t" 2 } } */
/* { dg-final { scan-assembler-times "\\sxori\t" 1 } } */

