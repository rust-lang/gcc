! { dg-do run }
! PR103367 Test case from the PR, previously segfaulted.
program p
  type t
     integer :: a(1,2) = 3
  end type
  type(t), parameter :: x(1) = t(4)
  integer, parameter :: y(2) = x(1)%a(1,:)
  if (any (y /= [4, 4])) stop 1
end

