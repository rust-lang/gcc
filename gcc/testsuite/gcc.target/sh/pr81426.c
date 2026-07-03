/* { dg-do compile { target { atomic_model_soft_gusa_available } } }  */
/* { dg-options "-O2 -fPIC -matomic-model=soft-gusa,strict" }  */

inline int a (char *b, char c)
{
  __sync_val_compare_and_swap (b, c, 0);
}

int d(void)
{
  while (1)
  {
    char e;
    a(&e, e);
  }
}