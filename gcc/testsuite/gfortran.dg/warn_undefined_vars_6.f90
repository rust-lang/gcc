! { dg-do compile }
! { dg-additional-options "-Wundefined-vars -std=legacy" }
! PR 126090 - warnings with STOP and friends
program memain
  character(8) :: s
  character(8) :: s1, s2, s3
  s = "abc"
  error stop s
  stop s
  pause s ! { dg-warning "Deleted feature" }
  error stop s1 ! { dg-warning "Undefined variable" }
  stop s2 ! { dg-warning "Undefined variable" }
  pause s3 ! { dg-warning "Undefined variable" }
  ! { dg-warning "Deleted feature" " " { target "*-*-*" } .-1 }
end program memain
