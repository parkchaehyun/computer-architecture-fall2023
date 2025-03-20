	.option nopic
	.attribute arch, "rv32i2p1_m2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	digitsum
	.type	digitsum, @function
	
digitsum:

	#------Your code starts here------
	#LHS: a0, RHS: a1
	addi x5, x0, 0
	addi x7, x0, 10
	Loop1:
	beq a0, x0, Loop2
	rem x6, a0, x7
	divu a0, a0, x7
	add x5, x5, x6
	beq x0, x0, Loop1
	Loop2:
	beq a1, x0, Exit
	rem x6, a1, x7
	divu a1, a1, x7
	add x5, x5, x6
	beq x0, x0, Loop2
	Exit:
	addi a0, x5, 0
	#Load return value to reg a0
	#------Your code ends here------

	#Ret
	jr	ra
	.size	digitsum, .-digitsum
	.ident	"GCC: (g2ee5e430018) 12.2.0"
