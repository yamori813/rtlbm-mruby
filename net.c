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

#include "asicregs.h"
#include "intr.h"
#include "rtlregs.h"
#include "system.h"

unsigned char debug_flags;

struct netif netif;

static ip4_addr_t ipaddr, netmask, gw, dnsserver, dnsres;

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
char tcpbuff[1024*8];
int tcplen;
int tcpoff;

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
udpecho_raw_init(int port)
{
  udpecho_raw_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
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
  tcpstat = 2;
  print("tcp_abort");
  return ERR_OK;
}

static void
http_err(void *arg, err_t err)
{
  tcpstat = 2;
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
tcphttp_raw_init(int disaddr, int disport)
{
static ip4_addr_t addr;

  ip4_addr_set_u32(&addr, disaddr);

  tcphttp_raw_pcb = tcp_new();
  if (tcphttp_raw_pcb != NULL) {
//    tcp_arg(tcphttp_raw_pcb, NULL);
    tcp_recv(tcphttp_raw_pcb, http_recv);
    tcp_sent(tcphttp_raw_pcb, http_sent);
    tcp_err(tcphttp_raw_pcb, http_err);
    tcp_poll(tcphttp_raw_pcb, http_poll, 10);
    tcp_connect(tcphttp_raw_pcb, &addr, disport, http_connected);
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
int resolvip;

static void
dns_found(const char* hostname, const ip_addr_t *ipaddr, void *arg)
{
  LWIP_UNUSED_ARG(arg);

  if (ipaddr != 0) {
    resolvip = ip_addr_get_ip4_u32(ipaddr);
    dnsstat = 1;
  } else {
    dnsstat = 2;
  }
}

void net_init()
{
long *lptr;
err_t err;
int i;

	gethwmac(eth0_mac);

	swCore_init();

#if USE_DHCP
	IP4_ADDR(&ipaddr, 0,0,0,0);
	IP4_ADDR(&netmask, 0,0,0,0);
	IP4_ADDR(&gw, 0,0,0,0);
#else
	IP4_ADDR(&ipaddr, 10,0,1,222);
	IP4_ADDR(&netmask, 255,255,255,0);
	IP4_ADDR(&gw, 10,0,1,1);
	IP4_ADDR(&dnsserver, 10,0,1,1);
#endif

	lwip_init();
#if USE_DHCP
	netif_add(&netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw), NULL, ethernetif_init,
#else
	netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init,
#endif
	    ethernet_input);

	vlan_init();

	lptr = (unsigned long *)IRR1;
	*lptr |= (3 << 0);
	request_IRQ(8, &irq_Ether, NULL);

	netif_set_default(&netif);
	netif_set_up(&netif);   /* send broadcast arp packet */

	netstat = 1;
#if USE_DHCP
	err = dhcp_start(&netif);
	if (err != ERR_OK) {
		print("dhcp error\n");
		netstat = 0;
	} else {
		for (i = 0; i < 1000; ++i) {
			delay_ms(100);
			if (netif.ip_addr.addr != 0)
				break;
		}
		if (netif.ip_addr.addr != 0) {
			xprintf("IP address : %d.%d.%d.%d\n",
			    (netif.ip_addr.addr >> 24) & 0xff,
			    (netif.ip_addr.addr >> 16) & 0xff,
			    (netif.ip_addr.addr >> 8) & 0xff,
			    netif.ip_addr.addr & 0xff);
		} else {
			print("dhcp can't get address\n");
			netstat = 0;
		}
	}
#else
	dns_setserver(0, &dnsserver);
#endif

}

void
rtl_udp_bind(int port)
{
	udpbuff[0] = '\0';
	udpecho_raw_init(port);
}

int
rtl_udp_recv(char *buf, int len)
{
int rlen, udplen;

	rlen = 0;

	if (udpbuff[0] != 0) {
		cli();
		rlen = udplen > len ? len : udplen;
		memcpy(buf, udpbuff, rlen);
		sti();
	}
	return rlen;
}

int
lookup(char *host, int *addr)
{
err_t err;
int res;

	res = 0;
	*addr = 0;
	dnsstat = 0;
	err = dns_gethostbyname(host,
	    &dnsres, dns_found, NULL);
	if (err == ERR_OK) {
		*addr = ip_addr_get_ip4_u32(&dnsres);
		res = 1;
	} else {
		while(dnsstat == 0)
			delay_ms(10);
		if (dnsstat == 1) {
			*addr = resolvip;
			res = 1;
		}
	}
	return res;
}

int
http_connect(int addr, int port, char *header)
{
        tcphttp_raw_init(addr, port);
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
	} else if (tcpstat == 2) {
		rlen = -1;
	}
	return rlen;
}

void
http_close()
{
	tcp_close(tcphttp_raw_pcb);
}
