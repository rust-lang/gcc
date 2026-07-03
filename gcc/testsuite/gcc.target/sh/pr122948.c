/* Verify that the T bit (carry/borrow) set expression of the subc insn is
   correct.  */
/* { dg-do run } */
/* { dg-options "-O1" } */

extern void abort (void);

__attribute__ ((noipa)) unsigned int
foo (unsigned int a, unsigned int b)
{
  if (b - a - 1 <= b)
    a = 0;
  return a;
}

__attribute__ ((noipa)) unsigned int
bar (unsigned int a, unsigned int b)
{
  return (b - a - 1) > b;
}

__attribute__ ((noipa)) unsigned int
baz (unsigned int a, unsigned int b)
{
  return (a + b + 1) <= a;
}

int
main (void)
{
  if (foo (0xffffffff, 0x4e92c833) != 0)
    abort ();

  if (bar (0xffffffff, 0x4e92c833) != 0)
    abort ();

  /* b - a - 1 wraps (borrow) whenever a + 1 > b (unsigned).  */
  if (bar (5, 3) != 1)
    abort ();

  if (bar (0, 0) != 1)
    abort ();

  if (bar (0, 5) != 0)
    abort ();

  /* addc carry-out: (a + b + 1) <= a.  */
  /* 0xffffffff + 0 + 1 wraps to 0.  */
  if (baz (0xffffffff, 0) != 1)
    abort ();

  if (baz (0xffffffff, 0xffffffff) != 1)
    abort ();

  /* wraps to 0.  */
  if (baz (0xfffffffe, 1) != 1)
    abort ();

  /* 1 <= 0 is false.  */
  if (baz (0, 0) != 0)
    abort ();

  if (baz (1, 2) != 0)
    abort ();

  return 0;
}
