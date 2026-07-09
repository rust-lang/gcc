/* { dg-do run } */

__attribute__ ((noipa,noclone,noinline))
void foo (char *a, int b)
{
  char c = 1 << b;
  *a |= c;
}

int
main (void)
{
  char a = 0;
  foo (&a, 1);
  if (a != 2) __builtin_abort ();
  foo (&a, __CHAR_BIT__ + 2);
  if (a != 2) __builtin_abort ();
}
  
