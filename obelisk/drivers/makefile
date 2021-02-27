#                                                  #
#  ObelISK drivers makefile                        #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

include ../config.mk

flash-$(CONFIG_ISFLASH) += flash/isflash.o
camera-$(CONFIG_0V7660) += camera/ov7660.o
can-$(CONFIG_BFCAN) += can/bfcan.o

drivers = $(flash-y) $(camera-y) $(can-y)

INCDIR:=-I$(ITRONPATH)/core/include \
		-I$(ITRONPATH)/drivers/include

DRIVERS = $(patsubst %.c, %.o, $(drivers))
TARGETS = $(DRIVERS)

ifdef CONFIG_USING_LWIP
TARGETS += lwip.o
endif
	
all: drivers.o

drivers.o: $(TARGETS)
	$(QLD)$(LD) -r -o $@ $^ $(CFLAGS)
	$(Q)rm $^
	#$(Q)mv $@ $(ITRONPATH)/bin

$(DRIVERS): %.o: %.c
	$(QCC)$(CC) $(KDEBUG) -c -o $@ $< $(INCDIR) $(KFLAGS) $(OPT)

lwip.o:
	$(Q3)$(MAKE) -C lwip
	@mv lwip/lwip.o .

clean:
	@find -name "*~" -exec rm '{}' \;  $(Q2)
	@find -name "*.o" -exec rm '{}' \;  $(Q2)
