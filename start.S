#define INITIAL_SP	0x80800000
#define CP0_CAUSE	$13

	.globl  main
	.globl  _init

	.text
	.globl	start
	.ent	start
	.set	reorder
start:
	li      $sp, INITIAL_SP
	nop
# bss clear
	la	$8,_fbss
	la	$9,_end
1:	sw	$0,0($8)
	add	$8,4
	blt	$8,$9,1b
	/* allocate space for result */
	addiu	$sp, $sp, -8
	jal	main
	/* store the result */
	sw	$2, 4($sp)
	addiu	$sp, $sp, 8
	/* watchdog reset */
	li	$3, 0xb800311c
	li	$4, 0
	sb	$4,0($3)
loop:	
	j	loop
	.end start

	.globl	exception_matrix
	.ent	exception_matrix
	.set	reorder
exception_matrix:
	mfc0	$27, CP0_CAUSE   
	nop
	nop
	la	$26, exception_handlers
	andi    $27, $27, 0x7c
	addu    $26, $26, $27
	lw      $26, ($26)
	jr      $26
	nop
	nop
	.end exception_matrix

