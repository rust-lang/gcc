/* { dg-additional-options -O2 } */
/* { dg-additional-options -fdump-tree-forwprop1 } */

long* SetupPrecalculatedData1 (long* a) {
  long b = 1;
  int i;
  for (i = 0; i < 64; i++) {
    if(i>>3 < 7)
      a[i] += (b<<(i+8));
  }
  return a;
}

long* SetupPrecalculatedData2 (long* a) {
  long b = 1;
  int i;
  for (i = 0; i <= 64; i++) {
    if(i>>3 < 7)
      a[i] += (b<<(i+8));
  }
  return a;
}

long* SetupPrecalculatedData3 (long* a) {
  long b = 1;
  int i;
  for (i = 0; i < 64; i++) {
    if(i>>3 > 7)
      a[i] += (b<<(i+8));
  }
  return a;
}

long* SetupPrecalculatedData4 (long* a) {
  long b = 1;
  int i;
  for (i = 0; i < 64; i++) {
    if(i>>3 >= 7)
      a[i] += (b<<(i+8));
  }
  return a;
}

/* { dg-final { scan-tree-dump-times ">> 3" 0 forwprop1 } } */
