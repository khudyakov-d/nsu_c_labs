	.file	"lr3.c"
	.text
	.p2align 4,,15
	.globl	ex
	.type	ex, @function
ex:
	subl	$12, %esp
	movl	$1, %eax
	fildl	16(%esp)
	fld1
	fld1
	.p2align 4,,10
	.p2align 3
.L2:
	fmul	%st(2), %st
	movl	%eax, 4(%esp)
	addl	$1, %eax
	cmpl	$500000000, %eax
	fildl	4(%esp)
	fdivrp	%st, %st(1)
	fadd	%st, %st(1)
	jne	.L2
	fstp	%st(0)
	fstp	%st(1)
	addl	$12, %esp
	ret
	.size	ex, .-ex
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
	xorl	%eax, %eax
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
