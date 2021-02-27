#                                                  #
#  ObelISK makefile                                #
#                                                  #
#  Author: Jordi Sánchez, jorsanp3@upvnet.upv.es   #
#                                                  #
####################################################

include config.mk

all: |links obelisk.o libraries examples

links:
	if [ ! -d $(ITRONPATH)/core/include/arch ] ; then \
		ln -sf $(ITRONPATH)/core/include/$(ARCH) $(ITRONPATH)/core/include/arch; \
	fi

obelisk.o:
	@exec sh ./scripts/asm-offsets.sh
	@exec echo -e ">> Building ObelISK core:"
	$(Q3)$(MAKE) -C core all
	@exec echo -e  "\r>> Building ObelISK core: done                             "
	@exec echo -e ">> Building drivers:"
	$(Q3)$(MAKE) -C drivers all
	@exec echo -e  "\r>> Building drivers: done                                    "
	$(QLD)$(LD) -r -o bin/$@ core/core.o drivers/drivers.o $(CFLAGS)
	
libraries:
	@exec echo -e ">> Building user libraries:"
	$(Q3)$(MAKE) -C user all
	@exec echo -e  "\r>> Building user libraries: done                             "

examples:
	@exec echo -e ">> Building ObelISK examples:"
	$(Q3)$(MAKE) -i -C examples all
	@exec echo -e  "\r>> Building ObelISK examples: done                          "
	@exec echo
	@exec echo ">> Include these in your profile environment:"
	@exec echo "   ITRON=$(ITRONPATH) export ITRON"
	@exec echo '   PATH=$$PATH:$$ITRON/bin export PATH'
	
menuconfig:
	@exec echo -n -e "\b \n>> Building the configuration utility:";
	$(Q3)$(MAKE) -C scripts all
	@scripts/kconfig/mconf Kconfig
	
clean:
	@exec echo -ne ">> Cleaning ObelISK build:"
	$(Q3)$(MAKE) -C core clean
	$(Q3)$(MAKE) -C scripts clean
	$(Q3)$(MAKE) -C user clean
	$(Q3)$(MAKE) -C drivers clean
	@find -name "*~" -exec rm '{}' \;  $(Q2)
	@find -name "*.a" -exec rm '{}' \;  $(Q2)
	@find -name "*.o" -exec rm '{}' \;  $(Q2)
	@find -name "*.obk" -exec rm '{}' \;  $(Q2)
	@find -name "*.lds" -exec rm '{}' \;  $(Q2)
	@exec echo -e  "\r>> Cleaning ObelISK build: done                          "
     
     