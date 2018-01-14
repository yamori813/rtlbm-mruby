
#include <stdint.h>
#include <strings.h>

#include "asicregs.h"
#include "rtlregs.h"

typedef uint32_t uint32;
typedef int32_t int32;
typedef uint16_t uint16;
typedef uint8_t uint8;

#define GIGA_PHY_ID     0x16

#define               RTL8651_ASICTABLE_ENTRY_LENGTH (8 * sizeof(uint32))
#define         RTL8651_ASICTABLE_BASE_OF_ALL_TABLES            0xBB000000

#define         rtl8651_asicTableAccessAddrBase(type) (RTL8651_ASICTABLE_BASE_OF_ALL_TABLES + ((type)<<16) )


extern char eth0_mac[6];

typedef struct {
    uint16      mac47_32;
    uint16      mac31_16;
    uint16      mac15_0;
    uint16              align;
} macaddr_t;

typedef struct ether_addr_s {
        uint8 octet[6];
} ether_addr_t;


typedef struct {
         /* word 0 */
        uint32  reserved1:12;
        uint32  fid:2;
        uint32     extEgressUntag  : 3;
        uint32     egressUntag : 6;
        uint32     extMemberPort   : 3;
        uint32     memberPort  : 6;

    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} vlan_table_t;

typedef struct {
    /* word 0 */
    uint32          mac18_0:19;
    uint32          vid          : 12;
    uint32          valid       : 1;    
    /* word 1 */
    uint32         inACLStartL:2;       
    uint32         enHWRoute : 1;       
    uint32         mac47_19:29;

    /* word 2 */
    uint32         mtuL       : 3;
    uint32         macMask :3;  
    uint32         outACLEnd : 7;       
    uint32         outACLStart : 7;     
    uint32         inACLEnd : 7;        
    uint32         inACLStartH: 5;      
    /* word 3 */
    uint32          reserv10   : 20;
    uint32          mtuH       : 12;

    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} netif_table_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint16          mac39_24;
    uint16          mac23_8;

    /* word 1 */
    uint32          reserv0: 6;
    uint32          auth: 1;
    uint32          fid:2;
    uint32          nxtHostFlag : 1;
    uint32          srcBlock    : 1;
    uint32          agingTime   : 2;
    uint32          isStatic    : 1;
    uint32          toCPU       : 1;
    uint32          extMemberPort   : 3;
    uint32          memberPort : 6;
    uint32          mac47_40    : 8;

#else /*LITTLE_ENDIAN*/
    /* word 0 */
    uint16          mac23_8;
    uint16          mac39_24;
                
    /* word 1 */
    uint32          mac47_40    : 8;
    uint32          memberPort : 6;
    uint32          extMemberPort   : 3;
    uint32          toCPU       : 1;
    uint32          isStatic    : 1;
    uint32          agingTime   : 2;
    uint32          srcBlock    : 1;
    uint32          nxtHostFlag : 1;
    uint32          fid:2;
    uint32          auth:1;     
    uint32          reserv0:6;  

#endif /*LITTLE_ENDIAN*/
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_l2Table_t;


enum {
    TYPE_L2_SWITCH_TABLE = 0,
    TYPE_ARP_TABLE,
    TYPE_L3_ROUTING_TABLE,
    TYPE_MULTICAST_TABLE,
    TYPE_NETINTERFACE_TABLE,
    TYPE_EXT_INT_IP_TABLE,
    TYPE_VLAN_TABLE,
    TYPE_VLAN1_TABLE,
    TYPE_SERVER_PORT_TABLE,
    TYPE_L4_TCP_UDP_TABLE,
    TYPE_L4_ICMP_TABLE,
    TYPE_PPPOE_TABLE,
    TYPE_ACL_RULE_TABLE,
    TYPE_NEXT_HOP_TABLE,
    TYPE_RATE_LIMIT_TABLE,
    TYPE_ALG_TABLE,
};

static uint8 fidHashTable[]={0x00,0x0f,0xf0,0xff};

static void _rtl8651_asicTableAccessForward(uint32 tableType, uint32 eidx, void *entryContent_P) {
  //      ASSERT_CSP(entryContent_P);


        while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done

#ifdef RTL865X_FAST_ASIC_ACCESS

        {
                register uint32 index;

                for( index = 0; index < _rtl8651_asicTableSize[tableType]; index++ )
                {
                        WRITE_MEM32(TCR0+(index<<2), *((uint32 *)entryContent_P + index));
                }

        }
#else
        WRITE_MEM32(TCR0, *((uint32 *)entryContent_P + 0));
        WRITE_MEM32(TCR1, *((uint32 *)entryContent_P + 1));
        WRITE_MEM32(TCR2, *((uint32 *)entryContent_P + 2));
        WRITE_MEM32(TCR3, *((uint32 *)entryContent_P + 3));
        WRITE_MEM32(TCR4, *((uint32 *)entryContent_P + 4));
        WRITE_MEM32(TCR5, *((uint32 *)entryContent_P + 5));
        WRITE_MEM32(TCR6, *((uint32 *)entryContent_P + 6));
        WRITE_MEM32(TCR7, *((uint32 *)entryContent_P + 7));
#endif  
        WRITE_MEM32(SWTAA, ((uint32) rtl8651_asicTableAccessAddrBase(tableType) + eidx * RTL8651_ASICTABLE_ENTRY_LENGTH));//Fill address
}

static int32 _rtl8651_forceAddAsicEntry(uint32 tableType, uint32 eidx, void *entryContent_P) {

        #ifdef RTL865XC_ASIC_WRITE_PROTECTION
        if (RTL865X_TLU_BUG_FIXED)      /* No need to stop HW table lookup process */
        {       /* No need to stop HW table lookup process */
                WRITE_MEM32(SWTCR0,EN_STOP_TLU|READ_MEM32(SWTCR0));
                while ( (READ_MEM32(SWTCR0) & STOP_TLU_READY)==0);
        }
        #endif

        _rtl8651_asicTableAccessForward(tableType, eidx, entryContent_P);

        WRITE_MEM32(SWTACR, ACTION_START | CMD_FORCE);//Activate add command
        while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done

        #ifdef RTL865XC_ASIC_WRITE_PROTECTION
        if (RTL865X_TLU_BUG_FIXED)      /* No need to stop HW table lookup process */
        {
                WRITE_MEM32(SWTCR0,~EN_STOP_TLU&READ_MEM32(SWTCR0));
        }
        #endif

        return 1;
}

uint32 rtl8651_filterDbIndex(ether_addr_t * macAddr,uint16 fid) {
    return ( macAddr->octet[0] ^ macAddr->octet[1] ^
                    macAddr->octet[2] ^ macAddr->octet[3] ^
                    macAddr->octet[4] ^ macAddr->octet[5] ^fidHashTable[fid]) & 0xFF;
}

static int32 rtl8651_setAsicL2Table(ether_addr_t        *mac, uint32 column)
{
        rtl865xc_tblAsic_l2Table_t entry;
        uint32  row;

        row = rtl8651_filterDbIndex(mac, 0);
        if((row >= RTL8651_L2TBL_ROW) || (column >= RTL8651_L2TBL_COLUMN))
                return 0;
        if(mac->octet[5] != ((row^(fidHashTable[0])^ mac->octet[0] ^ mac->octet[1] ^ mac->octet[2] ^ mac->octet[3] ^ mac->octet[4] ) & 0xff))
                return 0;

        memset(&entry, 0,sizeof(entry));
        entry.mac47_40 = mac->octet[0];
        entry.mac39_24 = (mac->octet[1] << 8) | mac->octet[2];
        entry.mac23_8 = (mac->octet[3] << 8) | mac->octet[4];

//      entry.extMemberPort = 0;   
        entry.memberPort = 7;
        entry.toCPU = 1;
        entry.isStatic = 1;
//      entry.nxtHostFlag = 1;

        /* RTL865xC: modification of age from ( 2 -> 3 -> 1 -> 0 ) to ( 3 -> 2 -> 1 -> 0 ). modification of granularity 100 sec to 150 sec
. */
        entry.agingTime = 0x03;
        
//      entry.srcBlock = 0;
        entry.fid=0;
        entry.auth=1;

        return _rtl8651_forceAddAsicEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);
}



void rtl8651_setAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 wData)
{

	WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( phyId << PHYADD_OFFSET ) |
	    ( regId << REGADD_OFFSET ) | wData );

	while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );

}

void rtl8651_getAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 *rData)
{

	uint32 status;

	WRITE_MEM32( MDCIOCR, COMMAND_READ | ( phyId << PHYADD_OFFSET ) |
	    ( regId << REGADD_OFFSET ) );

	REG32(GIMR) = REG32(GIMR) | (0x1<<14);
//	delay_ms(10);
	int i;
	for (i = 0; i < 0x800000; ++i) ++i;

	do {
		status = READ_MEM32( MDCIOSR );
	} while ( ( status & STATUS ) != 0);

	status &= 0xffff;
	*rData = status;
}

void tableAccessForeword(uint32 tableType, uint32 eidx, void *entryContent_P)
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

int32 swTable_readEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
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

    return 0;
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

void FullAndSemiReset( void )
{

	/* FIXME: Currently workable for FPGA, may need further modification
	 for real chip */
	/* Perform full-reset for sw-core. */ 
	REG32(SIRR) |= FULL_RST;

//	tick_Delay10ms(50);
	int i; for (i = 1; i < 0x10000000; i += 2) --i;

	/* Enable TRXRDY */
	REG32(SIRR) |= TRXRDY;
}

void rtl8651_restartAsicEthernetPHYNway(uint32 port, uint32 phyid)
{
	uint32 statCtrlReg0;

	/* read current PHY reg 0 */
	rtl8651_getAsicEthernetPHYReg( phyid, 0, &statCtrlReg0 );

	/* enable 'restart Nway' bit */
	statCtrlReg0 |= RESTART_AUTONEGO;

	/* write PHY reg 0 */
	rtl8651_setAsicEthernetPHYReg( phyid, 0, statCtrlReg0 );
}

int32 rtl8651_setAsicFlowControlRegister(uint32 port, uint32 enable, uint32 phyid)
{
        uint32 statCtrlReg4;

        /* Read */
        rtl8651_getAsicEthernetPHYReg( phyid, 4, &statCtrlReg4 );

        if ( enable && ( statCtrlReg4 & CAPABLE_PAUSE ) == 0 )
        {
                statCtrlReg4 |= CAPABLE_PAUSE;
        }
        else if ( enable == 0 && ( statCtrlReg4 & CAPABLE_PAUSE ) )
        {      
                statCtrlReg4 &= ~CAPABLE_PAUSE;
        }     
        else
                return ; /* The configuration does not change. Do nothing. */

        rtl8651_setAsicEthernetPHYReg( phyid, 4, statCtrlReg4 );    
                                                                                                                                         
        /* restart N-way. */
        rtl8651_restartAsicEthernetPHYNway(port, phyid);

        return ;
}

void Set_GPHYWB(unsigned int phyid, unsigned int page, unsigned int reg,
    unsigned int mask, unsigned int val)
{
unsigned int data=0;
unsigned int wphyid=0;  //start
unsigned int wphyid_end=1;   //end

	if(phyid==999) {
		wphyid=0;
		wphyid_end=5;    //total phyid=0~4
	} else {
		wphyid=phyid;
		wphyid_end=phyid+1;
	}

	for(; wphyid<wphyid_end; wphyid++) {
		//change page 

		if(page>=31) {
			rtl8651_setAsicEthernetPHYReg( wphyid, 31, 7  );
			rtl8651_setAsicEthernetPHYReg( wphyid, 30, page  );
		} else {
			rtl8651_setAsicEthernetPHYReg( wphyid, 31, page  );
		}
		if(mask!=0) {
			rtl8651_getAsicEthernetPHYReg( wphyid, reg, &data);
			data=data&mask;
		}
		rtl8651_setAsicEthernetPHYReg( wphyid, reg, data|val  );

		rtl8651_setAsicEthernetPHYReg( wphyid, 31, 0  );
	}
}

void Setting_RTL8196C_PHY_REV_B()
{
int i;

	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) |= (EnForceMode);

	Set_GPHYWB(999, 1, 17, 0xffff-(7<<10), 0x7<<10);
	Set_GPHYWB(999, 4, 24, 0xff00, 0xf3);
	Set_GPHYWB(999, 4, 16, 0xffff-(1<<3), 1<<3);

	Set_GPHYWB(999, 1, 19, 0xffff-(7<<11), 0x2<<11);
	Set_GPHYWB(999, 1, 23, 0xffff-(7<<6)  , 0x4<<6);
	Set_GPHYWB(999, 1, 18, 0xffff-(7<<3), 0x6<<3);

	REG32(MACCR)= (REG32(MACCR)&0xfffffff0)|0x05;

	Set_GPHYWB(999, 0, 21, 0xffff-(0xff<<0), 0x32<<0);

	Set_GPHYWB(999, 0, 22, 0xffff-(7<<4), 0x5<<4);

	Set_GPHYWB(999, 0, 0, 0xffff-(1<<9), 0x1<<9);

	Set_GPHYWB(999, 1, 17, 0xffff-(3<<1), 0x3<<1);

	Set_GPHYWB(999, 1, 18, 0xffff-(0xffff<<0), 0x9004<<0);

	Set_GPHYWB(999, 0, 21, 0xffff-(1<<14), 1<<14);

	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) &= ~(EnForceMode);
}

void dumptable(int *table)
{
int i, j;
char str[32];

	for(i = 0; i < 8; ++i) {
		sprintf(str, "%08x ", *table);
		for (j = 0; str[j] != '\0'; ++j) put(str[j]);
		++table;
	}
	put('\r');
	put('\n');
}

void phydump()
{
int i, j, k;
int data;
char str[32];

	for(i = 0; i < 32; ++i) {
		for(k = 0; k < 8; ++k) {
			rtl8651_getAsicEthernetPHYReg(k, i, &data);
			sprintf(str, "%04x ", data);
			for (j = 0; str[j] != '\0'; ++j) put(str[j]);
		}
		put('\r');
		put('\n');
	}
}

void setup_vlan()
{
vlan_table_t    entryContent;

	bzero( (void *) &entryContent, sizeof(entryContent) );
	entryContent.memberPort = ALL_PORT_MASK;
	entryContent.egressUntag = ALL_PORT_MASK;
	entryContent.fid = 0;
	swTable_addEntry(TYPE_VLAN_TABLE, 8, &entryContent);
}

void setup_netif(char *eth0_mac)
{
netif_table_t	entryContent;
macaddr_t	gMac;

	memcpy((void *)&gMac, (void *)eth0_mac, 6);
	
	bzero( (void *) &entryContent, sizeof(entryContent) );
	entryContent.vid = 8;
	entryContent.valid = 1;

	entryContent.mac47_19 = ((gMac.mac47_32 << 13) |
	    (gMac.mac31_16 >> 3)) & 0xFFFFFFF;
	entryContent.mac18_0 = ((gMac.mac31_16 << 16) |
	    gMac.mac15_0) & 0x7FFFF;

	entryContent.inACLStartH = (0 >> 2) & 0x1f;
	entryContent.inACLStartL = 0 & 0x3;
	entryContent.inACLEnd = 0;
	entryContent.outACLStart = 0;
	entryContent.outACLEnd = 0;
	entryContent.enHWRoute = 0;

	entryContent.macMask = 8 - (1 & 0x7);

	entryContent.mtuH = 1500 >> 3;
	entryContent.mtuL = 1500 & 0x7;
	swTable_addEntry(TYPE_NETINTERFACE_TABLE, 0, &entryContent);
}

void switch_init(char *macaddr)
{
int port;

	FullAndSemiReset();

	phydump();

	Setting_RTL8196C_PHY_REV_B();

	REG32(PCRP0) &= (0xFFFFFFFF-(0x00400000|MacSwReset));
	REG32(PCRP1) &= (0xFFFFFFFF-(0x00400000|MacSwReset));
	REG32(PCRP2) &= (0xFFFFFFFF-(0x00400000|MacSwReset));
	REG32(PCRP3) &= (0xFFFFFFFF-(0x00400000|MacSwReset));
	REG32(PCRP4) &= (0xFFFFFFFF-(0x00400000|MacSwReset));

	REG32(PCRP0) = REG32(PCRP0) | (0 << ExtPHYID_OFFSET) | AcptMaxLen_16K |
	    EnablePHYIf | MacSwReset;
	REG32(PCRP1) = REG32(PCRP1) | (1 << ExtPHYID_OFFSET) | AcptMaxLen_16K |
	    EnablePHYIf | MacSwReset;
	REG32(PCRP2) = REG32(PCRP2) | (2 << ExtPHYID_OFFSET) | AcptMaxLen_16K |
	    EnablePHYIf | MacSwReset;
	REG32(PCRP3) = REG32(PCRP3) | (3 << ExtPHYID_OFFSET) | AcptMaxLen_16K |
	    EnablePHYIf | MacSwReset;
	REG32(PCRP4) = REG32(PCRP4) | (4 << ExtPHYID_OFFSET) | AcptMaxLen_16K |
	    EnablePHYIf | MacSwReset;

	/* Set PVID of all ports to 8 */
	REG32(PVCR0) = (0x8 << 16) | 0x8;
	REG32(PVCR1) = (0x8 << 16) | 0x8;
	REG32(PVCR2) = (0x8 << 16) | 0x8;
	REG32(PVCR3) = (0x8 << 16) | 0x8;
        
	/* Enable L2 lookup engine and spanning tree functionality */
	REG32(MSCR) = EN_L2;
	REG32(QNUMCR) = P0QNum_1 | P1QNum_1 | P2QNum_1 | P3QNum_1 | P4QNum_1;

	/* Start normal TX and RX */
	REG32(SIRR) |= TRXRDY;

	/* set pin mux for sw led. bit [17:0] =0 */
	REG32(0xb8000040) &= ~(0x3FFFF);

	for(port=0;port<MAX_PORT_NUMBER;port++) {
		/* Set Flow Control capability. */
		rtl8651_restartAsicEthernetPHYNway(port+1, port);       
	}

//	rtl8651_setAsicL2Table((ether_addr_t*)(&eth0_mac), 0);

	/* rx broadcast and unicast packet */
	REG32(FFCR) = EN_UNUNICAST_TOCPU | EN_UNMCAST_TOCPU;

	setup_netif((char *)macaddr);

	setup_vlan();

	netif_table_t    netif;
	swTable_readEntry(TYPE_NETINTERFACE_TABLE, 0, &netif);
	dumptable((int *)&netif);

	vlan_table_t  vlan;
	swTable_readEntry(TYPE_VLAN_TABLE, 8, &vlan);
	dumptable((int *)&vlan);

}
