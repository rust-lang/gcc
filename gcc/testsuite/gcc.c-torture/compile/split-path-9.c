/* PR77366 */

void
foo(unsigned int size, unsigned int *state)
{
  unsigned int i;

  for(i = 0; i < size; i++)
    {
      if(state[i] & 1)
	state[i] ^= 1;
    }
}

