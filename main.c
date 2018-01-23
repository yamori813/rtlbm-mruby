#include <mruby.h>
#include <mruby/irep.h>

#include "intr.h"

#include "hoge.c"

extern char _end[];
extern char _fbss[];

int
main(int argc, char *argv[])
{
int i;
long *lptr;

	/* bss clear */
	for (lptr = (long *)_fbss; lptr < (long *)_end; ++lptr) {
		 *lptr = 0;
	}

	uart_init();

	intr_init();

	timer_init();

	net_init();

	mrb_state *mrb;
	mrb = mrb_open();
	mrb_load_irep( mrb, bytecode);
	mrb_close(mrb);

	return 1;
}

