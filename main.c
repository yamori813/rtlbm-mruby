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

void print(char *ptr)
{
	while(*ptr != '\0') {
		put(*ptr);
		++ptr;
	}
}

mrb_value myputs(mrb_state *mrb, mrb_value self){
	mrb_value val;
	mrb_get_args(mrb, "S", &val);
	print(RSTRING_PTR(val));
	return mrb_nil_value();
}

char eth0_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0xe8};

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

	timer_init();

	swCore_init();
	vlan_init();

	net_init();

	mrb_state *mrb;
	mrb = mrb_open();
	mrb_define_method(mrb, mrb->object_class, "myputs", myputs,
	    MRB_ARGS_REQ(1));
	mrb_load_irep( mrb, bytecode);
	mrb_close(mrb);

	return 1;
}

