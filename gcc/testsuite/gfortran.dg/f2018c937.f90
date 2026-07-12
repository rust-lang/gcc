! { dg-do compile }
! { dg-options "-fcoarray=single" }
! https://mailman.j3-fortran.org/pipermail/j3/2021-August/013230.html
program foo
   type :: a_t
      integer, allocatable :: n[:]
   end type
   class(*), allocatable :: a
   allocate(a_t :: a)         ! { dg-error "coarray ultimate component" }
end program foo
