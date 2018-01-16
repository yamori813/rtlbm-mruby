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

void Ether_isr(void)
{
long *lptr;
long reg;

	lptr = (unsigned long *)CPUIISR;
	if( *lptr & TX_DONE_IE0 ) {
		put('T');
		reg = *lptr | TX_DONE_IE0;
	}
	if( *lptr & RX_DONE_IE0 ) {
		put('R');
		ethernetif_input(&netif);
		reg = *lptr | RX_DONE_IE0;
	}
	*lptr = reg;
}

struct irqaction irq_Ether = {Ether_isr, (void *)NULL};
char eth0_mac_httpd[6];
extern struct pbuf *que;
void net_poll()
{
	if (que != NULL) {
		if (netif.input(que, &netif) != ERR_OK) {
			pbuf_free(que);
			que = NULL;
		}

	}

	sys_check_timeouts();
}

void net_init()
{
long *lptr;

	IP4_ADDR(&ipaddr, 10,10,10,2);
	IP4_ADDR(&netmask, 255,255,255,0);
	IP4_ADDR(&gw, 10,10,10,3);

	lwip_init();
	netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init,
	    ethernet_input);
	netif_set_default(&netif);
	netif_set_up(&netif);

	lptr = (unsigned long *)IRR1;
	*lptr |= (3 << 0);
	request_IRQ(8, &irq_Ether, NULL);
}
