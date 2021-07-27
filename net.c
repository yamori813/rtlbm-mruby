/*
 * Copyright (c) 2018 Hiroki Mori. All rights reserved.
 */

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

#include "lwip/dns.h"
#include "lwip/dhcp.h"

#if defined(RTL8196) || defined(RTL8196E)
#include "system.h"
#endif

//#define	NETDEBUG

unsigned char debug_flags;

extern struct netif netif;

static ip_addr_t ipaddr, netmask, gw, dnsserver;

err_t ethernetif_init(struct netif *netif);
err_t ethernet_input(struct pbuf *p, struct netif *netif);

static struct udp_pcb *udpecho_raw_pcb;
static struct udp_pcb *udpsntp_raw_pcb;

char udpbuff[1024];
char tcpbuff[1024*8];
int tcplen;
int tcpoff;

static void
udpecho_raw_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                 const ip_addr_t *addr, u16_t port)
{
  LWIP_UNUSED_ARG(arg);
  if (p != NULL) {
      pbuf_copy_partial(p, udpbuff, p->tot_len, 0);
      udpbuff[p->tot_len] = '\0';
    /* free the pbuf */
    pbuf_free(p);
  }
}

static struct tcp_pcb *tcphttp_raw_pcb;
int tcpstat;

static err_t
http_recv(void *arg, struct tcp_pcb *pcb,
                  struct pbuf *pbuf, err_t err)
{
  if (pbuf != NULL) {
    if (tcplen == 0)
      tcpoff = 0;
    pbuf_copy_partial(pbuf, tcpbuff+tcpoff+tcplen, pbuf->tot_len, 0);
    tcplen += pbuf->tot_len;
    tcp_recved(pcb, pbuf->tot_len);
    pbuf_free(pbuf);
  } else {
    tcpstat = 2;
  }
  return ERR_OK;
}

static err_t
http_sent (void *arg, struct tcp_pcb *pcb, u16_t len)
{
  return ERR_OK;
}

static err_t
http_poll(void *arg, struct tcp_pcb *pcb)
{
  tcp_abort(pcb);
  tcpstat = 3;
  print("tcp_abort");
  return ERR_OK;
}

static void
http_err(void *arg, err_t err)
{
  tcpstat = 3;
}

static err_t
http_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
  tcplen = 0;
  tcpoff = 0;
  tcpstat = 1;
  return ERR_OK;
}

void
my_tcp_write(char *buf, int len)
{
  err_t err = ERR_OK;
  len = (u16_t)LWIP_MIN(len, tcp_sndbuf(tcphttp_raw_pcb));
  err = tcp_write(tcphttp_raw_pcb, buf, len, TCP_WRITE_FLAG_COPY);
  tcp_output(tcphttp_raw_pcb);
}

void
my_tcp_close()
{
  tcp_close(tcphttp_raw_pcb);
}

void
tcphttp_raw_init(int *addr, int disport, int type)
{
static ip_addr_t distaddr;

  if (type == 0) {
    ip_addr_set_ip4_u32(&distaddr, *addr);
    tcphttp_raw_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
  } else {
    distaddr.u_addr.ip6.addr[0] = lwip_htonl((addr[0] << 16) | addr[1]) ;
    distaddr.u_addr.ip6.addr[1] = lwip_htonl((addr[2] << 16) | addr[3]) ;
    distaddr.u_addr.ip6.addr[2] = lwip_htonl((addr[4] << 16) | addr[5]) ;
    distaddr.u_addr.ip6.addr[3] = lwip_htonl((addr[6] << 16) | addr[7]) ;
    distaddr.type = IPADDR_TYPE_V6;
    tcphttp_raw_pcb = tcp_new_ip_type(IPADDR_TYPE_V6);
  }

  if (tcphttp_raw_pcb != NULL) {
//    tcp_arg(tcphttp_raw_pcb, NULL);
    tcp_recv(tcphttp_raw_pcb, http_recv);
    tcp_sent(tcphttp_raw_pcb, http_sent);
    tcp_err(tcphttp_raw_pcb, http_err);
    tcp_poll(tcphttp_raw_pcb, http_poll, 60);
    tcp_connect(tcphttp_raw_pcb, &distaddr, disport, http_connected);
    tcpstat = 0;
  }
}

int netstat = 0;

void net_poll()
{

	if (netstat == 1) {
		sys_check_timeouts();
		doque();
	}
}

int dnsstat;
ip_addr_t resolvip;

static void
dns_found(const char* hostname, const ip_addr_t *ipaddr, void *arg)
{
  LWIP_UNUSED_ARG(arg);

  if (ipaddr != 0) {
    resolvip = *ipaddr;
    dnsstat = 1;
  } else {
    dnsstat = 2;
  }
}

void net_start(int myaddr, int mymask, int mygw, int mydns)
{
	ip_addr_set_ip4_u32(&ipaddr, myaddr);
	ip_addr_set_ip4_u32(&netmask, mymask);
	ip_addr_set_ip4_u32(&gw, mygw);
	ip_addr_set_ip4_u32(&dnsserver, mydns);

	net_init(0);
}

void net_startdhcp()
{
	IP_ADDR4(&ipaddr, 0,0,0,0);
	IP_ADDR4(&netmask, 0,0,0,0);
	IP_ADDR4(&gw, 0,0,0,0);

	net_init(1);
}

int getmyaddress()
{

	return netif_ip4_addr(&netif);
}

void net_init(int use_dhcp)
{
err_t err;
int i;

	lwip_init();

	if (use_dhcp)
		netif_add(&netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),
		   ip_2_ip4(&gw), NULL, ethernetif_init, ethernet_input);
	else
		netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init,
 		    ethernet_input);

	netif_create_ip6_linklocal_address(&netif, 1);
	netif.ip6_autoconfig_enabled = 1;

	netif_set_link_up(&netif);

	netif_set_default(&netif);
	netif_set_up(&netif);   /* send broadcast arp packet */

	netstat = 1;

	if (use_dhcp) {
		err = dhcp_start(&netif);
		if (err != ERR_OK) {
			print("dhcp error\n");
			netstat = 0;
		} else {
			for (i = 0; i < 1000; ++i) {
				delay_ms(100);
				if (dhcp_supplied_address(&netif))
					break;
			}
			if (netif_ip4_addr(&netif) != 0) {
#ifdef NETDEBUG
				unsigned int ip4;
				ip4 = netif_ip4_addr(&netif);
				xprintf("IP address : %d.%d.%d.%d\n",
				    (ip4 >> 24) & 0xff,
				    (ip4 >> 16) & 0xff,
				    (ip4 >> 8) & 0xff,
				    ip4 & 0xff);
#endif
			} else {
				print("dhcp can't get address\n");
				netstat = 0;
			}
		}
	} else {
		dns_setserver(0, &dnsserver);
	}
#ifdef NETDEBUG
	xprintf("%x:%x:%x:%x:%x:%x:%x:%x ",
	    IP6_ADDR_BLOCK1(netif_ip6_addr(&netif, 0)),
	    IP6_ADDR_BLOCK2(netif_ip6_addr(&netif, 0)),
	    IP6_ADDR_BLOCK3(netif_ip6_addr(&netif, 0)),
	    IP6_ADDR_BLOCK4(netif_ip6_addr(&netif, 0)),
	    IP6_ADDR_BLOCK5(netif_ip6_addr(&netif, 0)),
	    IP6_ADDR_BLOCK6(netif_ip6_addr(&netif, 0)),
	    IP6_ADDR_BLOCK7(netif_ip6_addr(&netif, 0)),
	    IP6_ADDR_BLOCK8(netif_ip6_addr(&netif, 0)));
	xprintf("%x:%x:%x:%x:%x:%x:%x:%x ",
	    IP6_ADDR_BLOCK1(netif_ip6_addr(&netif, 1)),
	    IP6_ADDR_BLOCK2(netif_ip6_addr(&netif, 1)),
	    IP6_ADDR_BLOCK3(netif_ip6_addr(&netif, 1)),
	    IP6_ADDR_BLOCK4(netif_ip6_addr(&netif, 1)),
	    IP6_ADDR_BLOCK5(netif_ip6_addr(&netif, 1)),
	    IP6_ADDR_BLOCK6(netif_ip6_addr(&netif, 1)),
	    IP6_ADDR_BLOCK7(netif_ip6_addr(&netif, 1)),
	    IP6_ADDR_BLOCK8(netif_ip6_addr(&netif, 1)));
#endif

	udpsntp_raw_pcb = NULL;
}

void
rtl_udp_init()
{
  udpecho_raw_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
}

void
rtl_udp_bind(int port)
{
  udpbuff[0] = '\0';
  if (udpecho_raw_pcb != NULL) {
    err_t err;

    err = udp_bind(udpecho_raw_pcb, IP_ANY_TYPE, port);
    if (err == ERR_OK) {
      udp_recv(udpecho_raw_pcb, udpecho_raw_recv, NULL);
    } else {
      /* abort? output diagnostic? */
    }
  } else {
    /* abort? output diagnostic? */
  }
}

int
rtl_udp_recv(char *buf, int len)
{
int rlen, udplen;

	rlen = 0;

	if (udpbuff[0] != 0) {
		cli();
		udplen = strlen(udpbuff);
		rlen = udplen > len ? len : udplen;
		memcpy(buf, udpbuff, rlen);
		udpbuff[0] = '\0';
		sti();
	}
	return rlen;
}

void
rtl_udp_send(int addr, int port, char *buf, int len)
{
static ip4_addr_t distaddr;

	if (udpecho_raw_pcb != NULL) {
		ip4_addr_set_u32(&distaddr, addr);
		struct pbuf* b = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL);
		memcpy(b->payload, buf, len);
// UDP send must be live gateway. Because of gateware arp resolve at UDP send
// on lwip 2.0.3.
		udp_sendto(udpecho_raw_pcb, b, &distaddr, port);
// same as above
//		udp_connect(udpecho_raw_pcb, &distaddr, port);
//		udp_send(udpecho_raw_pcb, b);
//		udp_disconnect(udpecho_raw_pcb);
		pbuf_free(b);
	}
}


extern unsigned long starttime;

static void
udpsntp_raw_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                 const ip_addr_t *addr, u16_t port)
{
unsigned int timestamp;
unsigned char buf[48];

	LWIP_UNUSED_ARG(arg);
	if (p != NULL) {
		if (p->tot_len == sizeof(buf)) {
			pbuf_copy_partial(p, buf, p->tot_len, 0);
			timestamp = buf[40] << 24 | buf[41] << 16 |
			    buf[42] << 8 | buf[43];
			timestamp -= 2208988800UL;
			reset_counter();
			starttime = timestamp;
		}
 		/* free the pbuf */
		pbuf_free(p);

		udp_remove(udpsntp_raw_pcb);
		udpsntp_raw_pcb == NULL;
	}
}

void sntp(int *addr, int type)
{
int port;
static ip_addr_t distaddr;

	if (udpsntp_raw_pcb != NULL) {
		udp_remove(udpsntp_raw_pcb);
	}

	port = 123;
	if (type == 0) {
		udpsntp_raw_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
		udp_bind(udpsntp_raw_pcb, IP_ANY_TYPE, port);
		ip_addr_set_ip4_u32(&distaddr, *addr);
	} else {
		udpsntp_raw_pcb = udp_new_ip_type(IPADDR_TYPE_V6);
		udp_bind(udpsntp_raw_pcb, netif_ip6_addr(&netif, 1), port);
		distaddr.u_addr.ip6.addr[0] = lwip_htonl((addr[0] << 16) | addr[1]) ;
		distaddr.u_addr.ip6.addr[1] = lwip_htonl((addr[2] << 16) | addr[3]) ;
		distaddr.u_addr.ip6.addr[2] = lwip_htonl((addr[4] << 16) | addr[5]) ;
		distaddr.u_addr.ip6.addr[3] = lwip_htonl((addr[6] << 16) | addr[7]) ;
		distaddr.type = IPADDR_TYPE_V6;
	}
	udp_recv(udpsntp_raw_pcb, udpsntp_raw_recv, NULL);

	unsigned char msg[48]={0,0,0,0,0,0,0,0,0};

	msg[0] = 4 << 3 | 3;

	struct pbuf* b = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_POOL);
	memcpy(b->payload, msg, sizeof(msg));
	udp_sendto(udpsntp_raw_pcb, b, &distaddr, port);
	pbuf_free(b);
}

void cpip6addr(ip_addr_t *addr, int *dst)
{
	dst[0] = IP6_ADDR_BLOCK1((ip6_addr_t *)addr);
	dst[1] = IP6_ADDR_BLOCK2((ip6_addr_t *)addr);
	dst[2] = IP6_ADDR_BLOCK3((ip6_addr_t *)addr);
	dst[3] = IP6_ADDR_BLOCK4((ip6_addr_t *)addr);
	dst[4] = IP6_ADDR_BLOCK5((ip6_addr_t *)addr);
	dst[5] = IP6_ADDR_BLOCK6((ip6_addr_t *)addr);
	dst[6] = IP6_ADDR_BLOCK7((ip6_addr_t *)addr);
	dst[7] = IP6_ADDR_BLOCK8((ip6_addr_t *)addr);
}

int
lookup(char *host, int *addr, int type)
{
err_t err;
int res;
ip_addr_t dnsres;

	res = 0;
	*addr = 0;
	dnsstat = 0;
	err = dns_gethostbyname_addrtype(host,
	    &dnsres, dns_found, NULL, type);
	if (err == ERR_OK) {
		if (type == 0)
			*addr = ip_addr_get_ip4_u32(&dnsres);
		else
			cpip6addr(&dnsres, addr);
		res = 1;
	} else {
		while(dnsstat == 0)
			delay_ms(10);
		if (dnsstat == 1) {
			if (type == 0)
				*addr = ip_addr_get_ip4_u32(&resolvip);
			else
				cpip6addr(&resolvip, addr);
			res = 1;
		}
	}
	return res;
}

int
http_connect(int *addr, int port, char *header, int type)
{
        tcphttp_raw_init(addr, port, type);
        while(tcpstat == 0)
                delay_ms(10);
        if (tcpstat == 1) {
		my_tcp_write(header, strlen(header));
                return 1;
        }
        return 0;
}

int
http_read(char *buf, int len)
{
int rlen = 0;
int i;

	if (tcplen != 0) {
		cli();
		rlen = tcplen > len ? len : tcplen;
		memcpy(buf, tcpbuff + tcpoff, rlen);
		tcplen -= rlen;
		tcpoff += rlen;
		sti();
	} else if (tcpstat >= 2) {
		rlen = -1;
	}
	return rlen;
}

void
http_close()
{
	tcp_close(tcphttp_raw_pcb);
}
