#include <string.h>

#include "system.h"
#include "rtlregs.h"
#include "asicregs.h"
#include "mipsregs.h"
#include "intr.h"

#define asmlinkage __attribute__((regparm(0)))

#define ALLINTS (IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5) 

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

unsigned int                                     
clear_cp0_status(unsigned int clear)                            
{                                                               
	unsigned int res;                                       
                                                          
	res = read_32bit_cp0_register(CP0_STATUS);              
	res &= ~clear;                                          
	write_32bit_cp0_register(CP0_STATUS, res);                              
}                                                               

unsigned int                                     
change_cp0_status(unsigned int change, unsigned int newvalue)   
{                                                               
	unsigned int res;                                       
                                                                
	res = read_32bit_cp0_register(CP0_STATUS);              
	res &= ~change;                                         
	res |= (newvalue & change);                                     
	write_32bit_cp0_register(CP0_STATUS, res);              
                                                        
	return res;                                             
}

extern void put(char *);

asmlinkage void dummy_handler(struct pt_regs *regs)
{
	print("unknown exception");
}

void set_except_vector(int n, void *addr)
{
	unsigned handler = (unsigned long) addr;
	exception_handlers[n] = handler;
}

void unmask_irq(unsigned int irq)
{
unsigned long *lptr;

	lptr = (unsigned long *)GIMR;

	*lptr = *lptr | (1 << irq);
	*lptr;
}

void mask_irq(unsigned int irq)
{
unsigned long *lptr;

	lptr = (unsigned long *)GIMR;

	*lptr = *lptr & ~(1 << irq);
	*lptr;
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
	} else {
		print("unknown interrupt");
		for(;;) ;
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
	clear_cp0_status(ST0_BEV);
	for (i = 0; i <= 31; i++)
		set_except_vector(i, dummy_handler);
	memcpy((void *)(0x80000000 + 0x80), &exception_matrix, 0x80);
	flush_cache();

	set_except_vector(0, irq_finder);
	change_cp0_status(ST0_IM, ALLINTS);

	sti();
}
