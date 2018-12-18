/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
*
* Abstract: Switch core polling mode NIC driver source code.
*
*
* ---------------------------------------------------------------
*/

#define     BUF_FREE            0x00   /* Buffer is Free  */
#define     BUF_USED            0x80   /* Buffer is occupied */
#define     BUF_ASICHOLD        0x80   /* Buffer is hold by ASIC */
#define     BUF_DRIVERHOLD      0xc0   /* Buffer is hold by driver */

#define DESC_RISC_OWNED		(0 << 0)

//--------------------------------------------------------------------------
/* mbuf header associated with each cluster 
*/
struct mBuf
{
struct mBuf	*m_next;
struct pktHdr	*m_pkthdr;	/* Points to the pkthdr structure */
uint16_t		m_len;		/* data bytes used in this cluster */
#ifdef CONFIG_RTL865XC
uint16_t		m_flags;	/* mbuf flags; see below */
#else
int8_t		m_flags;	/* mbuf flags; see below */
#endif
#define MBUF_FREE	BUF_FREE	/* Free. Not occupied. should be on free list   */
#define MBUF_USED	BUF_USED	/* Buffer is occupied */
#define MBUF_EXT	0x10	/* has associated with an external cluster, this is always set. */
#define MBUF_PKTHDR	0x08	/* is the 1st mbuf of this packet */
#define MBUF_EOR	0x04	/* is the last mbuf of this packet. Set only by ASIC*/
uint8_t	*m_data;                  /*  location of data in the cluster */
uint8_t	*m_extbuf;                /* start of buffer*/
uint16_t	m_extsize;                /* sizeof the cluster */
int8_t	m_reserved[2];            /* padding */
};


//--------------------------------------------------------------------------
/* pkthdr records packet specific information. Each pkthdr is exactly 32 bytes.
 first 20 bytes are for ASIC, the rest 12 bytes are for driver and software usa
ge.
*/
struct pktHdr
{
    union
    {
        struct pktHdr *pkthdr_next;     /*  next pkthdr in free list */
        struct mBuf *mbuf_first;        /*  1st mbuf of this pkt */
    }PKTHDRNXT;
#define ph_nextfree         PKTHDRNXT.pkthdr_next
#define ph_mbuf             PKTHDRNXT.mbuf_first
    uint16_t    ph_len;                   /*   total packet length */
    uint16_t    ph_reserved1: 1;           /* reserved */
    uint16_t    ph_queueId: 3;            /* bit 2~0: Queue ID */
    uint16_t    ph_extPortList: 4;        /* dest extension port list. must be 0 
for TX */
    uint16_t    ph_reserved2: 3;          /* reserved */
    uint16_t    ph_hwFwd: 1;              /* hwFwd - copy from HSA bit 200 */
    uint16_t    ph_isOriginal: 1;         /* isOriginal - DP included cpu port or
 more than one ext port */
    uint16_t    ph_l2Trans: 1;            /* l2Trans - copy from HSA bit 129 */
    uint16_t    ph_srcExtPortNum: 2;      /* Both in RX & TX. Source extension port number. */

    uint16_t    ph_type: 3;
#define PKTHDR_ETHERNET      0
#define PKTHDR_IP            2
#define PKTHDR_ICMP          3
#define PKTHDR_IGMP          4
#define PKTHDR_TCP           5
#define PKTHDR_UDP           6
    uint16_t    ph_vlanTagged: 1;         /* the tag status after ALE */
    uint16_t    ph_LLCTagged: 1;          /* the tag status after ALE */
    uint16_t    ph_pppeTagged: 1;         /* the tag status after ALE */
    uint16_t    ph_pppoeIdx: 3;
    uint16_t    ph_linkID: 7;             /* for WLAN WDS multiple tunnel */
    uint16_t    ph_reason;                /* indicates wht the packet is received by CPU */

    uint16_t    ph_flags;                 /*  NEW:Packet header status bits */
#define PKTHDR_FREE          (BUF_FREE << 8)        /* Free. Not occupied. shou
ld be on free list   */
#define PKTHDR_USED          (BUF_USED << 8)
#define PKTHDR_ASICHOLD      (BUF_ASICHOLD<<8)      /* Hold by ASIC */
#define PKTHDR_DRIVERHOLD    (BUF_DRIVERHOLD<<8)    /* Hold by driver */
#define PKTHDR_CPU_OWNED     0x4000
#define PKT_INCOMING         0x1000     /* Incoming: packet is incoming */
#define PKT_OUTGOING         0x0800     /*  Outgoing: packet is outgoing */
#define PKT_BCAST            0x0100     /*send/received as link-level broadcast
  */
#define PKT_MCAST            0x0080     /*send/received as link-level multicast
   */
#define PKTHDR_PPPOE_AUTOADD    0x0004  /* PPPoE header auto-add */
#define CSUM_TCPUDP_OK       0x0001     /*Incoming:TCP or UDP cksum checked */
#define CSUM_IP_OK           0x0002     /* Incoming: IP header cksum has checke
d */
#define CSUM_TCPUDP          0x0001     /*Outgoing:TCP or UDP cksum offload to 
ASIC*/
#define CSUM_IP              0x0002     /* Outgoing: IP header cksum offload to
 ASIC*/

   uint8_t      ph_orgtos;                /* RX: original TOS of IP header's valu
e before remarking, TX: undefined */
   uint8_t      ph_portlist;              /* RX: source port number, TX: destination portmask */

   uint16_t     ph_vlanId_resv: 1;
   uint16_t     ph_txPriority: 3;
   uint16_t     ph_vlanId: 12;
   uint16_t     ph_flags2;
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)
   uint8_t      ph_ptpResv:1;
   uint8_t      ph_ptpMsgType:4;  /* message type */
   uint8_t      ph_ptpVer:2;      /* PTP version, 0: 1588v1; 1: 1588v2 or 802.1as; others: reserved */
   uint8_t      ph_ptpPkt:1;      /* 1: PTP */
   int8_t       ph_reserved[3];            /* padding */
#endif

};
