#include <sys/regdef.h>
#include "stackframe.h"
#include "offset.h"

#include "rtlregs.h"

	.globl	irq_dispatch
	.text
	.set	noreorder
	.align	5
	.globl	irq_finder
	.align	2; 
	.type	irq_finder,@function;
	.ent	irq_finder,0;
irq_finder: .frame  sp, PT_SIZE, sp
	SAVE_ALL
	CLI
	.set	at
	mfc0	t1, CP0_CAUSE
	nop
	nop

	mfc0	t2, CP0_STATUS
	nop
	nop

	and	t0, t2 
	and	t2, t0, 0xfc00

	la      t0, GISR
	la      t1, GIMR
	lw      a0, 0(t0)
	lw      a1, 0(t1)
	nop
	nop

	and     a0, a1
	and     a0, 0xffffffff
	bne     t2, zero, handle_it
	nop
	nop

handle_it:
	jal	irq_dispatch
	nop
	nop
	move	a1,sp
	RESTORE_ALL_AND_RET
	nop
	nop
	.end irq_finder
	.set	at
