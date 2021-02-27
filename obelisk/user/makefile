#                                                  #
#  ObelISK user libraries makefile                 #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

include ../config.mk

ULIBCPATH = $(ITRONPATH)/user/ulibc

all: 
	$(Q)if [ ! -d $(ULIBCPATH)/include/arch ] ; then \
		ln -sf $(ULIBCPATH)/include/$(ARCH) $(ULIBCPATH)/include/arch; \
	fi
	$(Q)if [ ! -d $(ULIBCPATH)/include/kernel_inc ] ; then \
		ln -sf $(ITRONPATH)/core/include $(ULIBCPATH)/include/kernel_inc; \
	fi
	$(Q3)$(MAKE) -C ulibc

clean:
	$(Q3)$(MAKE) -C ulibc clean
	rm -f $(ULIBCPATH)/include/arch
	rm -f $(ULIBCPATH)/include/kernel_inc



