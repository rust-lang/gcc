/* A foreign -march= value is judged by the selected target.  */
/* { dg-do compile } */
/* { dg-options "-march=x86-64" } */

int unused;

/* { dg-error "unrecognized argument in option" "" { target *-*-* } 0 } */
/* { dg-prune-output "valid arguments" } */
