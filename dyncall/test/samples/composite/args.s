	.section	__TEXT,__text,regular,pure_instructions
	.globl	_call_f_swx
	.align	4, 0x90
_call_f_swx:                            ## @call_f_swx
Ltmp2:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp3:
	.cfi_def_cfa_offset 16
Ltmp4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp5:
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -16(%rbp)
	movaps	%xmm0, -32(%rbp)
	movaps	-32(%rbp), %xmm0
	movaps	-16(%rbp), %xmm1
	movaps	%xmm1, 16(%rsp)
	movaps	%xmm0, (%rsp)
	callq	_f_swx
	addq	$64, %rsp
	popq	%rbp
	ret
Ltmp6:
	.cfi_endproc
Leh_func_end0:

	.globl	_call_f_s2x
	.align	4, 0x90
_call_f_s2x:                            ## @call_f_s2x
Ltmp9:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp10:
	.cfi_def_cfa_offset 16
Ltmp11:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp12:
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -16(%rbp)
	movaps	%xmm0, -32(%rbp)
	movaps	-32(%rbp), %xmm0
	movaps	-16(%rbp), %xmm1
	movaps	%xmm1, 16(%rsp)
	movaps	%xmm0, (%rsp)
	callq	_f_s2x
	addq	$64, %rsp
	popq	%rbp
	ret
Ltmp13:
	.cfi_endproc
Leh_func_end1:

	.globl	_call_f_s1x
	.align	4, 0x90
_call_f_s1x:                            ## @call_f_s1x
Ltmp16:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp17:
	.cfi_def_cfa_offset 16
Ltmp18:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp19:
	.cfi_def_cfa_register %rbp
	xorps	%xmm0, %xmm0
	popq	%rbp
	jmp	_f_s1x                  ## TAILCALL
Ltmp20:
	.cfi_endproc
Leh_func_end2:

	.globl	_call_f_s4w
	.align	4, 0x90
_call_f_s4w:                            ## @call_f_s4w
Ltmp23:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp24:
	.cfi_def_cfa_offset 16
Ltmp25:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp26:
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -16(%rbp)
	movaps	%xmm0, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, 24(%rsp)
	movq	-16(%rbp), %rax
	movq	%rax, 16(%rsp)
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rcx
	movq	%rcx, 8(%rsp)
	movq	%rax, (%rsp)
	callq	_f_s4w
	addq	$64, %rsp
	popq	%rbp
	ret
Ltmp27:
	.cfi_endproc
Leh_func_end3:

	.globl	_call_f_s2w
	.align	4, 0x90
_call_f_s2w:                            ## @call_f_s2w
Ltmp30:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp31:
	.cfi_def_cfa_offset 16
Ltmp32:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp33:
	.cfi_def_cfa_register %rbp
	xorl	%edi, %edi
	xorl	%esi, %esi
	popq	%rbp
	jmp	_f_s2w                  ## TAILCALL
Ltmp34:
	.cfi_endproc
Leh_func_end4:

	.globl	_call_f_s1w
	.align	4, 0x90
_call_f_s1w:                            ## @call_f_s1w
Ltmp37:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp38:
	.cfi_def_cfa_offset 16
Ltmp39:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp40:
	.cfi_def_cfa_register %rbp
	xorl	%edi, %edi
	popq	%rbp
	jmp	_f_s1w                  ## TAILCALL
Ltmp41:
	.cfi_endproc
Leh_func_end5:

	.globl	_call_f_s2i
	.align	4, 0x90
_call_f_s2i:                            ## @call_f_s2i
Ltmp44:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp45:
	.cfi_def_cfa_offset 16
Ltmp46:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp47:
	.cfi_def_cfa_register %rbp
	xorl	%edi, %edi
	popq	%rbp
	jmp	_f_s2i                  ## TAILCALL
Ltmp48:
	.cfi_endproc
Leh_func_end6:

	.globl	_call_f_s4i
	.align	4, 0x90
_call_f_s4i:                            ## @call_f_s4i
Ltmp51:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp52:
	.cfi_def_cfa_offset 16
Ltmp53:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp54:
	.cfi_def_cfa_register %rbp
	xorl	%edi, %edi
	xorl	%esi, %esi
	popq	%rbp
	jmp	_f_s4i                  ## TAILCALL
Ltmp55:
	.cfi_endproc
Leh_func_end7:

	.globl	_call_f_s8i
	.align	4, 0x90
_call_f_s8i:                            ## @call_f_s8i
Ltmp58:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp59:
	.cfi_def_cfa_offset 16
Ltmp60:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp61:
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -16(%rbp)
	movaps	%xmm0, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, 24(%rsp)
	movq	-16(%rbp), %rax
	movq	%rax, 16(%rsp)
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rcx
	movq	%rcx, 8(%rsp)
	movq	%rax, (%rsp)
	callq	_f_s8i
	addq	$64, %rsp
	popq	%rbp
	ret
Ltmp62:
	.cfi_endproc
Leh_func_end8:

	.globl	_call_f_s2f
	.align	4, 0x90
_call_f_s2f:                            ## @call_f_s2f
Ltmp65:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp66:
	.cfi_def_cfa_offset 16
Ltmp67:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp68:
	.cfi_def_cfa_register %rbp
	xorps	%xmm0, %xmm0
	popq	%rbp
	jmp	_f_s2f                  ## TAILCALL
Ltmp69:
	.cfi_endproc
Leh_func_end9:

	.globl	_call_f_s4f
	.align	4, 0x90
_call_f_s4f:                            ## @call_f_s4f
Ltmp72:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp73:
	.cfi_def_cfa_offset 16
Ltmp74:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp75:
	.cfi_def_cfa_register %rbp
	xorps	%xmm0, %xmm0
	xorps	%xmm1, %xmm1
	popq	%rbp
	jmp	_f_s4f                  ## TAILCALL
Ltmp76:
	.cfi_endproc
Leh_func_end10:

	.globl	_call_f_s1d
	.align	4, 0x90
_call_f_s1d:                            ## @call_f_s1d
Ltmp79:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp80:
	.cfi_def_cfa_offset 16
Ltmp81:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp82:
	.cfi_def_cfa_register %rbp
	pxor	%xmm0, %xmm0
	popq	%rbp
	jmp	_f_s1d                  ## TAILCALL
Ltmp83:
	.cfi_endproc
Leh_func_end11:

	.globl	_call_f_s2d
	.align	4, 0x90
_call_f_s2d:                            ## @call_f_s2d
Ltmp86:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp87:
	.cfi_def_cfa_offset 16
Ltmp88:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp89:
	.cfi_def_cfa_register %rbp
	pxor	%xmm0, %xmm0
	pxor	%xmm1, %xmm1
	popq	%rbp
	jmp	_f_s2d                  ## TAILCALL
Ltmp90:
	.cfi_endproc
Leh_func_end12:

	.globl	_call_f_s4d
	.align	4, 0x90
_call_f_s4d:                            ## @call_f_s4d
Ltmp93:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp94:
	.cfi_def_cfa_offset 16
Ltmp95:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp96:
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -16(%rbp)
	movaps	%xmm0, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, 24(%rsp)
	movq	-16(%rbp), %rax
	movq	%rax, 16(%rsp)
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rcx
	movq	%rcx, 8(%rsp)
	movq	%rax, (%rsp)
	callq	_f_s4d
	addq	$64, %rsp
	popq	%rbp
	ret
Ltmp97:
	.cfi_endproc
Leh_func_end13:

	.globl	_call_f_s1q
	.align	4, 0x90
_call_f_s1q:                            ## @call_f_s1q
Ltmp100:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp101:
	.cfi_def_cfa_offset 16
Ltmp102:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp103:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -16(%rbp)
	movaps	-16(%rbp), %xmm0
	movaps	%xmm0, (%rsp)
	callq	_f_s1q
	addq	$32, %rsp
	popq	%rbp
	ret
Ltmp104:
	.cfi_endproc
Leh_func_end14:

	.globl	_call_f_s2ds2d
	.align	4, 0x90
_call_f_s2ds2d:                         ## @call_f_s2ds2d
Ltmp107:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp108:
	.cfi_def_cfa_offset 16
Ltmp109:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp110:
	.cfi_def_cfa_register %rbp
	pxor	%xmm0, %xmm0
	pxor	%xmm1, %xmm1
	pxor	%xmm2, %xmm2
	pxor	%xmm3, %xmm3
	popq	%rbp
	jmp	_f_s2ds2d               ## TAILCALL
Ltmp111:
	.cfi_endproc
Leh_func_end15:

	.globl	_call_f_sfifi
	.align	4, 0x90
_call_f_sfifi:                          ## @call_f_sfifi
Ltmp114:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp115:
	.cfi_def_cfa_offset 16
Ltmp116:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp117:
	.cfi_def_cfa_register %rbp
	movabsq	$4294967296, %rdi       ## imm = 0x100000000
	movabsq	$13958643712, %rsi      ## imm = 0x340000000
	popq	%rbp
	jmp	_f_sfifi                ## TAILCALL
Ltmp118:
	.cfi_endproc
Leh_func_end16:

	.globl	_call_f_sifif
	.align	4, 0x90
_call_f_sifif:                          ## @call_f_sifif
Ltmp121:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp122:
	.cfi_def_cfa_offset 16
Ltmp123:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp124:
	.cfi_def_cfa_register %rbp
	movabsq	$4575657221408423936, %rdi ## imm = 0x3F80000000000000
	movabsq	$4629700416936869890, %rsi ## imm = 0x4040000000000002
	popq	%rbp
	jmp	_f_sifif                ## TAILCALL
Ltmp125:
	.cfi_endproc
Leh_func_end17:

	.globl	_call_f_full_sifif
	.align	4, 0x90
_call_f_full_sifif:                     ## @call_f_full_sifif
Ltmp128:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp129:
	.cfi_def_cfa_offset 16
Ltmp130:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp131:
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	-8(%rbp), %rcx
	movq	%rcx, 24(%rsp)
	movq	%rax, 16(%rsp)
	movl	$7, 8(%rsp)
	movl	$6, (%rsp)
	movl	$0, %edi
	movl	$1, %esi
	movl	$2, %edx
	movl	$3, %ecx
	movl	$4, %r8d
	movl	$5, %r9d
	callq	_f_full_sifif
	addq	$48, %rsp
	popq	%rbp
	ret
Ltmp132:
	.cfi_endproc
Leh_func_end18:

	.globl	_call_f_remain64_sifif
	.align	4, 0x90
_call_f_remain64_sifif:                 ## @call_f_remain64_sifif
Ltmp135:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp136:
	.cfi_def_cfa_offset 16
Ltmp137:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp138:
	.cfi_def_cfa_register %rbp
	xorl	%edi, %edi
	movl	$1, %esi
	xorl	%edx, %edx
	xorl	%ecx, %ecx
	popq	%rbp
	jmp	_f_remain64_sifif       ## TAILCALL
Ltmp139:
	.cfi_endproc
Leh_func_end19:

	.section	__TEXT,__const
	.align	4
LCPI20_0:
	.long	0                       ## float 0.000000e+00
	.long	1065353216              ## float 1.000000e+00
	.space	4
	.space	4
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_call_f_sfffc
	.align	4, 0x90
_call_f_sfffc:                          ## @call_f_sfffc
Ltmp142:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp143:
	.cfi_def_cfa_offset 16
Ltmp144:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp145:
	.cfi_def_cfa_register %rbp
	movaps	LCPI20_0(%rip), %xmm0
	movabsq	$13958643712, %rdi      ## imm = 0x340000000
	popq	%rbp
	jmp	_f_sfffc                ## TAILCALL
Ltmp146:
	.cfi_endproc
Leh_func_end20:

	.section	__TEXT,__const
	.align	4
LCPI21_0:
	.long	1073741824              ## float 2.000000e+00
	.long	1077936128              ## float 3.000000e+00
	.space	4
	.space	4
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_call_f_scfff
	.align	4, 0x90
_call_f_scfff:                          ## @call_f_scfff
Ltmp149:
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp150:
	.cfi_def_cfa_offset 16
Ltmp151:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp152:
	.cfi_def_cfa_register %rbp
	movaps	LCPI21_0(%rip), %xmm0
	movabsq	$4575657221408423936, %rdi ## imm = 0x3F80000000000000
	popq	%rbp
	jmp	_f_scfff                ## TAILCALL
Ltmp153:
	.cfi_endproc
Leh_func_end21:


.subsections_via_symbols
