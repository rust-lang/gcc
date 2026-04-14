/* { dg-do compile { target { ! x32 } } } */
/* { dg-options "-O2 -masm=att -mno-mmx -mno-80387 -mno-sse" } */

#include "abi-mix-1.c"

/* { dg-final { scan-assembler-not "%st" } } */
/* { dg-final { scan-assembler-not "%xmm" } } */
