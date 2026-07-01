/* { dg-do run }  */
/* { dg-options "-ffixed-r7 -ffixed-r8 -ffixed-r9 -ffixed-r10 -ffixed-r11 -ffixed-r12 -ffixed-r13" }  */
/* { dg-skip-if "over-constrained regs when not optimizting" { *-*-* } { "-O0" } { "" } }  */

/* The 'block_lump_real_i4' insn that is emitted on SH4 will use up regs
   r0~r6.  With r7~r13 being artificially fixed for this test case, there
   is only r14 (frame pointer) and r15 (stack pointer) left.  When compiling
   with -O0 the frame pointer is not eliminated and there are no free registers
   remaining and it will ICE.  */

#include <stdlib.h>
#include <string.h>

typedef struct { int c[64]; } obj;
obj obj0;
obj obj1;

void __attribute__ ((noinline))
bar (int a, int b, int c, int d, obj *q)
{
  if (q->c[0] != 0x12345678 || q->c[1] != 0xdeadbeef)
    abort ();
}

void foo (obj *p)
{
  obj bobj;
  bobj = *p;
  bar (0, 0, 0, 0, &bobj);
}

int
main ()
{
  obj0.c[0] = 0x12345678;
  obj0.c[1] = 0xdeadbeef;
  foo (&obj0);
  exit (0);
}
