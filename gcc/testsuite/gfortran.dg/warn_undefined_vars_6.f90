! { dg-do compile }
! { dg-additional-options "-Wundefined-vars -std=legacy" }
! PR 126090 - warnings with STOP and friends
program memain
  character(8) :: s
  character(8) :: s1, s2, s3
  s = "abc"
  error stop s
  stop s
  pause s
  error stop s1 ! { dg-warning "Undefined variable" }
  stop s2 ! { dg-warning "Undefined variable" }
  pause s3 ! { dg-warning "Undefined variable" }
end program memain
