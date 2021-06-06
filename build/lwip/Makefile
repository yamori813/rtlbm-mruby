#
#
#

CC=mips-cc
AR=mips-ar

NEWLIB=../../newlib-3.0.0.20180831

CROSS_CFLAGS = -Iinclude -I$(SRC)/include -I$(NEWLIB)/newlib/libc/include

CROSS_CFLAGS += -march=4181
#CROSS_CFLAGS += -march=5281
#CROSS_CFLAGS += -march=4181 -Os -g -fno-pic -mno-abicalls
CROSS_CFLAGS += -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0
CROSS_CFLAGS += -pipe -mlong-calls

OBJS_CORE= def.o dns.o inet_chksum.o init.o ip.o mem.o memp.o netif.o pbuf.o raw.o stats.o sys.o tcp.o tcp_in.o tcp_out.o timeouts.o udp.o

OBJS_IPV4=autoip.o dhcp.o etharp.o icmp.o igmp.o ip4.o ip4_addr.o ip4_frag.o
OBJS_IPV6= dhcp6.o ethip6.o icmp6.o inet6.o ip6_addr.o ip6.o ip6_frag.o mld6.o nd6.o


OBJS_DRV=ethernet.o

SRC=../src

all:	$(OBJS_CORE) $(OBJS_IPV4) $(OBJS_IPV6) $(OBJS_DRV)
	ar rs liblwip.a $(OBJS_CORE) $(OBJS_IPV4) $(OBJS_IPV6) $(OBJS_DRV)

def.o:$(SRC)/core/def.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/def.c
dns.o:$(SRC)/core/dns.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/dns.c
inet_chksum.o:$(SRC)/core/inet_chksum.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/inet_chksum.c
init.o:$(SRC)/core/init.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/init.c
ip.o:$(SRC)/core/ip.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ip.c
mem.o:$(SRC)/core/mem.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/mem.c
memp.o:$(SRC)/core/memp.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/memp.c
netif.o:$(SRC)/core/netif.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/netif.c
pbuf.o:$(SRC)/core/pbuf.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/pbuf.c
raw.o:$(SRC)/core/raw.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/raw.c
stats.o:$(SRC)/core/stats.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/stats.c
sys.o:$(SRC)/core/sys.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/sys.c
tcp.o:$(SRC)/core/tcp.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/tcp.c
tcp_in.o:$(SRC)/core/tcp_in.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/tcp_in.c
tcp_out.o:$(SRC)/core/tcp_out.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/tcp_out.c
timeouts.o:$(SRC)/core/timeouts.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/timeouts.c
udp.o:$(SRC)/core/udp.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/udp.c

autoip.o:$(SRC)/core/ipv4/autoip.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv4/autoip.c
dhcp.o:$(SRC)/core/ipv4/dhcp.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv4/dhcp.c
etharp.o:$(SRC)/core/ipv4/etharp.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv4/etharp.c
icmp.o:$(SRC)/core/ipv4/icmp.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv4/icmp.c
igmp.o:$(SRC)/core/ipv4/igmp.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv4/igmp.c
ip4.o:$(SRC)/core/ipv4/ip4.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv4/ip4.c
ip4_addr.o:$(SRC)/core/ipv4/ip4_addr.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv4/ip4_addr.c
ip4_frag.o:$(SRC)/core/ipv4/ip4_frag.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv4/ip4_frag.c

dhcp6.o:$(SRC)/core/ipv6/dhcp6.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv6/dhcp6.c
ethip6.o:$(SRC)/core/ipv6/ethip6.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv6/ethip6.c
icmp6.o:$(SRC)/core/ipv6/icmp6.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv6/icmp6.c
inet6.o:$(SRC)/core/ipv6/inet6.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv6/inet6.c
ip6_addr.o:$(SRC)/core/ipv6/ip6_addr.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv6/ip6_addr.c
ip6.o:$(SRC)/core/ipv6/ip6.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv6/ip6.c
ip6_frag.o:$(SRC)/core/ipv6/ip6_frag.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv6/ip6_frag.c
mld6.o:$(SRC)/core/ipv6/mld6.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv6/mld6.c
nd6.o:$(SRC)/core/ipv6/nd6.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/core/ipv6/nd6.c


ethernet.o:$(SRC)/netif/ethernet.c
	$(CC) $(CROSS_CFLAGS) -c $(SRC)/netif/ethernet.c

clean:
	rm -rf *.o *.a
