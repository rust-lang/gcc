/* An option the selected target does not know is rejected there,
   with its own diagnostic, not the driver's.  */
/* { dg-do compile } */
/* { dg-options "-m64" } */

int unused;

/* { dg-error "unrecognized command-line option" "" { target *-*-* } 0 } */
