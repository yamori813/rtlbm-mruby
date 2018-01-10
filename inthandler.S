#include <sys/regdef.h>
#include "stackframe.h"
#include "offset.h"

#include "rtlregs.h"

	.globl	irq_dispatch
	.text
	.globl	irq_finder
	.ent	irq_finder
	.set	reorder
irq_finder: 
	SAVE_ALL
	CLI
	mfc0	t1, CP0_CAUSE
	nop
	nop
	mfc0	t2, CP0_STATUS
	nop
	nop
	and	t0, t2 
	and	t2, t0, 0xfc00
	la      t0, (GISR  + 0xb8000000)
	la      t1, (GIMR0 + 0xb8000000)
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
	.end irq_finder
