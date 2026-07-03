/* { dg-do compile { target rv64 } } */
/* { dg-options "-march=rv64gcb -mabi=lp64d" } */



#define T(X) long xnor##X(long x) { return x ^ X; }

T(0x000200c3233fffffUL)
T(0x300000000003ffffUL)
T(0x00004f10000a0fffUL)
T(0x0000c7f3801fefffUL)
T(0xe00000000000fc00UL)
T(0x0eef7ffffffbbfffUL)
T(0xff0000000000ff00UL)

/* Each test above is better handled by inverting the constant
   and using xnor.  */

/* { dg-final { scan-assembler-times "\\txnor\\t" 7 } } */
