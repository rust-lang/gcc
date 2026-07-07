! { dg-do compile }
subroutine s1
  type t
    integer :: a(1,2) = 3
  end type
  type(t), parameter :: x(1) = t(4)
  integer, parameter :: y(1,2) = (x(1)%a(m,1)) ! { dg-error "does not reduce to a constant expression" }
  print *, y
end

subroutine s2
  type t
    integer :: a(2) = 3!
  end type
  type(t), parameter :: x(1) = t(4)
  integer, parameter :: y = x(1)%a(m) ! { dg-error "non-constant initialization expression" }
  print *, y
end

subroutine s3
  type t
    integer :: a(1,2) = 3
  end type
  type(t), parameter :: x(1) = t(4)
  integer, parameter :: y(1,2) = (x(b)%a) ! { dg-error "does not reduce to a constant expression" }
  print *, y
end

subroutine s4
  type t
    integer :: a(1,2) = 3
  end type
  type(t), parameter :: x(1) = t(4)
  integer :: y(1,2) = x(b)%a ! { dg-error "does not reduce to a constant expression" }
  print *, y
end
! { dg-prune-output "Legacy Extension: REAL array index" }
