#include <sys/cdefs.h>

#include "lwip/init.h"

#include "lwip/debug.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"

#include "lwip/stats.h"

#include "lwip/ip.h"
#include "lwip/ip4_frag.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

#include "asicregs.h"
#include "intr.h"
#include "rtlregs.h"
#include "system.h"

struct netif netif;

static ip4_addr_t ipaddr, netmask, gw;

err_t ethernetif_init(struct netif *netif);
err_t ethernet_input(struct pbuf *p, struct netif *netif);

char eth0_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0xe8};

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
	if(tmpbuf[0] == 'h')
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

static struct udp_pcb *udpecho_raw_pcb;

char udpbuff[1024];

static void
udpecho_raw_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                 const ip_addr_t *addr, u16_t port)
{
  LWIP_UNUSED_ARG(arg);
  if (p != NULL) {
    /* send received packet back to sender */
//    udp_sendto(upcb, p, addr, port);
      pbuf_copy_partial(p, udpbuff, p->tot_len, 0);
      udpbuff[p->tot_len] = '\0';
    /* free the pbuf */
    pbuf_free(p);
  }
}

void
udpecho_raw_init(void)
{
  udpecho_raw_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  if (udpecho_raw_pcb != NULL) {
    err_t err;

    err = udp_bind(udpecho_raw_pcb, IP_ANY_TYPE, 7000);
    if (err == ERR_OK) {
      udp_recv(udpecho_raw_pcb, udpecho_raw_recv, NULL);
    } else {
      /* abort? output diagnostic? */
    }
  } else {
    /* abort? output diagnostic? */
  }
}

int netstat = 0;

void net_poll()
{

	if (netstat == 1)
		sys_check_timeouts();
}

void net_init()
{
long *lptr;

	gethwmac(eth0_mac);

	swCore_init();

	IP4_ADDR(&ipaddr, 10,10,10,2);
	IP4_ADDR(&netmask, 255,255,255,0);
	IP4_ADDR(&gw, 10,10,10,3);

	lwip_init();
	netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init,
	    ethernet_input);

	vlan_init();

	lptr = (unsigned long *)IRR1;
	*lptr |= (3 << 0);
	request_IRQ(8, &irq_Ether, NULL);

	netif_set_default(&netif);
	netif_set_up(&netif);   /* send broadcast arp packet */

	udpbuff[0] = '\0';
	udpecho_raw_init();

	netstat = 1;

}
