#include <stdint.h>

typedef uint32_t uint32;
typedef int32_t int32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef int8_t int8;

#define	GIGA_PHY_ID	0x16
#define	FAILED 0
#define SUCCESS 1
#define TRUE 1
#define ASSERT_CSP {}

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
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)
	uint32  vid:12;
#else
        uint32  reserved1:12;
#endif
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
