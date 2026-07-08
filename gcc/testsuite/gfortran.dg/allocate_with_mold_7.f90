! { dg-do run }
!
module m
  implicit none
  type :: container
     class(*), allocatable :: arr(:)
  end type container
contains
  subroutine get_rank (x, r)
    class(*), intent(in) :: x(..)
    integer, intent(out) :: r
    r = rank (x)
  end subroutine get_rank
end module m

program main
  use m
  implicit none
  type(container), pointer :: c
  integer(1), allocatable :: junk(:)
  integer :: r

  allocate (c)
  allocate (c%arr(3), mold = 7)

  call get_rank (c%arr, r)
  if (r /= 1) stop 1

  select type (y => c%arr)
  type is (integer)
  class default
    stop 2
  end select

  deallocate (c)
end program main
