#include <mruby.h>
#include <mruby/string.h>
#include <mruby/irep.h>

#include "intr.h"

#include "hoge.c"

extern char _end[];
extern char _fbss[];

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

	/* bss clear */
	for (lptr = (long *)_fbss; lptr < (long *)_end; ++lptr) {
		 *lptr = 0;
	}

	uart_init();

	intr_init();

	timer_init();

	delay_ms(10);
	swCore_init();
	delay_ms(10);
	vlan_init();
	delay_ms(10);
	net_init();
	delay_ms(10);

	mrb_state *mrb;
	mrb = mrb_open();
	mrb_define_method(mrb, mrb->object_class, "myputs", myputs,
	    MRB_ARGS_REQ(1));
	mrb_load_irep( mrb, bytecode);
	mrb_close(mrb);

	return 1;
}

