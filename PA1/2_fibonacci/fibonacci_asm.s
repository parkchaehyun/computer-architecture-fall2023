	.option nopic
	.attribute arch, "rv32i2p1_m2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	fibonacci
	.type	fibonacci, @function
	
fibonacci:
	#------Your code starts here------
	# fibonacci number : a0
	addi sp, sp, -8
	sw x1, 4(sp)
	sw a0, 0(sp)
	addi x5, a0, -3
	bge x5, x0, C1
	addi a0, x0, 1
	addi sp, sp, 8
	jalr x0, 0(x1)
	
	C1:
	lw x5, 0(sp)
	addi x28, x5, -1
	addi a0, x28, 0
	jal x1, fibonacci
	addi sp, sp, -4
	sw a0, 0(sp)
	lw x5, 4(sp)
	addi x5, x5, -2
	addi a0, x5, 0
	jal x1, fibonacci
	lw x6, 0(sp)
	add a0, x6, a0
	lw x1, 8(sp)
	addi sp, sp, 12
	jalr x0, 0(x1)

	# Load return value to reg a0
	#------Your code ends here------

	jr	ra
	.size	fibonacci, .-fibonacci
	.ident	"GCC: (g2ee5e430018) 12.2.0"
