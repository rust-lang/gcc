! { dg-do compile }
! PR fortran/126018 - Stop code may be a function call (CHARACTER or INTEGER)
program p
  implicit none
  error stop character_stop ()   ! was: "must be either INTEGER or CHARACTER"
  stop integer_stop ()           ! likewise for integer
contains
  character (1) function character_stop ()
    character_stop = "a"
  end function character_stop
  integer function integer_stop ()
    integer_stop = 1
  end function integer_stop
end program p
