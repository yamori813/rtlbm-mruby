#include <string.h>

#include "rtlregs.h"
#include "intr.h"

#define asmlinkage __attribute__((regparm(0)))

unsigned long kernelsp;

static struct irqaction *irq_action[NR_IRQS] =
{
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

extern asmlinkage void irq_finder(void);

extern char exception_matrix;
unsigned long exception_handlers[32];

asmlinkage void dummy_handler(struct pt_regs *regs)
{
	put('m');
}

void set_except_vector(int n, void *addr)
{
	unsigned handler = (unsigned long) addr;
	exception_handlers[n] = handler;
}

static void  unmask_irq(unsigned int irq)
{
unsigned long *lptr;

	lptr = (unsigned long *)(0xb8000000 + GIMR0);

	*lptr = *lptr | (1 << irq);
}
static void  mask_irq(unsigned int irq)
{
unsigned long *lptr;

	lptr = (unsigned long *)(0xb8000000 + GIMR0);

	*lptr = *lptr & ~(1 << irq);
}

int request_IRQ(unsigned long irq, struct irqaction *action, void* dev_id)
{
struct irqaction **p;

	action->dev_id = dev_id;

	p = irq_action + irq;

	*p = action;

	unmask_irq(irq);

	return 0;
}

asmlinkage void do_IRQ(int irqnr, struct pt_regs *regs)
{
struct irqaction *action;

	action = *(irqnr + irq_action);
                
	if (action) 
	{
		action->handler(irqnr, action->dev_id, regs);
	}
}

void irq_dispatch(int irq_nr, struct pt_regs *regs)
{
	int i,irq=0;
	for (i=0; i<=31; i++) {
		if (irq_nr & 0x01) {
			do_IRQ(irq, regs);
		}
		irq++;
		irq_nr = irq_nr >> 1;
	}
}

void
intr_init()
{
int i;

	/* copy to exception handler */
	/* todo CP0 BEV=0 */
	for (i = 0; i <= 31; i++)
		set_except_vector(i, dummy_handler);
	memcpy((void *)(0x80000000 + 0x80), &exception_matrix, 0x80);
	/* todo flush cache */

	set_except_vector(0, irq_finder);
}
