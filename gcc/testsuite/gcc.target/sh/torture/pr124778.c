/* { dg-do compile }  */
/* { dg-additional-options "-std=gnu99" }  */
/* This used to trigger an ICE in decompose_multiword_subregs at -O2.  */

long long g;

void
f (unsigned n, char *p)
{
  long long size = 4 + (long long)n;
  while (size >= 4)
    {
      size -= 4;
      p += 4;
    }
  g += *(unsigned short *)p;
}
