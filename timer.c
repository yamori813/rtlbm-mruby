#include <sys/cdefs.h>

#include "intr.h"

int count = 0;

void Timer_isr(void)
{
unsigned long *lptr;
unsigned long reg;

	lptr = (unsigned long *)0xb8003114;
	reg = *lptr;
	*lptr = reg | 1 << 29;
	++count;
}

struct irqaction irq_Timer = {Timer_isr, (void *)NULL};

int timer_count1()
{
int *ptr;
	
	ptr = (unsigned long *)0xb8003108;
	return *ptr;
}

/* for lwip */

int sys_now()
{
        return count * 1000 + timer_count1() / 8388;
}

void timer_init()
{
unsigned long *lptr;

	lptr = (unsigned long *)0xb8003114;
	*lptr = *lptr | (1 << 31);
	lptr = (unsigned long *)0xb8003110;
	*lptr = *lptr | (1 << 31);
	lptr = (unsigned long *)0xb8003108;
	*lptr = (16 << 16);
	lptr = (unsigned long *)0xb8003100;
	*lptr = 0x00800000;
	request_IRQ(14, &irq_Timer, NULL);
}

