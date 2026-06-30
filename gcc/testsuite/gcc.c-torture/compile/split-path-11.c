
void foo(unsigned long long *M)
{
  for (unsigned long long k = 0; k < 227; ++k)
    {
      unsigned long long y =
	((M[k] & 0xffffffff80000000ULL) | (M[k + 1] & 0x7fffffffULL));
      M[k] = (M[k + 397] ^ (y >> 1) ^ ((y & 1) ? 2567483615ULL : 0));
    }
}

