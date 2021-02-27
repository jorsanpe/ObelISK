#                                                  #
#  ObelISK examples makefile                       #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

include ../config.mk

ifdef CONFIG_USING_LWIP
LWIP = #$(BINDIR)/lwip.o
LDS = -T$(BINDIR)/$(CPU)_lwip.lds -L$(BINDIR)
else
LWIP = 
LDS = -T$(BINDIR)/$(CPU).lds -L$(BINDIR)
endif

SOURCES = $(wildcard $(ITRONPATH)/examples/*.c)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .dxe,$(NAMES))
ITRON = #$(BINDIR)/obelisk.o

DRIVERS = $(ITRONPATH)drivers/flash.o

INCPATH = -I$(ITRONPATH)user/ulibc/include \
		-I$(ITRONPATH)drivers/include

ifdef CONFIG_USING_LWIP
INCPATH += -I$(ITRONPATH)lwip/src/include \
		-I$(ITRONPATH)lwip/src/include/ipv4 \
		-I$(ITRONPATH)lwip/contrib/include
endif

ifdef V
QLD = 
QCC =
endif

DEBUG = -g

all: $(TARGETS)

%.dxe: %.o
	$(QLD)$(LD) -o $@ $^ $(ITRON) $(LWIP) $(DRIVERS) $(LDS) -I$(ITRONPATH)/bin
	rm $^

%.o: %.c
	$(QCC)$(CC) $(DEBUG) -c -o $@ $^ -Y $(INCPATH) -mcpu=$(CPU) $(CFLAGS) $(Q2)
	
clean:
	rm *.dxe