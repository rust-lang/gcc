/* PR target/124146 */
/* { dg-do compile } */
/* { dg-options "-O1" } */

/* Verify that the C++ version of the original reproducer does not ICE.  */

long a;
void *b;
char c;

long
foo (void *)
{
  typedef __attribute__((__aligned__))
    __attribute__((__may_alias__)) unsigned long T;
  a = *(T *) b;
  return a;
}

void
bar (unsigned long)
{
  long d = foo (&c);
  bar (d);
}
