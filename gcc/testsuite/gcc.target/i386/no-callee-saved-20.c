/* { dg-do compile { target { *-*-linux* && lp64 } } } */
/* { dg-options "-O2 -fno-pic -mgeneral-regs-only -mtune=generic -mtune-ctrl=prologue_using_move,epilogue_using_move" } */
/* Keep labels and directives ('.cfi_startproc', '.cfi_endproc').  */
/* { dg-final { check-function-bodies "**" "" "" { target "*-*-*" } {^\t?\.}  } } */

__attribute__((no_callee_saved_registers, noipa))
void
foo (void)
{
}

/*
**bar:
**.LFB[0-9]+:
**	.cfi_startproc
**	subq	\$376, %rsp
**...
**	movq	%rax, 264\(%rsp\)
**	movq	%rdx, 272\(%rsp\)
**	movq	%rcx, 280\(%rsp\)
**	movq	%rbx, 288\(%rsp\)
**	movq	%rsi, 296\(%rsp\)
**	movq	%rdi, 304\(%rsp\)
**	movq	%r8, 312\(%rsp\)
**	movq	%r9, 320\(%rsp\)
**	movq	%r10, 328\(%rsp\)
**	movq	%r11, 336\(%rsp\)
**	movq	%r12, 344\(%rsp\)
**	movq	%r13, 352\(%rsp\)
**	movq	%r14, 360\(%rsp\)
**	movq	%r15, 368\(%rsp\)
**	movaps	%xmm0, \(%rsp\)
**	movaps	%xmm1, 16\(%rsp\)
**	movaps	%xmm2, 32\(%rsp\)
**	movaps	%xmm3, 48\(%rsp\)
**	movaps	%xmm4, 64\(%rsp\)
**	movaps	%xmm5, 80\(%rsp\)
**	movaps	%xmm6, 96\(%rsp\)
**	movaps	%xmm7, 112\(%rsp\)
**	movaps	%xmm8, 128\(%rsp\)
**	movaps	%xmm9, 144\(%rsp\)
**	movaps	%xmm10, 160\(%rsp\)
**	movaps	%xmm11, 176\(%rsp\)
**	movaps	%xmm12, 192\(%rsp\)
**	movaps	%xmm13, 208\(%rsp\)
**	movaps	%xmm14, 224\(%rsp\)
**	movaps	%xmm15, 240\(%rsp\)
**...
**	call	foo
**	movaps	\(%rsp\), %xmm0
**	movaps	16\(%rsp\), %xmm1
**	movaps	32\(%rsp\), %xmm2
**	movaps	48\(%rsp\), %xmm3
**	movaps	64\(%rsp\), %xmm4
**	movaps	80\(%rsp\), %xmm5
**	movaps	96\(%rsp\), %xmm6
**	movaps	112\(%rsp\), %xmm7
**	movaps	128\(%rsp\), %xmm8
**	movaps	144\(%rsp\), %xmm9
**	movaps	160\(%rsp\), %xmm10
**	movaps	176\(%rsp\), %xmm11
**	movaps	192\(%rsp\), %xmm12
**	movaps	208\(%rsp\), %xmm13
**	movq	264\(%rsp\), %rax
**	movq	272\(%rsp\), %rdx
**	movq	280\(%rsp\), %rcx
**	movq	288\(%rsp\), %rbx
**	movq	296\(%rsp\), %rsi
**	movq	304\(%rsp\), %rdi
**	movq	312\(%rsp\), %r8
**	movq	320\(%rsp\), %r9
**	movq	328\(%rsp\), %r10
**	movq	336\(%rsp\), %r11
**	movq	344\(%rsp\), %r12
**	movq	352\(%rsp\), %r13
**	movq	360\(%rsp\), %r14
**	movq	368\(%rsp\), %r15
**	movaps	224\(%rsp\), %xmm14
**	movaps	240\(%rsp\), %xmm15
**	addq	\$376, %rsp
**...
**	ret
**	.cfi_endproc
**...
*/

__attribute__((no_caller_saved_registers, target("sse2")))
void
bar (void)
{
  foo ();
}
