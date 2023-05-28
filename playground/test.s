	.file	"test.cpp"
	.text
#APP
	.text
.global Add
.type Add, @function
Add:
leal 3(%rdi,%rsi,8), %eax
ret
#NO_APP
	.globl	str
	.section	.rodata
.LC0:
	.string	"abcc"
	.section	.data.rel.local,"aw",@progbits
	.align 8
	.type	str, @object
	.size	str, 8
str:
	.quad	.LC0
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$1, -12(%rbp)
	movl	$2, -8(%rbp)
	movl	-8(%rbp), %edx
	movl	-12(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	Add@PLT
	movl	%eax, -4(%rbp)
	movq	str(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
