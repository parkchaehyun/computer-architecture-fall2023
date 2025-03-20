	.option nopic
	.attribute arch, "rv32i2p1_m2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	tree
	.type	tree, @function
tree:

	#------Your code starts here------
	# root tree node addr : a0, queue array addr : a1
	addi x5, x0, 0 #sum = 0;
	
	#queue[0] = *(root);
	lw x29, 8(a0)
	sw x29, 8(a1)
	lw x29, 4(a0)
	sw x29, 4(a1)
	lw x29, 0(a0)
	sw x29, 0(a1)
	
	addi x6, x0, 1 #cur=1;
	addi x7, x0, 0 #head=0;
	
	Loop:
	beq x6, x7, Exit #if(head==cur) exit loop
	addi x29, x0, 12
	mul x29, x29, x7
	add x30, a1, x29 #node=&queue[head]
	addi x7, x7, 1 #head+=1
	#if(node->left != NULL) 
	lw x31, 4(x30)
	beq x31, x0, C1
	addi x29, x0, 12
	mul x29, x29, x6
	add x28, a1, x29 #queue[cur]
	
	#queue[cur] = *(node->left);
	lw x29, 8(x31)
	sw x29, 8(x28)
	lw x29, 4(x31)
	sw x29, 4(x28)
	lw x29, 0(x31)
	sw x29, 0(x28)
	
	addi x6, x6, 1 #cur+=1
	
	C1: 
	#if(node->right != NULL) 
	lw x31, 8(x30)
	beq x31, x0, C2
	addi x29, x0, 12
	mul x29, x29, x6
	add x28, a1, x29 #queue[cur]
	
	#queue[cur] = *(node->right);
	lw x29, 8(x31)
	sw x29, 8(x28)
	lw x29, 4(x31)
	sw x29, 4(x28)
	lw x29, 0(x31)
	sw x29, 0(x28)
	
	addi x6, x6, 1 #cur+=1
	
	C2:
	#sum += node->val;
	lw x29, 0(x30)
	add x5, x5, x29
	beq x0, x0, Loop
	
	Exit:
	addi a0, x5, 0
	# Load return value to reg a0
	#------Your code ends here------

	jr	ra
	.size	tree, .-tree
	.ident	"GCC: (g2ee5e430018) 12.2.0"
