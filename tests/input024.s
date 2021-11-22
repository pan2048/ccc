	.text
	.file 1 "input024.c"
# internal switch(expr) routine
# %rsi = switch table, %rax = expr
# from SubC: http://www.t3x.org/subc/

__switch:
        pushq   %rsi
        movq    %rdx, %rsi
        movq    %rax, %rbx
        cld
        lodsq
        movq    %rax, %rcx
__next:
        lodsq
        movq    %rax, %rdx
        lodsq
        cmpq    %rdx, %rbx
        jnz     __no
        popq    %rsi
        jmp     *%rax
__no:
        loop    __next
        lodsq
        popq    %rsi
        jmp     *%rax

	.data
	.globl	a
a:
	.long	0
	.globl	b
b:
	.long	0
	.globl	c
c:
	.long	0
L3:
	.byte	37
	.byte	100
	.byte	10
	.byte	0
L4:
	.byte	37
	.byte	100
	.byte	10
	.byte	0
L5:
	.byte	37
	.byte	100
	.byte	10
	.byte	0
L6:
	.byte	37
	.byte	100
	.byte	10
	.byte	0
L7:
	.byte	37
	.byte	100
	.byte	10
	.byte	0
	.loc 1 6 0
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	addq	$0,%rsp
	.loc 1 7 0
	movq	$42, %r10
	movl	%r10d, a(%rip)
	movq	$19, %r10
	movl	%r10d, b(%rip)
	.loc 1 8 0
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	movslq	a(%rip), %r10
	movslq	b(%rip), %r11
	andq	%r11, %r10
	movq	%r10, %rsi
	leaq	L3(%rip), %r10
	movq	%r10, %rdi
	call	printf@PLT
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	movq	%rax, %r10
	.loc 1 9 0
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	movslq	a(%rip), %r10
	movslq	b(%rip), %r11
	orq	%r11, %r10
	movq	%r10, %rsi
	leaq	L4(%rip), %r10
	movq	%r10, %rdi
	call	printf@PLT
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	movq	%rax, %r10
	.loc 1 10 0
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	movslq	a(%rip), %r10
	movslq	b(%rip), %r11
	xorq	%r11, %r10
	movq	%r10, %rsi
	leaq	L5(%rip), %r10
	movq	%r10, %rdi
	call	printf@PLT
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	movq	%rax, %r10
	.loc 1 11 0
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	movq	$1, %r10
	movq	$3, %r11
	movb	%r11b, %cl
	shlq	%cl, %r10
	movq	%r10, %rsi
	leaq	L6(%rip), %r10
	movq	%r10, %rdi
	call	printf@PLT
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	movq	%rax, %r10
	.loc 1 12 0
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	movq	$63, %r10
	movq	$3, %r11
	movb	%r11d, %cl
	shrq	%cl, %r10
	movq	%r10, %rsi
	leaq	L7(%rip), %r10
	movq	%r10, %rdi
	call	printf@PLT
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	movq	%rax, %r10
	.loc 1 13 0
	movq	$0, %r10
	movl	%r10d, %eax
	jmp	L2
L2:
	addq	$0,%rsp
	popq	%rbp
	ret
