/* { dg-do compile } */
/* { dg-options "-mavx2 -O2" } */

typedef unsigned int v4si __attribute__((vector_size(16)));

v4si foo(v4si vec, int val) {
    vec[0] = val;
    vec[2] = val;
    return vec;
}

v4si bar(v4si vec, int val) {
    vec[0] = val;
    vec[1] = val;
    vec[2] = val;
    vec[3] = val;
    return vec;
}

/* { dg-final { scan-assembler-times "vpbroadcastd" 2 } } */
/* { dg-final { scan-assembler-times "vpblendd\[ \\t\]+" 1 } } */
/* { dg-final { scan-assembler-not "vpinsrd" } } */
