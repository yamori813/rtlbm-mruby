/*
 * Copyright (c) 2018 Hiroki Mori. All rights reserved.
 */

#include <mruby.h>
#include <mruby/irep.h>
#include <mruby/string.h>
#include <mruby/error.h>

#include "asicregs.h"

#define	MRBOFFSET	0x180000

extern char version[];

unsigned long memend();

int
main(int argc, char *argv[])
{
unsigned char hdrbuf[22];
int mrbsize;
unsigned char *mrbbuf;

	mt19937ar_init();

	uart_init();

	print(version);

	intr_init();

	timer_init();

	xprintf("MEMORY %dM\n", (memend() - 0x80000000) >> 20);

	spi_probe();

	flashread(hdrbuf, MRBOFFSET, sizeof(hdrbuf));
	if (hdrbuf[0x0] == 0x52 && hdrbuf[0x1] == 0x49 &&
	    hdrbuf[0x2] == 0x54 && hdrbuf[0x3] == 0x45) {
		mrbsize = (hdrbuf[0xa] << 24) | (hdrbuf[0xb] << 16) |
		    (hdrbuf[0xc] << 8) | hdrbuf[0xd];
		mrbbuf = malloc(mrbsize);
		flashread(mrbbuf, MRBOFFSET, mrbsize);

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

#define  REVR  0xB8000000
#define  RTL8196C_REVISION_A  0x80000001
#define  RTL8196C_REVISION_B  0x80000002
#define  RTL8198_REVISION_A  0xC0000000
#define  RTL8198_REVISION_B  0xC0000001

#define  RTL8196D_REVISION  0x8196d000
#define  RTL8196E_REVISION  0x8196e000
#define  RTL8197D_REVISION  0x8197c003

#define	MODULE_UNKNOWN				0
#define	MODULE_RTL8196C				1
#define	MODULE_BCM4712				2
#define	MODULE_RTL8196E				3
#define	MODULE_BCM5350				4
#define	MODULE_BCM5352				5
#define	MODULE_BCM5354				6
#define	MODULE_RTL8198				10
#define	MODULE_RTL8197D				11

int
getarch()
{
unsigned long rev = REG32(REVR);

	if (rev == RTL8196C_REVISION_A || rev == RTL8196C_REVISION_B)
		return MODULE_RTL8196C;
	else if (rev == RTL8198_REVISION_A || rev == RTL8198_REVISION_B)
		return MODULE_RTL8198;
	else if (rev == RTL8196E_REVISION)
		return MODULE_RTL8196E;
	else if (rev == RTL8197D_REVISION)
		return MODULE_RTL8197D;
	else {
		xprintf("REVR %x\n", rev);
		return MODULE_UNKNOWN;
	}
}

#define DCR_REG 0xb8001004

unsigned long memend()
{
unsigned long *reg;
unsigned long endaddr;

	reg = DCR_REG;
	switch(*reg) {
		case 0x54480000:    /* 32MB */
			endaddr = 0x82000000;
			break;
		case 0x52480000:    /* 16MB */
			endaddr = 0x81000000;
			break;
		case 0x52080000:    /* 8MB */
			endaddr = 0x80800000;
			break;
		default:    /* 8MB */
			endaddr = 0x80800000;
			break;
	}

	return endaddr;
}
