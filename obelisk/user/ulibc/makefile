#                                                  #
#  ObelISK ulibc makefile                           #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

include ../../config.mk

INCDIR = -I$(ITRONPATH)/user/ulibc/include

SOURCES = $(wildcard libc/*.c) $(wildcard $(ARCH)/*.S) $(wildcard $(ARCH)/*.c)
NAMES = $(notdir $(basename $(SOURCES)))
TARGETS = $(addsuffix .o,$(NAMES))

#ifdef USING_LWIP
INCDIR += -I$(ITRONPATH)/drivers/lwip/src/include \
		  -I$(ITRONPATH)/drivers/lwip/contrib/include \
		  -I$(ITRONPATH)/drivers/lwip/src/include/ipv4
#endif

ulibc.a: $(TARGETS)
	$(QAR)$(AR) -r ulibc.a $(TARGETS)
	rm *.o

%.o: libc/%.c
	$(QCC)$(CC) $(KDEBUG) -c -o $@ $^ $(INCDIR)

%.o: $(ARCH)/%.S
	$(QAS)$(AS) $(KDEBUG) -c -o $@ $^ $(INCDIR)

%.o: $(ARCH)/%.c
	$(QCC)$(CC) $(KDEBUG) -c -o $@ $^ $(INCDIR)

clean:
	@find -name "*.o" -exec rm '{}' \;  $(Q2)
	@find -name "*.a" -exec rm '{}' \;  $(Q2)
