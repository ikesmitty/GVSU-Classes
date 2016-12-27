	.file	"timestamp_shell.c"
	.section	.rodata
	.align 8
.LC0:
	.string	"%8d; Start %10u; Stop %10u; Difference %5d\n"
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
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	.cfi_offset 3, -40
	movl	$100, %r12d
	movl	$200, %r13d
	movl	$-1, %ebx
	movl	$0, %ebx
	jmp	.L2
.L3:
	rdtsc
	movl	%eax, %r12d
  addl $1, %ecx
  addl $1, %ecx
  addl $1, %ecx
  addl $1, %ecx
  addl $1, %ecx
  addl $1, %ecx
  addl $1, %ecx
  addl $1, %ecx
  addl $1, %ecx
  addl $1, %ecx
	rdtsc
	movl 	%eax, %r13d

	movl	%r13d, %eax
	subl	%r12d, %eax
	movl	%eax, %r8d
	movl	%r13d, %ecx
	movl	%r12d, %edx
	movl	%ebx, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	addl	$1, %ebx
.L2:
	cmpl	$99999, %ebx
	jle	.L3
	movl	$199, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-4)"
	.section	.note.GNU-stack,"",@progbits
