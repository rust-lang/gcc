/* The selection's own size type: unsigned int on m68k, where the
   primary's is long unsigned int.  */
/* { dg-do compile } */

extern __SIZE_TYPE__ size_object;
extern unsigned int size_object;
