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
	.data
	.type	str, @object
	.size	str, 6
str:
	.string	"abc?\n"
	.text
	.globl	pp
	.type	pp, @function
pp:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -20(%rbp)
	movzbl	-20(%rbp), %eax
	movb	%al, 3+str(%rip)
	movl	$1, %eax
	leaq	str(%rip), %rcx
	movl	$1, %edi
	movl	$6, %edx
	movq	%rcx, %rsi
#APP
# 16 "test.cpp" 1
	syscall	

# 0 "" 2
#NO_APP
	movq	%rax, -8(%rbp)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	pp, .-pp
	.globl	nomain
	.type	nomain, @function
nomain:
.LFB1:
	.cfi_startproc
	endbr64
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
	movl	$119, %edi
	call	pp
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	nomain, .-nomain
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
