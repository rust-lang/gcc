/* { dg-options "-O3 -w -g" } */
/* { dg-additional-options "-fPIC" { target fpic } } */

extern float fmodf(float, float);
float b, c;
long h, i, k, *ab;
char j;
short m;
static __attribute__((noinline)) int a(int n, int p2) {
  static int d[6];
  for (int e = 0; e < 256; ++e)
    d[e] = e;
  int f = n;
  f = d[f ^ p2];
  return f;
}
int g(double, char) { return 0; }
void l(long n, int p2, char p) {
  long o = 1, q, *t;
  int r = 1, s;
  ab = &o;
  k = r = k;
  if (0 >= p)
    goto u;
  goto v;
u:
  r = n - o;
  if ((9142 ^ r) >= 90 + r)
    goto ac;
  r = k ? k : 9039991.125f;
  if (__builtin_isfinite(c))
    __builtin_trap();
  if (905969534 + (p << 24) < o)
    goto v;
ac:
  q = -o;
  m = 6 - (p << 6);
  if (q + 1073741825 ^ o + 7 & q)
    goto af;
  goto ae;
af:
  if (__builtin_isfinite(c))
    __builtin_trap();
  *ab = 5 + o;
  if (p2)
    goto u;
  if (__builtin_isfinite(b / b))
    __builtin_trap();
  c = 0 + fmodf(b, b);
  if (o < n)
    goto ag;
  if (h)
    goto af;
ae:
  *t = k - 10992403549608;
  q = 21285390711 * o - 403452967995;
  if (q)
    goto ah;
  goto ai;
ah:
  k = h + h % 490681911 - *t % 9283091488;
  r = (int)(o >> 4) - 1321465791 - -2057527364 + 724134559;
  if (p)
    goto ai;
ag:
  *t = j = g(0, p) ?: 81;
  r = h;
  if ((5275004732 | q - 80545797) - 9 / o)
    goto ai;
v:
  m = r = b;
ai:
  s = q >> 4;
  a(s, m);
  s = a(s, 0);
  s = a(s, r);
  a(s, b);
  s = (long)p2 >> a(s, p);
  a(s, i);
}
