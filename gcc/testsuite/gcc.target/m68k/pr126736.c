/* { dg-do run } */
/* { dg-require-effective-target builtin_eh_return } */
/* { dg-options "-O2" } */

/* A function that calls __builtin_eh_return saves %d0 and %d1, the
   EH_RETURN_DATA_REGNO registers.  Only the exception return path may
   restore them: on a normal return they hold the return value.  */

extern void use (void *);

__attribute__((noipa))
int
leaf_frame (int should_unwind, long offset)
{
  if (should_unwind)
    __builtin_eh_return (offset, 0);
  return 42;
}

/* Saves three registers, so the prologue uses moveml but the epilogue has
   too few left to restore with it.  */

__attribute__((noipa))
int
partial_moveml (int should_unwind, long offset)
{
  register int a __asm__ ("%d2") = 7;

  __asm__ ("" : "+r" (a));
  if (should_unwind)
    __builtin_eh_return (offset, 0);
  return a + 35;
}

__attribute__((noipa))
int
full_moveml (int should_unwind, long offset)
{
  register int a __asm__ ("%d2") = 1;
  register int b __asm__ ("%d3") = 2;
  register int c __asm__ ("%d4") = 3;
  register int d __asm__ ("%a2") = 4;

  __asm__ ("" : "+r" (a), "+r" (b), "+r" (c), "+r" (d));
  if (should_unwind)
    __builtin_eh_return (offset, 0);
  return a + b + c + d + 32;
}

__attribute__((noipa))
int
alloca_frame (int should_unwind, long offset, int size)
{
  char *buffer = __builtin_alloca (size);

  use (buffer);
  if (should_unwind)
    __builtin_eh_return (offset, 0);
  return 42;
}

/* A frame too big for a 16-bit offset, so the restores go through %a1.  */

__attribute__((noipa))
int
big_frame (int should_unwind, long offset)
{
  register int a __asm__ ("%d2") = 7;
  volatile char buffer[0x9000];

  buffer[0] = 1;
  use ((void *) buffer);
  __asm__ ("" : "+r" (a));
  if (should_unwind)
    __builtin_eh_return (offset, 0);
  return a + buffer[0] + 34;
}

__attribute__((noinline, noclone))
void
use (void *p)
{
  __asm__ ("" :: "r" (p) : "memory");
}

int
main (void)
{
  if (leaf_frame (0, 0) != 42)
    __builtin_abort ();
  if (partial_moveml (0, 0) != 42)
    __builtin_abort ();
  if (full_moveml (0, 0) != 42)
    __builtin_abort ();
  if (alloca_frame (0, 0, 64) != 42)
    __builtin_abort ();
  if (big_frame (0, 0) != 42)
    __builtin_abort ();
  return 0;
}
