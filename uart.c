/*
 * Copyright (c) 2018 Hiroki Mori. All rights reserved.
 */

#include "asicregs.h"

#include "intr.h"
#include "system.h"

#include "xprintf.h"

char rxbuff[1024];
int bufstart;
int bufend;

int getrxdata(char *buff, int len)
{
int datlen, datlen2;

	datlen = 0;
	if (bufstart != bufend) {
		cli();
		if (bufend > bufstart) {
			datlen = bufend - bufstart;
			datlen = datlen > len ? len : datlen;
			memcpy(buff, rxbuff + bufstart, datlen);
			bufstart = bufstart + datlen;
		} else {
			datlen = sizeof(rxbuff) - bufstart;
			if (len > datlen) {
				memcpy(buff, rxbuff + bufstart, datlen);
				datlen2 = bufend > len - datlen ? len - datlen :
				    bufend;
				memcpy(buff + datlen, rxbuff, datlen2);
				datlen += datlen2;
				bufstart = datlen2;
			} else {
				memcpy(buff, rxbuff + bufstart, len);
				datlen = len;
				bufstart += len;
			}
		}
		sti();
	}
	return datlen;
}

void uart_poll()
{
unsigned char *lsr;
unsigned char *prt;
unsigned char dat;

	lsr = (unsigned char *)UART_LSR_REG;
	while (*lsr & 0x01) {
		prt = (unsigned char *)UART_THR_REG;
		dat = *prt;
		rxbuff[bufend] = dat;
		++bufend;
		if (bufend == sizeof(rxbuff))
			bufend = 0;
		if (bufend == bufstart)
			print("rx buffer overflow\n");
	}
}

void put(char ch)
{
unsigned char *prt;
unsigned char reg;
int i;

	i = 0;
	prt = (unsigned char *)UART_LSR_REG;
	while (1) {
		++i;
		if (i >=0x6000)
			break;
		reg = *prt;
		if (reg & 0x20)
			break;
	}
	prt = (unsigned char *)UART_THR_REG;
	*prt = ch;
}

void print(char *ptr)
{
	while(*ptr != '\0') {
		put(*ptr);
		++ptr;
	}
}

void uart_init()
{
	/* Disable All Interrupts */
	REG32(UART_IER_REG) = 0x00000000;

	xfunc_out=put;

	bufstart = 0;
	bufend = 0;
#if 0
	/* GPIO register dump */
	dumpmem(0xB8000030, 32);
	dumpmem(0xB8003500, 32);
#endif
}
