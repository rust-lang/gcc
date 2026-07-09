/* Copyright (C) 2013-2026 Free Software Foundation, Inc.
   Contributed by Andreas Krebbel <krebbel@linux.vnet.ibm.com>

   This file is part of the GNU Transactional Memory Library (libitm).

   Libitm is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Libitm is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */


#include "asmcfi.h"

.text
	.align 4
.globl	_ITM_beginTransaction
	.type _ITM_beginTransaction, @function

	/* _ITM_beginTransaction (int props); props -> r2 */
_ITM_beginTransaction:
	cfi_startproc
	lgr	%r3,%r15	/* backup stack pointer */
	aghi	%r15,-304	/* jump buffer (144) + reg save area (160) */
	cfi_adjust_cfa_offset(304)
	stmg	%r6,%r14,160(%r15)
	stg	%r3,72+160(%r15) /* store the old stack pointer */
	std	%f8,80+160(%r15)
	std	%f9,88+160(%r15)
	std	%f10,96+160(%r15)
	std	%f11,104+160(%r15)
	std	%f12,112+160(%r15)
	std	%f13,120+160(%r15)
	std	%f14,128+160(%r15)
	std	%f15,136+160(%r15)
	la	%r3,160(%r15)	/* second argument to GTM_begin_transaction */
	brasl	%r14,GTM_begin_transaction
	lg	%r1,64+160(%r15)
	aghi	%r15,304
	cfi_adjust_cfa_offset(-304)
	br	%r1
	cfi_endproc

	.size _ITM_beginTransaction, .-_ITM_beginTransaction

	.align 4
.globl	GTM_longjmp
	.type GTM_longjmp, @function
	.hidden GTM_longjmp

	/* uint32_t GTM_longjmp (uint32_t, const gtm_jmpbuf *, uint32_t) */
GTM_longjmp:
	/* First parameter becomes the return value of
	   _ITM_beginTransaction (r2).
	   Third parameter is ignored for now.  */
	cfi_startproc
	ld	%f8,80(%r3)
	ld	%f9,88(%r3)
	ld	%f10,96(%r3)
	ld	%f11,104(%r3)
	ld	%f12,112(%r3)
	ld	%f13,120(%r3)
	ld	%f14,128(%r3)
	ld	%f15,136(%r3)
	lmg	%r6,%r15,0(%r3)
	br	%r14
	cfi_endproc

	.size GTM_longjmp, .-GTM_longjmp

	.section .note.GNU-stack, "", @progbits
