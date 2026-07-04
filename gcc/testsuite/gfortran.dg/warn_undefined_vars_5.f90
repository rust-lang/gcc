! { dg-do compile }
! { dg-additional-options "-Wundefined-vars" }
program memain
  implicit none
  integer :: n, m
  namelist /n1/ n
  namelist /n2/ m
  write (*,n1) ! { dg-warning "Undefined variable" }
  write (*,nml=n2) ! { dg-warning "Undefined variable" }
end program memain
