	.file	"main.c"
	.text
	.globl	call_and_ret
	.type	call_and_ret, @function
call_and_ret:
.LFB0:
	.cfi_startproc
	endbr64
	movl	(%rsi), %eax
	addl	(%rdi), %eax
	addl	(%rdx), %eax
	addl	(%rcx), %eax
	addl	(%r8), %eax
	cltq
	addq	(%r9), %rax
	addq	8(%rsp), %rax
	movslq	16(%rsp), %rdx
	addq	%rdx, %rax
	movswq	24(%rsp), %rdx
	addq	%rax, %rdx
	movsbq	32(%rsp), %rax
	addq	%rdx, %rax
	ret
	.cfi_endproc
.LFE0:
	.size	call_and_ret, .-call_and_ret
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	endbr64
	subq	$56, %rsp
	.cfi_def_cfa_offset 64
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	movl	$0, 16(%rsp)
	movl	$1, 20(%rsp)
	movl	$2, 24(%rsp)
	movl	$3, 28(%rsp)
	movl	$4, 32(%rsp)
	movq	$99, 8(%rsp)
	movl	$88, 4(%rsp)
	leaq	32(%rsp), %rcx
	leaq	28(%rsp), %rdx
	leaq	24(%rsp), %rsi
	leaq	20(%rsp), %rdi
	pushq	$44
	.cfi_def_cfa_offset 72
	pushq	$33
	.cfi_def_cfa_offset 80
	pushq	$22
	.cfi_def_cfa_offset 88
	pushq	$11
	.cfi_def_cfa_offset 96
	leaq	40(%rsp), %r9
	leaq	36(%rsp), %r8
	call	call_and_ret
	addq	$32, %rsp
	.cfi_def_cfa_offset 64
	movq	%rax, %rdi
	call	remote@PLT
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L5
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L5:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.2.0-19ubuntu1) 11.2.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
