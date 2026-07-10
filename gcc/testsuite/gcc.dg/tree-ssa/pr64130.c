/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-evrp-details" } */

__extension__ typedef __UINT32_TYPE__ uint32_t;

int funsigned (uint32_t a)
{
  if (a < 1) return 1;
  return (-1 * 0x1ffffffffL) / a == 0;
}

/* { dg-final { scan-tree-dump "int \\\[-8589934591, -2\\\]" "evrp" } } */

