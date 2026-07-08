! { dg-do run }
!
! Test case contributed by Andrew Benson <abensonca@gmail.com>

module m
  implicit none
  type :: t
     integer :: i
  end type t
  type :: container
     class(t), allocatable :: arr(:)
  end type container
  type(t) :: scalar_mold
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
  integer :: r

  allocate (c)
  allocate (c%arr(1), mold = scalar_mold)

  ! Check the rank
  call get_rank (c%arr, r)
  if (r /= 1) stop 1

  deallocate (c)
end program main
