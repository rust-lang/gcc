/* PR target/124146 */
/* { dg-do compile } */
/* { dg-options "-O1" } */

/* The may_alias attribute makes the typedef T its own TYPE_MAIN_VARIANT
   while still recording the user alignment from the aligned attribute.
   Taking the main variant in aarch64_function_arg_alignment therefore did
   not strip the user alignment, which used to trigger an assertion failure
   (ICE) when foo was inlined into bar and the value of type T was passed
   directly to bar.  */

long a;
void *b;
char c;

long
foo (void *p)
{
  typedef __attribute__((__aligned__)) __attribute__((__may_alias__)) unsigned long T;
  a = *(T *) b;
  return a;
}

void
bar (unsigned long x)
{
  long d = foo (&c);
  bar (d);
}
