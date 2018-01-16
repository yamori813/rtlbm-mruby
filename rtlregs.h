
#define	INITIAL_SP	0x80800000
#define	CP0_STATUS	$12 
#define	CP0_CAUSE	$13
#define	CP0_EPC		$14
#define	ST0_CU0		0x10000000

#define        GIMR            0xb8003000
#define        GIMR_REG        0xb8003000
#define        GISR            0xb8003004
#define        IRR1            0xb800300c
#define        IRR2            0xb8003010
#define        IRR3            0xb8003014

#define	NR_IRQS		64

#define DESC_OWNED_BIT                                   (1 << 0)
#define DESC_WRAP                                                (1 << 1)
#define DESC_SWCORE_OWNED                        (1 << 0)

#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM16(addr)         (*(volatile unsigned short *) (addr))
#define WRITE_MEM8(addr, val)    (*(volatile unsigned char *) (addr)) = (val)
#define READ_MEM8(addr)          (*(volatile unsigned char *) (addr))


