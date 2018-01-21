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

#define	RTL865X_SWNIC_TXRING_MAX_RING	4

static unsigned int*  txPkthdrRing;
static unsigned int*  rxPkthdrRing;
uint32_t* rxMbufRing;
int txPos;

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

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

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */

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
char eth0_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0xe8};
struct ethernetif *ethernetif = netif->state;

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
	ptr = (unsigned int *)malloc(4 * RTL865X_SWNIC_TXRING_MAX_RING);
	ptr = (unsigned int)ptr | 0xa0000000;
	memset(ptr, 0, 4 * RTL865X_SWNIC_TXRING_MAX_RING);
	txPkthdrRing = ptr;

	/* Allocate Rx descriptors of rings */
	ptr = (unsigned int *)malloc(4 * RTL865X_SWNIC_TXRING_MAX_RING);
	ptr = (unsigned int)ptr | 0xa0000000;
	memset(ptr, 0, 4 * RTL865X_SWNIC_TXRING_MAX_RING);
	rxPkthdrRing = ptr;

	/* Allocate MBuf descriptors of rings */
	ptr = (uint32_t *)malloc(4 * 4);
	ptr = (unsigned int)ptr | 0xa0000000;
	memset(ptr, 0, 4 * 4);
	rxMbufRing = ptr;

	/* Allocate pkthdr */
	pPkthdrList = (struct pktHdr *) malloc(sizeof(struct pktHdr) * 8);
	pPkthdrList = (unsigned int)pPkthdrList | 0xa0000000;

	/* Allocate mbufs */
	pMbufList = (struct mBuf *) malloc(sizeof(struct mBuf) * 8);
	pMbufList = (unsigned int)pMbufList | 0xa0000000;

	/* Allocate clusters */
	pClusterList = (uint8_t *)malloc(size_of_cluster * 8);
	pClusterList = (unsigned int)pClusterList | 0xa0000000;

	/* setup tx ring */
	for (i = 0; i < RTL865X_SWNIC_TXRING_MAX_RING; ++i) {
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

	txPkthdrRing[RTL865X_SWNIC_TXRING_MAX_RING-1] |= DESC_WRAP;

	flush_cache();

	ptr = (unsigned int *)CPUTPDCR0;
	*ptr = txPkthdrRing;
	ptr = (unsigned int *)CPUTPDCR1;
	*ptr = 0;

	/* setup rx ring */
	for (i = 0; i < 4; i++)
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

	rxPkthdrRing[4 -1] |= DESC_WRAP;
	rxMbufRing[4 - 1] |= DESC_WRAP;

	ptr = (unsigned int *)CPURPDCR0;
	*ptr = rxPkthdrRing;
	ptr = (unsigned int *)CPURPDCR1;
	*ptr = 0;
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
dumpmem((int *)0xbb801300, 64);
dumpmem((int *)0xbb801800, 64);
dumpmem((int *)0xbb801b00, 64);
dumpmem((int *)0xBB804100, 64);
	char *str[32];
	sprintf(str, "len %d.", p->len);
	print(str);
*/

	pPkthdr->ph_mbuf->m_len = pPkthdr->ph_len;
	pPkthdr->ph_mbuf->m_extsize = pPkthdr->ph_len;

	pPkthdr->ph_vlanId = 8;
	pPkthdr->ph_portlist = ALL_PORT_MASK;
	txPkthdrRing[txPos] |= DESC_SWCORE_OWNED;

	ptr = (unsigned int *)CPUICR;
	*ptr |= TXFD;

	++txPos;
	if (txPos == 4)
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

struct pbuf *que;

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


	for (i = 0; i < 4; ++i) {
		if ((rxPkthdrRing[i] & DESC_OWNED_BIT) ==
		    DESC_RISC_OWNED ) {
			pPkthdr = (struct pktHdr *) (rxPkthdrRing[i] & 
					~(DESC_OWNED_BIT | DESC_WRAP));
			data = (int)pPkthdr->ph_mbuf->m_data;
			len = pPkthdr->ph_len;

			/* We allocate a pbuf chain of pbufs from the pool. */
			p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

			if (p != NULL) {
				pbuf_take(p, data, len);
//				que = p;
#if 1
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
			flush_cache();
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

	que = NULL;

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

