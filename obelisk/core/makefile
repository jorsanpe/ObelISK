#                                                  #
#  ObelISK core makefile                           #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

include ../config.mk

INCDIR:=-I$(ITRONPATH)/core/include
		
PORTDIR = $(ITRONPATH)/core/kernel/port
ARCHDIR = $(ITRONPATH)/core/kernel/$(ARCH)
LIBDIR = $(ITRONPATH)/core/libc

SOURCES = $(wildcard $(ARCHDIR)/*.S $(ARCHDIR)/*.c $(PORTDIR)/*.c $(LIBDIR)/*.c)
NAMES = $(notdir $(basename $(SOURCES)))
TARGETS = $(addsuffix .o,$(NAMES))

AUTOCONF=$(ITRONPATH)/include/autoconf.h

all: core.o | ldsbuild

ldsbuild: $(BINDIR)/$(CPU).lds.S
	@$(CC) $(ASMFLAGS) -x assembler-with-cpp -E -P $(BINDIR)/$(CPU).lds.S -o $(BINDIR)/$(CPU).lds $(INCDIR)

core.o: $(TARGETS)
	@exec echo $(TARGETS)
	$(QLD)$(LD) -r -o $@ $^ $(CFLAGS)
	rm $(TARGETS)
%.o: $(ARCHDIR)/%.S
	$(QAS)$(AS) $(KDEBUG) -c -o $@ $^ $(INCDIR) $(KFLAGS) $(ASMFLAGS) 
%.o: $(ARCHDIR)/%.c
	$(QCC)$(CC) $(KDEBUG) -c -o $@ $^ $(INCDIR) $(KFLAGS) $(OPT)
%.o: $(PORTDIR)/%.c
	$(QCC)$(CC) $(KDEBUG) -c -o $@ $^ $(INCDIR) $(KFLAGS) $(OPT)
%.o: $(LIBDIR)/%.c
	$(QCC)$(CC) $(KDEBUG) -c -o $@ $^ $(INCDIR) $(KFLAGS) $(OPT)
clean:
	rm -f $(ITRONPATH)/core/include/arch
	


	
	