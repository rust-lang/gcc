/* { dg-do compile } */
/* { dg-options "-fopenmp-ompt-detailed" } */

/* { dg-error ".-fopenmp-ompt. and .-fopenmp-ompt-detailed. require .-fopenmp." "" { target *-*-* } 0 } */
