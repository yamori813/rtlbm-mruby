/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Hiroki Mori
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#include "mbuf.h"
#include "asicregs.h"
#include "rtlregs.h"
#include "system.h"
#include "intr.h"

static unsigned int*  txPkthdrRing;
static unsigned int*  rxPkthdrRing;
uint32_t* rxMbufRing;
int txPos;

#define TX_RING0	4
#define TX_RING1	2
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)
#define TX_RING2	2
#define TX_RING3	2
#define TX_RING		(TX_RING0 + TX_RING1 + TX_RING2 + TX_RING3)
#else
#define TX_RING		(TX_RING0 + TX_RING1)
#endif
#define RX_RING		8

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

struct netif netif;

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */

struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
//static void  ethernetif_input(struct netif *netif);


#ifdef RTLBM_MRUBY_DEBUG
dumppkt(unsigned char *dat, int len)
{
int i;
char buf[8];

	for (i = 0; i < len; ++i) {
		sprintf(buf, "%02x ", *dat++);
		print(buf);
	}
	print("\r\n");
}
#endif

#ifndef RTLMACADDR
#define	RTLMACADDR	0x56,0xaa,0xa5,0x5a,0x7d,0xe8
#endif

char eth0_mac[6]={RTLMACADDR};

void gethwmac(unsigned char *mac)
{
	unsigned char tmpbuf[6];
	unsigned short len;
	unsigned char *buf;
	unsigned char sum=0;
	int i;
	
	if (flashread(tmpbuf, HW_SETTING_OFFSET,6)==0 ) {
		return;
	}
	if(tmpbuf[0] == 'H' && tmpbuf[1] == '6' && tmpbuf[2] == '0' &&
	    tmpbuf[3] == '1')
	{
		memcpy(&len, &tmpbuf[4], 2);
		if(len > 0x2000)
			return;
		if(NULL==(buf=(unsigned char *)malloc(len)))
			return;
		flashread(buf,HW_SETTING_OFFSET+6,len);
		if(len != 0 && len <= 0x2000) {					
			for (i=0;i<len;i++) 
				sum += buf[i];
		}
		else
			sum=1;
		if(0 == sum)
		{			
			memcpy(mac,buf+HW_NIC0_MAC_OFFSET,6);
			if(memcmp(mac,"\x0\x0\x0\x0\x0\x0", 6) && !(mac[0] & 0x1))
			{
				/*normal mac*/
			}
			else
			{
				memset(mac,0x0,6);
			}
		}
		if(buf)
			free(buf);
	}
	return;
}

void Ether_isr(void)
{
long *lptr;
long reg;

	lptr = (unsigned long *)CPUIISR;
	if( *lptr & TX_DONE_IE0 ) {
		reg = *lptr | TX_DONE_IE0;
	}
	if( *lptr & TX_DONE_IE1 ) {
		reg = *lptr | TX_DONE_IE1;
	}
	if( *lptr & RX_DONE_IE0 ) {
		ethernetif_input(&netif);
		reg = *lptr | RX_DONE_IE0;
	}
	*lptr = reg;
}

struct irqaction irq_Ether = {Ether_isr, (void *)NULL};
/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */

extern char eth0_mac[6];

static void
low_level_init(struct netif *netif)
{
int i;
unsigned int *ptr;
struct pktHdr *pPkthdrList;
struct mBuf *pMbufList;
struct pktHdr * pPkthdr;
struct mBuf * pMbuf;
uint32_t  size_of_cluster;
uint8_t * pClusterList;
struct ethernetif *ethernetif = netif->state;
long *lptr;

	gethwmac(eth0_mac);

	swCore_init();

	vlan_init();

#if RTL8196E
	lptr = (unsigned long *)IRR1;
	*lptr |= (3 << 28);
	request_IRQ(15, &irq_Ether, NULL);
#else
	lptr = (unsigned long *)IRR1;
	*lptr |= (3 << 0);
	request_IRQ(8, &irq_Ether, NULL);
#endif

	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set MAC hardware address */
	netif->hwaddr[0] = eth0_mac[0];
	netif->hwaddr[1] = eth0_mac[1];
	netif->hwaddr[2] = eth0_mac[2];
	netif->hwaddr[3] = eth0_mac[3];
	netif->hwaddr[4] = eth0_mac[4];
	netif->hwaddr[5] = eth0_mac[5];

	/* maximum transfer unit */
	netif->mtu = 1500;

	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
	    NETIF_FLAG_LINK_UP;

	size_of_cluster = 2048;

	/* only 1 ring and 4 dicriptors implimentation at tx and rx*/

	/* Allocate Tx descriptors of rings */
	ptr = (unsigned int *)malloc(4 * TX_RING);
	ptr = (unsigned int)ptr | 0xa0000000;
	memset(ptr, 0, 4 * TX_RING);
	txPkthdrRing = ptr;

	/* Allocate Rx descriptors of rings */
	ptr = (unsigned int *)malloc(4 * RX_RING);
	ptr = (unsigned int)ptr | 0xa0000000;
	memset(ptr, 0, 4 * RX_RING);
	rxPkthdrRing = ptr;

	/* Allocate MBuf descriptors of rings */
	ptr = (uint32_t *)malloc(4 * RX_RING);
	ptr = (unsigned int)ptr | 0xa0000000;
	memset(ptr, 0, 4 * 4);
	rxMbufRing = ptr;

	/* Allocate pkthdr */
	pPkthdrList = (struct pktHdr *) malloc(sizeof(struct pktHdr) * 
	    (TX_RING + RX_RING));
	pPkthdrList = (unsigned int)pPkthdrList | 0xa0000000;

	/* Allocate mbufs */
	pMbufList = (struct mBuf *) malloc(sizeof(struct mBuf) *
	    (TX_RING + RX_RING));
	pMbufList = (unsigned int)pMbufList | 0xa0000000;

	/* Allocate clusters */
	pClusterList = (uint8_t *)malloc(size_of_cluster *
	    (TX_RING + RX_RING));
	pClusterList = (unsigned int)pClusterList | 0xa0000000;

	/* setup tx ring */
	for (i = 0; i < TX_RING; ++i) {
		pPkthdr = pPkthdrList++;
		pMbuf = pMbufList++;
		bzero((void *) pPkthdr, sizeof(struct pktHdr));
		bzero((void *) pMbuf, sizeof(struct mBuf));

		pPkthdr->ph_mbuf = pMbuf;
		pPkthdr->ph_len = 0;
		pPkthdr->ph_flags = PKTHDR_USED | PKT_OUTGOING;
		pPkthdr->ph_type = PKTHDR_ETHERNET;
		pPkthdr->ph_portlist = 0;

		pMbuf->m_next = NULL;
		pMbuf->m_pkthdr = pPkthdr;
		pMbuf->m_flags = MBUF_USED | MBUF_EXT | MBUF_PKTHDR | MBUF_EOR;
//		pMbuf->m_data = NULL;
//		pMbuf->m_extbuf = NULL;
		pMbuf->m_extsize = 0;
		pMbuf->m_data = pMbuf->m_extbuf = pClusterList;
		pClusterList += size_of_cluster;

		txPkthdrRing[i] = (int32_t) pPkthdr | DESC_RISC_OWNED;
	}

	txPkthdrRing[TX_RING0 - 1] |= DESC_WRAP;
	txPkthdrRing[TX_RING - 1] |= DESC_WRAP;

	ptr = (unsigned int *)CPUTPDCR0;
	*ptr = &txPkthdrRing[0];
	ptr = (unsigned int *)CPUTPDCR1;
	*ptr = &txPkthdrRing[TX_RING0];
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)
	txPkthdrRing[TX_RING0 + TX_RING1 - 1] |= DESC_WRAP;
	txPkthdrRing[TX_RING0 + TX_RING1 + TX_RING2 - 1] |= DESC_WRAP;
	ptr = (unsigned int *)CPUTPDCR2;
	*ptr = &txPkthdrRing[TX_RING0 + TX_RING1];
	ptr = (unsigned int *)CPUTPDCR3;
	*ptr = &txPkthdrRing[TX_RING0 + TX_RING1 + TX_RING2];
#endif

	/* setup rx ring */
	for (i = 0; i < RX_RING; i++)
	{
		/* Dequeue pkthdr and mbuf */
		pPkthdr = pPkthdrList++;
		pMbuf = pMbufList++;

		bzero((void *) pPkthdr, sizeof(struct pktHdr));
		bzero((void *) pMbuf, sizeof(struct mBuf));

		/* Setup pkthdr and mbuf */
		pPkthdr->ph_mbuf = pMbuf;
		pPkthdr->ph_len = 0;
		pPkthdr->ph_flags = PKTHDR_USED | PKT_INCOMING;
		pPkthdr->ph_type = PKTHDR_ETHERNET;
		pPkthdr->ph_portlist = 0;
		pMbuf->m_next = NULL;
		pMbuf->m_pkthdr = pPkthdr;
		pMbuf->m_len = 0;
		pMbuf->m_flags = MBUF_USED | MBUF_EXT | MBUF_PKTHDR | MBUF_EOR;
		pMbuf->m_data = NULL;
		pMbuf->m_extsize = size_of_cluster;
		/*offset 2 bytes for 4 bytes align of ip packet*/
		pMbuf->m_data = pMbuf->m_extbuf = (pClusterList+2);
		pClusterList += size_of_cluster;
                        
		/* Setup descriptors */
		rxPkthdrRing[i] = (int32_t) pPkthdr | DESC_SWCORE_OWNED;
		rxMbufRing[i] = (int32_t) pMbuf | DESC_SWCORE_OWNED;
	}

	rxPkthdrRing[RX_RING / 2 -1] |= DESC_WRAP;
	rxMbufRing[RX_RING / 2 - 1] |= DESC_WRAP;
	rxPkthdrRing[RX_RING -1] |= DESC_WRAP;
	rxMbufRing[RX_RING - 1] |= DESC_WRAP;

	ptr = (unsigned int *)CPURPDCR0;
	*ptr = &rxPkthdrRing[0];
	ptr = (unsigned int *)CPURPDCR1;
	*ptr = &rxPkthdrRing[RX_RING / 2];
	ptr = (unsigned int *)CPURPDCR2;
	*ptr = 0;
	ptr = (unsigned int *)CPURPDCR3;
	*ptr = 0;
	ptr = (unsigned int *)CPURPDCR4;
	*ptr = 0;
	ptr = (unsigned int *)CPURPDCR5;
	*ptr = 0;

	ptr = (unsigned int *)CPURMDCR0;
	*ptr = rxMbufRing;

	ptr = (unsigned int *)CPUICR;
	*ptr = TXCMD | RXCMD | BUSBURST_32WORDS | MBUF_2048BYTES;

	ptr = (unsigned int *)CPUIIMR;
	*ptr = TX_DONE_IE_ALL | RX_DONE_IE_ALL;

//	REG32(MDCIOCR)=0x96181441;      // enable Giga port 8211B LED

	txPos = 0;
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */


static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
unsigned int *ptr;
struct pktHdr * pPkthdr;
uint8_t* pktbuf;
struct ethernetif *ethernetif = netif->state;
int i;

#if 0
	for (i = 0; i < 4; ++i) {
		if(txPkthdrRing[i] | DESC_OWNED_BIT)
			put('x');
	}
#endif
	pPkthdr = (struct pktHdr *) ((int32_t) txPkthdrRing[txPos]
	    & ~(DESC_OWNED_BIT | DESC_WRAP));

	pktbuf = (unsigned int)pPkthdr->ph_mbuf->m_data;

	if (p->tot_len < 60) {
		pPkthdr->ph_len = 64;
		bzero((void *) pktbuf, 64);
	} else {
		pPkthdr->ph_len = p->tot_len + 4;
	}

	pbuf_copy_partial(p, pktbuf, p->tot_len, 0);

//dumppkt(pktbuf, len);
/*
dumpmem((int *)0xBB804100, 64);
for (i = 0;i < 5; ++i)
dumpmem(0xbb801100+i*0x80, 16);
for (i = 0;i < 5; ++i)
dumpmem(0xbb801800+i*0x80, 16);
	char *str[32];
	sprintf(str, "len %d.", p->len);
	print(str);
*/

	pPkthdr->ph_mbuf->m_len = pPkthdr->ph_len;
	pPkthdr->ph_mbuf->m_extsize = pPkthdr->ph_len;

	pPkthdr->ph_vlanId = 8;
	pPkthdr->ph_portlist = ALL_PORT_MASK;
	pPkthdr->ph_srcExtPortNum = 0;
	txPkthdrRing[txPos] |= DESC_SWCORE_OWNED;

	ptr = (unsigned int *)CPUICR;
	*ptr |= TXFD;

	++txPos;
	if (txPos == 6)
		txPos = 0;

	return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */

#define INQEULEN 16

struct pbuf *inque[INQEULEN];
int inquestart;
int inqueend;

extern struct netif netif;

eninque(struct pbuf *p)
{
	inque[inqueend] = p;
	++inqueend;
	if (inqueend == INQEULEN)
		inqueend = 0;
}

doque()
{
err_t err;

	if(inquestart != inqueend) {
		while (inquestart != inqueend) {
			err = netif.input(inque[inquestart], &netif);
			if (err != ERR_OK) {
				pbuf_free(inque[inquestart]);
			}
			++inquestart;
			if (inquestart == INQEULEN)
				inquestart = 0;
		}
	}
}

void
low_level_input(struct netif *netif)
{
struct ethernetif *ethernetif = netif->state;
struct pbuf *p;
u16_t len;
struct pktHdr * pPkthdr;
char        *data;
int i;

	/* Obtain the size of the packet and put it into the "len" variable. */
	len = 0;


	for (i = 0; i < RX_RING; ++i) {
		if ((rxPkthdrRing[i] & DESC_OWNED_BIT) ==
		    DESC_RISC_OWNED ) {
			pPkthdr = (struct pktHdr *) (rxPkthdrRing[i] & 
					~(DESC_OWNED_BIT | DESC_WRAP));
			data = (int)pPkthdr->ph_mbuf->m_data;
			len = pPkthdr->ph_len;

			/* We allocate a pbuf chain of pbufs from the pool. */
			p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
#if 0
char str[128];
sprintf(str, "inlen = %d vid= %d.", len, pPkthdr->ph_vlanId);print(str);
#endif
			if (p != NULL) {
				pbuf_take(p, data, len);
#if USE_INQUEUE
				eninque(p);
#else
				if (netif->input(p, netif) != ERR_OK) {
					pbuf_free(p);
					p = NULL;
				}
#endif
			} else {
				/* pbuf error */
			}

			/* Reset OWN bit */
			rxPkthdrRing[i] |= DESC_SWCORE_OWNED;
			rxMbufRing[i] |= DESC_SWCORE_OWNED;
		}
	}

	if ( REG32(CPUIISR) & PKTHDR_DESC_RUNOUT_IP_ALL ) {
		/* Enable and clear interrupt for continue reception */
		REG32(CPUIIMR) |= PKTHDR_DESC_RUNOUT_IE_ALL;
		REG32(CPUIISR) = PKTHDR_DESC_RUNOUT_IP_ALL;
	}

	return;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */

void
ethernetif_input(struct netif *netif)
{
struct ethernetif *ethernetif;

	ethernetif = netif->state;

	low_level_input(netif);
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */


err_t
ethernetif_init(struct netif *netif)
{
struct ethernetif *ethernetif;

	ethernetif = mem_malloc(sizeof(struct ethernetif));
	if (ethernetif == NULL) {
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
		return ERR_MEM;
	}

	inquestart = 0;
	inqueend = 0;

	netif->state = ethernetif;
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;

	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */

	netif->output = etharp_output;
	netif->linkoutput = low_level_output;

//	ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

	/* initialize the hardware */
	low_level_init(netif);

	return ERR_OK;
}

