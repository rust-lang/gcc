/* A multi-target build changes nothing about a default compilation;
   multi-target.exp also compares this file's output against an
   explicit selection of the primary.  */
/* { dg-do compile } */

int f (void)
{
  return 42;
}
