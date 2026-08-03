/* With no m68k assembler anywhere, the driver says so instead of
   running the host's.  */
/* { dg-do assemble { target multi_target_as_unresolved_m68k } } */

int f (void)
{
  return 42;
}

/* { dg-error "assembler for .m68k" "" { target *-*-* } 0 } */
/* { dg-prune-output "compilation terminated" } */
