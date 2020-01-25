/*
 * Copyright (c) 2018 Hiroki Mori. All rights reserved.
 *
 * I2C Bitbang on gpio
 */

#define	HIGH	1
#define	LOW	0

#define	IN	0
#define	OUT	1

void udelay(int usec);

unsigned long gpio_getctl();
void gpio_setctl(unsigned long val);
unsigned long gpio_getdir();
void gpio_setdir(unsigned long val);
unsigned long gpio_getdat();
void gpio_setdat(unsigned long val);

int sclpin;
int sdapin;
int udel;

#define	DELAY	udelay(udel)

static void setscl(int val)
{
	unsigned long reg;

	reg = gpio_getdat();

	reg = (reg & ~(1 << sclpin)) | (val << sclpin);

	gpio_setdat(reg);
}

static void setsda(int val)
{
	unsigned long reg;

	reg = gpio_getdat();

	reg = (reg & ~(1 << sdapin)) | (val << sdapin);

	gpio_setdat(reg);
}

static int readsda()
{
	unsigned long reg;

	reg = gpio_getdat();

	return (reg & (1 << sdapin)) ? HIGH : LOW;
}

static int readscl()
{
	unsigned long reg;

	reg = gpio_getdat();

	return (reg & (1 << sclpin)) ? HIGH : LOW;
}

static void setdir(int val)
{
	unsigned long reg;

	reg = gpio_getdir();

	reg = (reg & ~(1 << sdapin)) | (val << sdapin);

	gpio_setdir(reg);
}

static void setscldir(int val)
{
	unsigned long reg;

	reg = gpio_getdir();

	reg = (reg & ~(1 << sclpin)) | (val << sclpin);

	gpio_setdir(reg);
}

void i2c_init(int scl, int sda, int u)
{
	unsigned long reg;

	sclpin = scl;
	sdapin = sda;
	udel = u;

	reg = gpio_getctl();
	reg &= ~((1 << scl) | (1 << sda));
	gpio_setctl(reg);
	reg = gpio_getdir();
	reg = (reg & ~(1 << sclpin)) | (OUT << sclpin);
	gpio_setdir(reg);

	setscl(HIGH);
	setdir(OUT);
	setsda(HIGH);
}

int i2c_write(unsigned char ch, int start, int stop)
{
int i, res;

	res = 0;
	setdir(OUT);
	if(start)
	setsda(LOW);
	DELAY;
	for (i = 0;i < 8; ++i) {
		setscl(LOW);
		setsda((ch & (1 << (7 - i))) ? HIGH : LOW);
		DELAY;
		setscl(HIGH);
		DELAY;
	}
	setdir(IN);
	setscl(LOW);
	DELAY;
	setscl(HIGH);
	for (i = 0;i < 1000; ++i) {
		if (readsda() == LOW) {
			res = 1;
			break;
		}
	}
	DELAY;
	setscl(LOW);
	DELAY;
	if (stop || res == 0) {
		setdir(OUT);
		setsda(LOW);
		setscl(HIGH);
		DELAY;
		setsda(HIGH);
	}

	return res;
}

unsigned char i2c_read(int stop)
{
int i;
unsigned char ch;

	ch = 0;
	/* Clock stretching */
	setscldir(IN);
	while (readscl() == 0)
		DELAY;
	setscl(HIGH);
	setscldir(OUT);
	setdir(IN);
	for (i = 0; i < 8; ++i) {
		if (i != 0) {
			setscl(LOW);
			DELAY;
			setscl(HIGH);
		}
		ch |= readsda() << (7 - i);
		DELAY;
	}
	setscl(LOW);
	setdir(OUT);
	if (stop)
		setsda(HIGH);
	else
		setsda(LOW);
	DELAY;
	setscl(HIGH);
	DELAY;
	setscl(LOW);
	DELAY;
	if (stop) {
		setscl(HIGH);
		DELAY;
		setsda(HIGH);
		DELAY;
	}

	return ch;
}

