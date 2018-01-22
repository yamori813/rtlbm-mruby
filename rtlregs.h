
#define	INITIAL_SP		0x80800000
#define	CP0_STATUS		$12 
#define	CP0_CAUSE		$13
#define	CP0_EPC			$14
#define	ST0_CU0			0x10000000

#define PIN_MUX_SEL 		0xb8000040

#define	GIMR			0xb8003000
#define	GIMR_REG		0xb8003000
#define	GISR			0xb8003004
#define	IRR1			0xb800300c
#define	IRR1_REG		0xb800300c
#define	IRR2			0xb8003010
#define	IRR3			0xb8003014

#define	NR_IRQS			64

#define DESC_OWNED_BIT		(1 << 0)
#define DESC_WRAP		(1 << 1)
#define DESC_SWCORE_OWNED	(1 << 0)

/* Timer control registers 
*/
#define GICR_BASE		0xB8003000
#define TC0DATA_REG		(0x100 + GICR_BASE)  /* Timer/Counter 0 data */
#define TC1DATA_REG		(0x104 + GICR_BASE)  /* Timer/Counter 1 data */
#define TC0CNT_REG		(0x108 + GICR_BASE)  /* Timer/Counter 0 count */
#define TC1CNT_REG		(0x10C + GICR_BASE)  /* Timer/Counter 1 count */
#define TCCNR_REG		(0x110 + GICR_BASE)  /* Timer/Counter control */
#define TCIR_REG		(0x114 + GICR_BASE)  /* Timer/Counter intertupt */
#define CDBR_REG		(0x118 + GICR_BASE)   /* Clock division base */
#define WDTCNR_REG		(0x11C + GICR_BASE)   /* Watchdog timer control */


#define DIVISOR			0xE
#define DIVF_OFFSET		16          

#define TICK_10MS_FREQ		100 /* 100 Hz */
#define TICK_100MS_FREQ		1000 /* 1000 Hz */
#define TICK_FREQ		TICK_10MS_FREQ  

#define LPS_PREC		8
#define HZ			100
