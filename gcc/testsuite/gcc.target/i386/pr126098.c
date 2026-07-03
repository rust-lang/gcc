/* PR target/126098 */
/* { dg-do compile } */
/* { dg-options "-mavx512vnni -O3" } */

/* AVX512VNNI does not imply AVX512BW, so the byte (V64QI) dot-product must
   not be advertised as available: emulating it needs vpmovsxbw on 512-bit
   vectors, which requires AVX512BW.  Advertising it led to an unrecognizable
   (sign_extend:V32HI (reg:V32QI)) insn and an ICE in extract_insn.  */

char *fn_c, *fn_d;
int fn() {
  int res = 0;
  for (int i = 0; i < 64; ++i)
    res += fn_c[i] * fn_d[i];
  return res;
}
