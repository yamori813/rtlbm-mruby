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

#include "intr.h"

struct netif netif;

static ip4_addr_t ipaddr, netmask, gw;

err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif);

void Ether_isr(void)
{
}

struct irqaction irq_Ether = {Ether_isr, (void *)NULL};

void net_init()
{
	IP4_ADDR(&ipaddr, 192,168,0,2);
	IP4_ADDR(&netmask, 255,255,255,0);
	IP4_ADDR(&gw, 192,168,0,1);

	lwip_init();
	netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init,
	   ethernetif_input);

	request_IRQ(8, &irq_Ether, NULL);
}
