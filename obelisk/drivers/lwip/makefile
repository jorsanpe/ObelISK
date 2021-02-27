#                                                  #
#  lwIP library makefile                           #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

include ../../config.mk
CONFIG_LWIP_BUILD_FEATURES = SOCKET_API
KFLAGS:=-D_KERNEL_
KDEBUG:=-g

#ifeq ($(CONFIG_LWIP_BUILD_FEATURES), )
ifdef CONFIG_LWIP_MINIMAL_FEATURES
C_SOURCES = src/api/err.c \
	src/core/init.c \
	src/core/mem.c \
	src/core/memp.c \
	src/core/netif.c \
	src/core/pbuf.c \
	src/core/stats.c \
	src/core/udp.c \
	src/core/ipv4/icmp.c \
	src/core/ipv4/inet.c \
	src/core/ipv4/ip.c \
	src/core/ipv4/ip_addr.c \
	src/core/ipv4/ip_frag.c \
	src/netif/etharp.c \
	contrib/src/sys_arch.c \
	contrib/src/ethernetif.c \
	contrib/src/lwip_init.c \
	contrib/src/smc9218.c \
	contrib/src/lwip_log.c
endif

#ifeq ($(CONFIG_LWIP_BUILD_FEATURES), SEQUENTIAL_API)
ifdef CONFIG_LWIP_SEQUENTIAL_API
C_SOURCES = src/api/netifapi.c \
	src/api/err.c \
	src/core/dhcp.c \
	src/core/init.c \
	src/core/mem.c \
	src/core/memp.c \
	src/core/netif.c \
	src/core/pbuf.c \
	src/core/raw.c \
	src/core/stats.c \
	src/core/udp.c \
	src/core/ipv4/autoip.c \
	src/core/ipv4/icmp.c \
	src/core/ipv4/igmp.c \
	src/core/ipv4/inet.c \
	src/core/ipv4/ip.c \
	src/core/ipv4/ip_addr.c \
	src/core/ipv4/ip_frag.c \
	src/core/snmp/asn1_dec.c \
	src/core/snmp/asn1_enc.c \
	src/core/snmp/mib2.c \
	src/core/snmp/mib_structs.c \
	src/core/snmp/msg_in.c \
	src/core/snmp/msg_out.c \
	src/netif/etharp.c \
	contrib/src/sys_arch.c \
	contrib/src/ethernetif.c \
	contrib/src/lwip_init.c \
	contrib/src/smc9218.c \
	contrib/src/lwip_log.c
endif

#ifeq ($(CONFIG_LWIP_BUILD_FEATURES), SOCKET_API)
ifdef CONFIG_LWIP_SOCKET_API
C_SOURCES = src/api/api_lib.c \
	src/api/api_msg.c \
	src/api/err.c \
	src/api/netbuf.c \
	src/api/netifapi.c \
	src/api/sockets.c \
	src/api/tcpip.c \
	src/core/dhcp.c \
	src/core/init.c \
	src/core/mem.c \
	src/core/memp.c \
	src/core/netif.c \
	src/core/pbuf.c \
	src/core/raw.c \
	src/core/stats.c \
	src/core/udp.c \
	src/core/tcp.c \
	src/core/tcp_in.c \
	src/core/tcp_out.c \
	src/core/sys.c \
	src/core/ipv4/autoip.c \
	src/core/ipv4/icmp.c \
	src/core/ipv4/igmp.c \
	src/core/ipv4/inet.c \
	src/core/ipv4/inet_chksum.c \
	src/core/ipv4/ip.c \
	src/core/ipv4/ip_addr.c \
	src/core/ipv4/ip_frag.c \
	src/core/snmp/asn1_dec.c \
	src/core/snmp/asn1_enc.c \
	src/core/snmp/mib2.c \
	src/core/snmp/mib_structs.c \
	src/core/snmp/msg_in.c \
	src/core/snmp/msg_out.c \
	src/netif/etharp.c \
	contrib/src/sys_arch.c \
	contrib/src/ethernetif.c \
	contrib/src/lwip_init.c \
	contrib/src/smc9218.c \
	contrib/src/lwip_log.c
endif

TARGETS = $(patsubst %.c, %.o, $(C_SOURCES))

INCDIR:=-I$(ITRONPATH)core/include \
		-I$(ITRONPATH)core/include/arch/$(ARCH)/common \
		-I$(ITRONPATH)core/include/arch/$(ARCH)/$(CPU) \
		-I$(ITRONPATH)drivers/lwip/src/include \
		-I$(ITRONPATH)drivers/lwip/src/include/ipv4 \
		-I$(ITRONPATH)drivers/lwip/contrib/include
		#-I$(ITRONPATH)lwip/contrib/include/arch

all: lwip.o

lwip.o: $(TARGETS)
	$(QLD)$(LD) -r -o lwip.o $(TARGETS) $(CFLAGS)
	$(Q)rm $(TARGETS)
$(TARGETS): %.o: %.c
	$(QCC)$(CC) $(KDEBUG) -c -o $@ $< $(INCDIR) $(KFLAGS) $(OPT)
clean:
	rm $(TARGETS) lwip.o

