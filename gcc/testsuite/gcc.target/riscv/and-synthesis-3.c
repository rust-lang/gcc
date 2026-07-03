/* { dg-do compile { target rv64 } } */
/* { dg-options "-march=rv64gcb -mabi=lp64d" } */



#define T(X) long andn##X(long x) { return x & X; }

T(0x000200c3233fffffUL)
T(0x300000000003ffffUL)
T(0x00004f10000a0fffUL)
T(0x0000c7f3801fefffUL)
T(0xe00000000000fc00UL)
T(0x0eef7ffffffbbfffUL)
T(0xff0000000000ff00UL)
T(0xff000000000000ffUL)
T(0xfffff00000000fffUL)
T(0xf00000000000000fUL)

/* { dg-final { scan-assembler-times "\\tandn\\t" 7 } } */
/* { dg-final { scan-assembler-times "\\trori\\t" 6 } } */
/* { dg-final { scan-assembler-times "\\tzext.h\\t" 1 } } */
/* { dg-final { scan-assembler-times "\\tzext.w\\t" 1 } } */
/* { dg-final { scan-assembler-times "\\tandi\\t" 1 } } */
