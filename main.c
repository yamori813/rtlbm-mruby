/*
 * Copyright (c) 2018 Hiroki Mori. All rights reserved.
 */

#include <mruby.h>
#include <mruby/irep.h>
#include <mruby/string.h>
#include <mruby/error.h>

#include "intr.h"

extern char _end[];
extern char _fbss[];

#define	MRBOFFSET	0x100000

extern char version[];

int
main(int argc, char *argv[])
{
int i, j;
long *lptr;
unsigned char hdrbuf[22];
int mrbsize;
unsigned char *mrbbuf;

	/* bss clear */
	for (lptr = (long *)_fbss; lptr < (long *)_end; ++lptr) {
		 *lptr = 0;
	}

	unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
	init_by_array(init, length);

	uart_init();

	print(version);

	intr_init();

	timer_init();

	spi_probe();

	flashread(hdrbuf, MRBOFFSET, sizeof(hdrbuf));
	if (hdrbuf[0x0] == 0x52 && hdrbuf[0x1] == 0x49 &&
	    hdrbuf[0x2] == 0x54 && hdrbuf[0x3] == 0x45) {
		mrbsize = (hdrbuf[0xa] << 24) | (hdrbuf[0xb] << 16) |
		    (hdrbuf[0xc] << 8) | hdrbuf[0xd];
		mrbbuf = malloc(mrbsize);
		flashread(mrbbuf, 0x100000, mrbsize);

		mrb_state *mrb;
		mrb = mrb_open();
		mrb_load_irep( mrb, mrbbuf);
		if (mrb->exc) {
			mrb_value exc = mrb_obj_value(mrb->exc);
			mrb_value inspect = mrb_inspect(mrb, exc);
			print(mrb_str_to_cstr(mrb, inspect));
		}
		mrb_close(mrb);
	} else {
		print("can't find mrb code on flash\n");
	}

	return 1;
}

