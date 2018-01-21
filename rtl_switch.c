
#include <stdint.h>
#include <strings.h>

#include "asicregs.h"
#include "rtlregs.h"
#include "rtl_switch.h"

extern char eth0_mac[6];

static void _rtl8651_clearSpecifiedAsicTable(uint32 type, uint32 count) 
{
        struct { uint32 _content[8]; } entry;
        uint32 idx;
        
        bzero(&entry, sizeof(entry));
        for (idx=0; idx<count; idx++)// Write into hardware
                swTable_addEntry(type, idx, &entry);
}

static void enable_10M_power_saving(int phyid , int regnum,int data)
{   
        unsigned int uid,tmp;  
        rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );
        uid=tmp;
        uid =data;
        rtl8651_setAsicEthernetPHYReg( phyid, regnum, uid );
        rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );
        uid=tmp;
}

static void tableAccessForeword(uint32 tableType, uint32 eidx, void *entryContent_P)
{
//    ASSERT_CSP(entryContent_P);

    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );
    
    /* Write registers according to entry width of each table */
    REG32(TCR7) = *((uint32 *)entryContent_P + 7);
    REG32(TCR6) = *((uint32 *)entryContent_P + 6);
    REG32(TCR5) = *((uint32 *)entryContent_P + 5);
    REG32(TCR4) = *((uint32 *)entryContent_P + 4);
    REG32(TCR3) = *((uint32 *)entryContent_P + 3);
    REG32(TCR2) = *((uint32 *)entryContent_P + 2);
    REG32(TCR1) = *((uint32 *)entryContent_P + 1);
    REG32(TCR0) = *(uint32 *)entryContent_P;
    
    /* Fill address */
    REG32(SWTAA) = table_access_addr_base(tableType) + eidx * TABLE_ENTRY_DISTANCE;
}

static void swTable_readEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{
    uint32 *    entryAddr;

    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

//    ASSERT_CSP(entryContent_P);
    
    entryAddr = (uint32 *) (table_access_addr_base(tableType) + eidx * TABLE_ENTRY_DISTANCE);
    
    /* Wait for command ready */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );
    
    /* Read registers according to entry width of each table */
    *((uint32 *)entryContent_P + 7) = *(entryAddr + 7);
    *((uint32 *)entryContent_P + 6) = *(entryAddr + 6);
    *((uint32 *)entryContent_P + 5) = *(entryAddr + 5);
    *((uint32 *)entryContent_P + 4) = *(entryAddr + 4);
    *((uint32 *)entryContent_P + 3) = *(entryAddr + 3);
    *((uint32 *)entryContent_P + 2) = *(entryAddr + 2);
    *((uint32 *)entryContent_P + 1) = *(entryAddr + 1);
    *((uint32 *)entryContent_P + 0) = *(entryAddr + 0);

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;
}

int32 swTable_addEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{
    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

    tableAccessForeword(tableType, eidx, entryContent_P);
        
    /* Activate add command */
    REG32(SWTACR) = ACTION_START | CMD_ADD;
    
    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;

    /* Check status */
    if ( (REG32(SWTASR) & TABSTS_MASK) != TABSTS_SUCCESS )
        return -1;
    else
        return 0;
}

void dumpmem(int *mem, int size)
{
int i, j, k;
char str[32];

	for (k = 0; k * 16 < size; ++k) {
		sprintf(str, "%08x:       ", mem);
		print(str);
		for(i = 0; i < 4; ++i) {
			if (i == 3)
				sprintf(str, "%08x\r\n", *mem++);
			else
				sprintf(str, "%08x        ", *mem++);
			print(str);
		}
	}
}

void dumptable(int *table)
{
int i, j;
char str[32];

	for(i = 0; i < 8; ++i) {
		sprintf(str, "%08x ", *table);
		print(str);
		++table;
	}
	put('\r');
	put('\n');
}

void dumpphy()
{
int i, j, k;
int data;
char str[32];

	for(i = 0; i < 32; ++i) {
		sprintf(str, "%02d: ", i);
		print(str);
		for(k = 0; k < 8; ++k) {
			rtl8651_getAsicEthernetPHYReg(k, i, &data);
			sprintf(str, "%04x ", data);
			print(str);
		}
		put('\r');
		put('\n');
	}
}

static void setup_vlan(int vid)
{
vlan_table_t    entryContent;

	bzero( (void *) &entryContent, sizeof(entryContent) );
	entryContent.memberPort = ALL_PORT_MASK;
	entryContent.egressUntag = ALL_PORT_MASK;
	entryContent.fid = 0;
	if (swTable_addEntry(TYPE_VLAN_TABLE, vid, &entryContent) != 0)
		print("vlan swTable_addEntry error");
}

static void setup_netif(ether_addr_t *mac, int vid, int mtu)
{
netif_table_t	entryContent;

	bzero( (void *) &entryContent, sizeof(entryContent) );
	entryContent.vid = vid;
	entryContent.valid = 1;

	entryContent.mac47_19 = ((mac->octet[0] << 21) | (mac->octet[1] << 13) |
	    (mac->octet[2] << 5) | (mac->octet[3] >> 3)) & 0xFFFFFFF;
	entryContent.mac18_0 = ((mac->octet[3] << 16) | (mac->octet[4] << 8 ) |
	    mac->octet[5]) & 0x7FFFF;

	entryContent.inACLStartH = (0 >> 2) & 0x1f;
	entryContent.inACLStartL = 0 & 0x3;
	entryContent.inACLEnd = 0;
	entryContent.outACLStart = 0;
	entryContent.outACLEnd = 0;
	entryContent.enHWRoute = 0;

	entryContent.macMask = 8 - (1 & 0x7);

	entryContent.mtuH = mtu >> 3;
	entryContent.mtuL = mtu & 0x7;
	if (swTable_addEntry(TYPE_NETINTERFACE_TABLE, 0, &entryContent) != 0)
		print("netif swTable_addEntry error");
}

vlan_init()
{
int i;

	REG32(PIN_MUX_SEL)=REG32(PIN_MUX_SEL)&(0xFFFFFFFF-0x00300000);

	for (i = 0;i < 5; ++i)
		enable_10M_power_saving(i, 0x18, 0x0310);

        setup_netif((ether_addr_t*)(&eth0_mac), 8, 1500);

        setup_vlan(8);
#if 0
	netif_table_t	netif;
	swTable_readEntry(TYPE_NETINTERFACE_TABLE, 0, &netif);
	dumptable(&netif);

	vlan_table_t    vlan;
	swTable_readEntry(TYPE_VLAN_TABLE, 8, &vlan);
	dumptable(&vlan);
	dumpphy();
	dumpmem((int *)0xBB804100, 64);
#endif
	dumpphy();
}

unsigned int read_gpio_hw_setting()
{
#ifndef RTL8196C
	unsigned int tmp;
	int b2;

	/* set (GP2)=(F3)= gpio */
	REG32(PEFGHCNR) = REG32(PEFGHCNR) & (~(0x8<<8));
	/* changet o GPIO mode */
	REG32(PEFGHPTYPE) = REG32(PEFGHPTYPE) & (~(0x8<<8));
	/* 0 input, 1 output, set inpur */
	REG32(PEFGHDIR) = REG32(PEFGHDIR) & (~(0x8<<8));
	tmp = REG32(PEFGHDAT);
	b2 = (tmp&(0x08<<8))>>11;
	tmp = (b2<<1)&0x2;      
	return tmp;     
#else
	return 0;
#endif
}

