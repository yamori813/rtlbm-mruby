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

	unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	uart_init();

	intr_init();

	timer_init();

	spi_probe();

	net_init();

	mrb_state *mrb;
	mrb = mrb_open();
	mrb_load_irep( mrb, bytecode);
	mrb_close(mrb);

	return 1;
}

