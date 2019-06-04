	.file	"lr3.c"
	.text
	.globl	ex
	.type	ex, @function
ex:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -36(%rbp)
	movsd	.LC0(%rip), %xmm0	
	movsd	%xmm0, -16(%rbp)
	movsd	.LC0(%rip), %xmm0
	movsd	%xmm0, -24(%rbp)
	movq	$1, -8(%rbp)
	jmp	.L2
.L5:
	cvtsi2sd	-36(%rbp), %xmm0
	movapd	%xmm0, %xmm1
	mulsd	-24(%rbp), %xmm1
	movq	-8(%rbp), %rax
	testq	%rax, %rax
	js	.L3
	cvtsi2sdq	%rax, %xmm0
	jmp	.L4
.L3:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	cvtsi2sdq	%rdx, %xmm0
	addsd	%xmm0, %xmm0
.L4:
	divsd	%xmm0, %xmm1
	movapd	%xmm1, %xmm0
	movsd	%xmm0, -24(%rbp)
	movsd	-16(%rbp), %xmm0
	addsd	-24(%rbp), %xmm0
	movsd	%xmm0, -16(%rbp)
	addq	$1, -8(%rbp)
.L2:
	cmpq	$499999999, -8(%rbp)
	jbe	.L5
	movsd	-16(%rbp), %xmm0
	popq	%rbp
	ret
	.size	ex, .-ex
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$10, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	ex
	movq	%xmm0, %rax
	movq	%rax, -16(%rbp)
	movl	$0, %eax
	leave
	ret
	.size	main, .-main
	.section	.rodata
	.align 8
.LC0:
	.long	0
	.long	1072693248
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
