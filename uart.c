#include <mruby.h>
#include <mruby/string.h>
#include <mruby/irep.h>

#include "asicregs.h"

#include "intr.h"

void uart_init()
{
	/* 8 bits, 1 stop bit, no parity. */
	REG32(UART_LCR_REG) = 0x03000000;

	/* Reset/Enable the FIFO */
	REG32(UART_FCR_REG) = 0xc7000000;

	/* Disable All Interrupts */
	REG32(UART_IER_REG) = 0x00000000;
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
