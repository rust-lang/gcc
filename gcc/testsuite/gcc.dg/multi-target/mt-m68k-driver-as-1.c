/* Assembling for the selection goes through its cross assembler.  */
/* { dg-do assemble { target multi_target_as_m68k } } */

int f (void)
{
  return 42;
}
