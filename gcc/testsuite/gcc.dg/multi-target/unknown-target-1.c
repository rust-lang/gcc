/* An unknown --target= is rejected up front, with the error listing
   every target built into the compiler.  */
/* { dg-do compile } */
/* { dg-options "--target=bogus-unknown-elf" } */

int unused;

/* { dg-error "unknown target .bogus-unknown-elf." "" { target *-*-* } 0 } */
/* { dg-prune-output "compilation terminated" } */
