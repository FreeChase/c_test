	.file	"do_print.c"
	.section .rdata,"dr"
LC0:
	.ascii "%u: %u %u\12\0"
	.text
	.globl	_do_print
	.def	_do_print;	.scl	2;	.type	32;	.endef
_do_print:
LFB13:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	$0, -12(%ebp)
	jmp	L2
L3:
	movl	8(%ebp), %eax
/APP
 # 11 ".\do_print.c" 1
	bsfl %eax,%eax
 # 0 "" 2
/NO_APP
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC0, (%esp)
	call	_printf
	addl	$1, -12(%ebp)
L2:
	cmpl	$4, -12(%ebp)
	jbe	L3
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE13:
	.ident	"GCC: (i686-posix-dwarf-rev1, Built by MinGW-W64 project) 4.9.2"
	.def	_printf;	.scl	2;	.type	32;	.endef
