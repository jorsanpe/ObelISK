#                                                  #
#  ObelISK makefile configuration file             #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

# Makefile configuration
CC:=bfin-elf-gcc
AS:=bfin-elf-gcc -x assembler-with-cpp
LD:=bfin-elf-ld
AR:=bfin-elf-ar
OBJCOPY=bfin-elf-objcopy
CFLAGS:=-nostdlib
KFLAGS:=-D_KERNEL_
ASMFLAGS:=-D__ASSEMBLY__
KDEBUG:=-g
OPT=#-O2

#ifndef ITRONPATH
#
#endif

clean-targets := clean distclean
config-targets := config xconfig menuconfig oldconfig silentconfig help
.PHONY: $(clean-targets) $(config-targets) scripts_basic obelisk

ifndef ITRONPATH
ITRONPATH=$(shell echo `pwd`/ | sed -n 's|\(\(obelisk[^/.]*/\)\+\).*|\1|p;' )
#ITRONPATH=$(shell echo `pwd`)
BINDIR = $(ITRONPATH)/bin
endif

# skip .config when configuring
ifeq ($(findstring $(MAKECMDGOALS), $(config-targets) $(clean-targets)),)
need_config := 1
endif

# check if the .config exists
ifeq ($(ITRONPATH)/.config, $(wildcard $(ITRONPATH)/.config))
exists_config := 1
endif

# if there's no .config file warn the user and abort
$(if $(need_config), \
	$(if $(exists_config),, \
	$(warning "No .config file found at $(ITRONPATH),") \
	$(error "run `make menuconfig` in the base directory")))

# if the .config is needed, include it
ifdef exists_config
include $(ITRONPATH)/.config

# By default it is quiet. If "make V=1" then verbose.
quiet		= quiet_
QCC		= echo -n "\t CC $@ \n";
QAS		= echo -n "\t AS $@ \n";
QAR		= echo -n "\t AR $@ \n";
QLD		= echo -n "\t LD $@ \n";
QOBJCOPY= echo -n "\t OBJCOPY $@ \n";
Q2		=> /dev/null 2>&1
Q3		= 
MAKEFLAGS 	+= --no-print-directory --quiet

#ifdef CONFIG_VERBOSE_COMPILE
	#V=1
#endif

ifdef V 
  ifeq ($(V),1)
    quiet 	=
    Q 		=
    Q2		=
    Q3		=
    MAKEFLAGS 	=
  endif
endif

ifdef CONFIG_ARCH_BLACKFIN
ARCH = blackfin
ifdef CONFIG_CPU_BF548
CPU = bf548
endif
endif
endif
