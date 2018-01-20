#include <mruby.h>
#include <mruby/string.h>
#include <mruby/irep.h>

#include "asicregs.h"

#include "intr.h"

void uart_init()
{
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
		if (reg & (0x20 << 24))
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