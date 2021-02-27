#                                                  #
#  ObelISK examples makefile                       #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

include ../config.mk

LDS = -T$(BINDIR)/$(CPU).lds -L$(BINDIR)
SOURCES = $(wildcard $(ITRONPATH)/examples/*.c)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .obk,$(NAMES))
OBELISK = #$(BINDIR)/obelisk.o

DRIVERS = #$(ITRONPATH)drivers/flash.o
ULIBC = $(ITRONPATH)/user/ulibc/ulibc.a

INCPATH = -I$(ITRONPATH)user/ulibc/include \
		-I$(ITRONPATH)drivers/include

#ifdef CONFIG_USING_LWIP
INCPATH += -I$(ITRONPATH)drivers/lwip/src/include \
		-I$(ITRONPATH)drivers/lwip/src/include/ipv4 \
		-I$(ITRONPATH)drivers/lwip/contrib/include
#endif

ifdef V
QLD = 
QCC =
endif

DEBUG = -g

all: $(TARGETS)

%.obk: %.o
	$(QLD)$(LD) -o $@ $^ $(OBELISK) $(LWIP) --start-group $(ULIBC) --end-group $(LDS) -I$(ITRONPATH)/bin
	rm $^

%.o: %.c
	$(QCC)$(CC) $(DEBUG) -c -o $@ $^ -Y $(INCPATH) -mcpu=$(CPU) $(CFLAGS) $(Q2)
	
clean:
	rm *.obk
