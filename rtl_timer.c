#include <sys/cdefs.h>
#include <time.h>

#include "asicregs.h"
#include "rtlregs.h"

#include "intr.h"

static unsigned long loops_per_jiffy;
static volatile unsigned int jiffies = 0;

void Timer_isr(void)
{
unsigned long *lptr;
unsigned long reg;

	lptr = (unsigned long *)0xb8003114;
	reg = *lptr;
	*lptr = reg | 1 << 29;

	net_poll();
	uart_poll();

	++jiffies;
}

struct irqaction irq_Timer = {Timer_isr, (void *)NULL};

/* for lwip */

int sys_now()
{
        return jiffies * 10;
}

int check_cpu_speed()
{
unsigned long ticks, loopbit;
int lps_precision = LPS_PREC;

	loops_per_jiffy = (1<<12);
	while (loops_per_jiffy <<= 1) {
		/* wait for "start of" clock tick */
		ticks = jiffies;
		while (ticks == jiffies)
			/* nothing */;
		/* Go .. */
		ticks = jiffies;
		__delay(loops_per_jiffy);
		ticks = jiffies - ticks;
		if (ticks)
			break;
	}

	/* Do a binary approximation to get loops_per_jiffy set to equal
	 one clock (up to lps_precision bits) */

	loops_per_jiffy >>= 1;
	loopbit = loops_per_jiffy;
	while ( lps_precision-- && (loopbit >>= 1) ) {
		loops_per_jiffy |= loopbit;
		ticks = jiffies;
		while (ticks == jiffies);
		ticks = jiffies;
		__delay(loops_per_jiffy);
		if (jiffies != ticks)   /* longer than 1 tick */
			loops_per_jiffy &= ~loopbit;
	}

	return ((loops_per_jiffy/(500000/HZ))+1);
}

void timer_init()
{
	request_IRQ(14, &irq_Timer, NULL);

	/* Set timer mode and Enable timer */
	REG32(TCCNR_REG) = (0<<31) | (0<<30);       //using time0

	REG32(CDBR_REG) = (DIVISOR) << DIVF_OFFSET;

	int SysClkRate = 200*1000*1000;

	REG32(TC0DATA_REG) = (((SysClkRate / DIVISOR) / TICK_FREQ) + 1) << 4;

	/* Set timer mode and Enable timer */
	REG32(TCCNR_REG) = (1<<31) | (1<<30);       //using time0

	/* We must wait n cycles for timer to re-latch the new value
	 of TC1DATA. */
	int c;  
	for( c = 0; c < DIVISOR; c++ ) ;

	/* Set interrupt routing register */
#ifdef RTL8196C
	REG32(IRR1_REG) = (4<<24);  // time0:IRQ4
#else  //RTL8196B, RTL8198
	REG32(IRR1_REG) = 0x00050004;  //uart:IRQ5,  time0:IRQ4
#endif    
    
	/* Enable timer interrupt */
	REG32(TCIR_REG) = (1<<31);

	char str[32];
	sprintf(str, "CPU %dMHz\r\n", check_cpu_speed());
	print(str);
}

void delay_ms(unsigned int time_ms)
{
   unsigned int preTime;
   
   preTime = jiffies;
   while ( jiffies - preTime <  time_ms/10 );
}

__inline__ void
__delay(unsigned long loops)
{
	__asm__ __volatile__ (
		".set\tnoreorder\n"
		"1:\tbnez\t%0,1b\n\t"
		"subu\t%0,1\n\t"
		".set\treorder"
		:"=r" (loops)
		:"0" (loops));
}

void udelay(int usec)
{
unsigned long max, val, val2, now, start;

	max = REG32(TC0DATA_REG) >> 4;
	val = max * usec / 10000;
	start = REG32(TC0CNT_REG) >> 4;
	while (1) {
		now = REG32(TC0CNT_REG) >> 4;
		if (now > start)
			val2 = start + max - now;
		else
			val2 = start - now;

		val2 = max - val2;

		if (val2 > val)
			break;
	}
}
