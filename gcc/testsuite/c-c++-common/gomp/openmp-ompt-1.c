/* { dg-do compile } */
/* { dg-options "-fopenmp-ompt" } */

/* { dg-error ".-fopenmp-ompt. and .-fopenmp-ompt-detailed. require .-fopenmp." "" { target *-*-* } 0 } */
