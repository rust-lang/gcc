! { dg-do run }
! PR103367, this test previously
! Test case from the PR segfaulted at compile time.
program p
  type inner
    integer :: n = 3
  end type
  type outer
    type(inner) :: a(2) = inner(1)
  end type
  type(outer), parameter :: x(1) = outer(inner(4))
  integer, parameter :: y(2) = x(1)%a%n
  if (any (y /= [4, 4])) stop 1
end
