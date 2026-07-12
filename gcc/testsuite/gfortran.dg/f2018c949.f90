! { dg-do compile }
! { dg-options "-fcoarray=single" }
program main
  type :: type
    integer, allocatable :: a[:]
  end type

  type(type) :: x
  class(*), allocatable :: y

  allocate(x%a[*])
  allocate(y, source = x) !{ dg-error "coarray ultimate component" }
end
