deps_config := \
	drivers/camera/Kconfig \
	drivers/lwip/Kconfig \
	drivers/Kconfig \
	core/Kconfig \
	user/bootloader/Kconfig \
	core/kernel/blackfin/Kconfig \
	Kconfig

.config include/linux/autoconf.h: $(deps_config)

$(deps_config):
