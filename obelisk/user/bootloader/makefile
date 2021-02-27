#                                                  #
#  ObelISK bootloader makefile                     #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

include ../../config.mk

ASMFLAGS:=-D__ASSEMBLY__
KFLAGS:=-D_KERNEL_

INCDIR:=-I$(ITRONPATH)/user/libc/include \
		-I$(ITRONPATH)/core/include/arch/$(ARCH)/common \
		-I$(ITRONPATH)/core/include/arch/$(ARCH)/$(CPU)
		
SOURCES = src/bloader.c src/start.S
NAMES = $(notdir $(basename $(SOURCES)))
TARGETS = $(addsuffix .o,$(NAMES))

LDS = -T./boot.lds

all: bootloader.bin 

bootloader.bin: $(TARGETS)
	$(QLD)$(LD) -o bootloader.o $(TARGETS) $(CFLAGS) $(LDS)
	$(QOBJCOPY)$(OBJCOPY) bootloader.o -O binary bootloader.bin
	rm *.o
%.o: %.S
	$(QAS)$(AS) $(KDEBUG) -c -o $@ $^ -mcpu=$(CPU) $(INCDIR) $(KFLAGS) $(ASMFLAGS)
%.o: %.c
	$(QCC)$(CC) $(KDEBUG) -c -o $@ $^ -mcpu=$(CPU) $(INCDIR) $(KFLAGS)
clean:
	@find -name "bootloader.bin" -exec rm '{}' \;  $(Q2)
