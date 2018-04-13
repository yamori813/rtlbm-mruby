


#define	PIN_MUX_SEL	0xb8000040
#define PIN_MUX_SEL2	0xb800003c
#define	PABCD_CNR	0xb8003500
#define	PABCD_DIR	0xb8003508
#define	PABCD_DAT	0xb800350c
#define	PABCD_ISR	0xb8003510
#define	PAB_IMR		0xb8003514
#define	PCD_IMR		0xb8003518

void gpio_init(unsigned long sel, unsigned long sel2)
{
unsigned long *lptr;

	lptr = (unsigned long *)PIN_MUX_SEL;
	*lptr = *lptr | sel;

	lptr = (unsigned long *)PIN_MUX_SEL2;
	*lptr = *lptr | sel2;
}

unsigned long gpio_getctl()
{
unsigned long *lptr;

	lptr = (unsigned long *)PABCD_CNR;

	return *lptr;
}

void gpio_setctl(unsigned long val)
{
unsigned long *lptr;

	lptr = (unsigned long *)PABCD_CNR;

	*lptr = val;
}

unsigned long gpio_getdir()
{
unsigned long *lptr;

	lptr = (unsigned long *)PABCD_DIR;

	return *lptr;
}

void gpio_setdir(unsigned long val)
{
unsigned long *lptr;

	lptr = (unsigned long *)PABCD_DIR;

	*lptr = val;
}

unsigned long gpio_getdat()
{
unsigned long *lptr;

	lptr = (unsigned long *)PABCD_DAT;

	return *lptr;
}

void gpio_setdat(unsigned long val)
{
unsigned long *lptr;

	lptr = (unsigned long *)PABCD_DAT;

	*lptr = val;
}
