!   Fortran wrappers of the degree trigonometric functions COSD, SIND, TAND
!   and their inverses ACOSD, ASIND, ATAND.
!   Copyright (C) 2026 Free Software Foundation, Inc.
!
!This file is part of the GNU Fortran runtime library (libgfortran).
!
!GNU libgfortran is free software; you can redistribute it and/or
!modify it under the terms of the GNU General Public
!License as published by the Free Software Foundation; either
!version 3 of the License, or (at your option) any later version.

!GNU libgfortran is distributed in the hope that it will be useful,
!but WITHOUT ANY WARRANTY; without even the implied warranty of
!MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!GNU General Public License for more details.
!
!Under Section 7 of GPL version 3, you are granted additional
!permissions described in the GCC Runtime Library Exception, version
!3.1, as published by the Free Software Foundation.
!
!You should have received a copy of the GNU General Public License and
!a copy of the GCC Runtime Library Exception along with this program;
!see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
!<http://www.gnu.org/licenses/>.


#include "config.h"
#include "kinds.inc"


#if defined (HAVE_GFC_REAL_4)

elemental function _gfortran_specific__acosd_r4 (parm)
   real (kind=4), intent (in) :: parm
   real (kind=4) :: _gfortran_specific__acosd_r4

   _gfortran_specific__acosd_r4 = acosd (parm)
end function

elemental function _gfortran_specific__asind_r4 (parm)
   real (kind=4), intent (in) :: parm
   real (kind=4) :: _gfortran_specific__asind_r4

   _gfortran_specific__asind_r4 = asind (parm)
end function

elemental function _gfortran_specific__atan2d_r4 (y, x)
   real (kind=4), intent (in) :: y, x
   real (kind=4) :: _gfortran_specific__atan2d_r4

   _gfortran_specific__atan2d_r4 = atan2d (y, x)
end function

elemental function _gfortran_specific__atand_r4 (parm)
   real (kind=4), intent (in) :: parm
   real (kind=4) :: _gfortran_specific__atand_r4

   _gfortran_specific__atand_r4 = atand (parm)
end function

elemental function _gfortran_specific__cosd_r4 (parm)
   real (kind=4), intent (in) :: parm
   real (kind=4) :: _gfortran_specific__cosd_r4

   _gfortran_specific__cosd_r4 = cosd (parm)
end function

elemental function _gfortran_specific__sind_r4 (parm)
   real (kind=4), intent (in) :: parm
   real (kind=4) :: _gfortran_specific__sind_r4

   _gfortran_specific__sind_r4 = sind (parm)
end function

elemental function _gfortran_specific__tand_r4 (parm)
   real (kind=4), intent (in) :: parm
   real (kind=4) :: _gfortran_specific__tand_r4

   _gfortran_specific__tand_r4 = tand (parm)
end function

#endif


#if defined (HAVE_GFC_REAL_8)

elemental function _gfortran_specific__acosd_r8 (parm)
   real (kind=8), intent (in) :: parm
   real (kind=8) :: _gfortran_specific__acosd_r8

   _gfortran_specific__acosd_r8 = acosd (parm)
end function

elemental function _gfortran_specific__asind_r8 (parm)
   real (kind=8), intent (in) :: parm
   real (kind=8) :: _gfortran_specific__asind_r8

   _gfortran_specific__asind_r8 = asind (parm)
end function

elemental function _gfortran_specific__atan2d_r8 (y, x)
   real (kind=8), intent (in) :: y, x
   real (kind=8) :: _gfortran_specific__atan2d_r8

   _gfortran_specific__atan2d_r8 = atan2d (y, x)
end function

elemental function _gfortran_specific__atand_r8 (parm)
   real (kind=8), intent (in) :: parm
   real (kind=8) :: _gfortran_specific__atand_r8

   _gfortran_specific__atand_r8 = atand (parm)
end function

elemental function _gfortran_specific__cosd_r8 (parm)
   real (kind=8), intent (in) :: parm
   real (kind=8) :: _gfortran_specific__cosd_r8

   _gfortran_specific__cosd_r8 = cosd (parm)
end function

elemental function _gfortran_specific__sind_r8 (parm)
   real (kind=8), intent (in) :: parm
   real (kind=8) :: _gfortran_specific__sind_r8

   _gfortran_specific__sind_r8 = sind (parm)
end function

elemental function _gfortran_specific__tand_r8 (parm)
   real (kind=8), intent (in) :: parm
   real (kind=8) :: _gfortran_specific__tand_r8

   _gfortran_specific__tand_r8 = tand (parm)
end function

#endif


#if defined (HAVE_GFC_REAL_10)

elemental function _gfortran_specific__acosd_r10 (parm)
   real (kind=10), intent (in) :: parm
   real (kind=10) :: _gfortran_specific__acosd_r10

   _gfortran_specific__acosd_r10 = acosd (parm)
end function

elemental function _gfortran_specific__asind_r10 (parm)
   real (kind=10), intent (in) :: parm
   real (kind=10) :: _gfortran_specific__asind_r10

   _gfortran_specific__asind_r10 = asind (parm)
end function

elemental function _gfortran_specific__atan2d_r10 (y, x)
   real (kind=10), intent (in) :: y, x
   real (kind=10) :: _gfortran_specific__atan2d_r10

   _gfortran_specific__atan2d_r10 = atan2d (y, x)
end function

elemental function _gfortran_specific__atand_r10 (parm)
   real (kind=10), intent (in) :: parm
   real (kind=10) :: _gfortran_specific__atand_r10

   _gfortran_specific__atand_r10 = atand (parm)
end function

elemental function _gfortran_specific__cosd_r10 (parm)
   real (kind=10), intent (in) :: parm
   real (kind=10) :: _gfortran_specific__cosd_r10

   _gfortran_specific__cosd_r10 = cosd (parm)
end function

elemental function _gfortran_specific__sind_r10 (parm)
   real (kind=10), intent (in) :: parm
   real (kind=10) :: _gfortran_specific__sind_r10

   _gfortran_specific__sind_r10 = sind (parm)
end function

elemental function _gfortran_specific__tand_r10 (parm)
   real (kind=10), intent (in) :: parm
   real (kind=10) :: _gfortran_specific__tand_r10

   _gfortran_specific__tand_r10 = tand (parm)
end function

#endif


#if defined (HAVE_GFC_REAL_16)

elemental function _gfortran_specific__acosd_r16 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__acosd_r16

   _gfortran_specific__acosd_r16 = acosd (parm)
end function

elemental function _gfortran_specific__asind_r16 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__asind_r16

   _gfortran_specific__asind_r16 = asind (parm)
end function

elemental function _gfortran_specific__atan2d_r16 (y, x)
   real (kind=16), intent (in) :: y, x
   real (kind=16) :: _gfortran_specific__atan2d_r16

   _gfortran_specific__atan2d_r16 = atan2d (y, x)
end function

elemental function _gfortran_specific__atand_r16 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__atand_r16

   _gfortran_specific__atand_r16 = atand (parm)
end function

elemental function _gfortran_specific__cosd_r16 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__cosd_r16

   _gfortran_specific__cosd_r16 = cosd (parm)
end function

elemental function _gfortran_specific__sind_r16 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__sind_r16

   _gfortran_specific__sind_r16 = sind (parm)
end function

elemental function _gfortran_specific__tand_r16 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__tand_r16

   _gfortran_specific__tand_r16 = tand (parm)
end function

#endif


#if defined (HAVE_GFC_REAL_17)

elemental function _gfortran_specific__acosd_r17 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__acosd_r17

   _gfortran_specific__acosd_r17 = acosd (parm)
end function

elemental function _gfortran_specific__asind_r17 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__asind_r17

   _gfortran_specific__asind_r17 = asind (parm)
end function

elemental function _gfortran_specific__atan2d_r17 (y, x)
   real (kind=16), intent (in) :: y, x
   real (kind=16) :: _gfortran_specific__atan2d_r17

   _gfortran_specific__atan2d_r17 = atan2d (y, x)
end function

elemental function _gfortran_specific__atand_r17 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__atand_r17

   _gfortran_specific__atand_r17 = atand (parm)
end function

elemental function _gfortran_specific__cosd_r17 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__cosd_r17

   _gfortran_specific__cosd_r17 = cosd (parm)
end function

elemental function _gfortran_specific__sind_r17 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__sind_r17

   _gfortran_specific__sind_r17 = sind (parm)
end function

elemental function _gfortran_specific__tand_r17 (parm)
   real (kind=16), intent (in) :: parm
   real (kind=16) :: _gfortran_specific__tand_r17

   _gfortran_specific__tand_r17 = tand (parm)
end function

#endif
