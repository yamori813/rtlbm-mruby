#include <mruby.h>
#include <mruby/string.h>
#include <mruby/irep.h>

#include "intr.h"

#include "hoge.c"

extern char _end[];
extern char _fbss[];

void put(char ch)
{
unsigned char *prt;
unsigned char reg;
int i;

	i = 0;
	prt = (unsigned char *)0xb8002014;
	while (1) {
		++i;
		if (i >=0x6000)
			break;
		reg = *prt;
		if (reg & 0x20)
			break;
	}
	prt = (unsigned char *)0xb8002000;
	*prt = ch;
}

mrb_value myputs(mrb_state *mrb, mrb_value self){
	char *ptr;
	mrb_value val;
	mrb_get_args(mrb, "S", &val);
	for (ptr = RSTRING_PTR(val); *ptr != '\0'; ++ptr) {
		put(*ptr);
	}
	put('\r');
	put('\n');
	return mrb_nil_value();
}

int count = 0;

void Timer_isr(void)
{
unsigned long *lptr;
unsigned long reg;

	/* uart intr disable */
	lptr = (unsigned long *)0xb8003114;
	reg = *lptr;
	*lptr = reg | 1 << 29;
	++count;
	put('T');
}

struct irqaction irq_Timer = {Timer_isr, (void *)NULL};

int
main(int argc, char *argv[])
{
int i;
unsigned char *ptr;
long *lptr;

	/* uart intr disable */
	ptr = (unsigned char *)0xb8002004;
	*ptr = 0;

	/* bss clear */
	for (lptr = (long *)_fbss; lptr < (long *)_end; ++lptr) {
		 *lptr = 0;
	}

	intr_init();

	lptr = (unsigned long *)0xb8003114;
	*lptr = *lptr | (1 << 31);
	lptr = (unsigned long *)0xb8003110;
	*lptr = *lptr | (1 << 31);
	lptr = (unsigned long *)0xb8003108;
	*lptr = (16 << 16);
	lptr = (unsigned long *)0xb8003100;
	*lptr = 0x8000000;
	request_IRQ(14, &irq_Timer, NULL);

	mrb_state *mrb;
	mrb = mrb_open();
	mrb_define_method(mrb, mrb->object_class, "myputs", myputs,
	    MRB_ARGS_REQ(1));
	mrb_load_irep( mrb, bytecode);
	mrb_close(mrb);

	return 1;
}
