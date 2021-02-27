/*
 * $FILE: timer.c
 *
 * Timers handling
 *
 * Author: Jordi Sánchez <jorsanp3@upvnet.upv.es>
 *
 * $LICENSE:
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <processor.h>
#include <arch/timer.h>
#include <linkage.h>
#include <itron.h>
#include <stddef.h>
#include <arch/sdram_init.h>

#define USEC_DIV  1000000

// Exported from "entry.S"
extern void evt_timer(void);

/* Writing to PLL_CTL initiates a PLL relock sequence. */
__init static inline void bfin_write_PLL_CTL(unsigned int val)
{
	unsigned long flags, iwr0, iwr1, iwr2;

	if (val == bfin_read_PLL_CTL())
		return;

	__hw_cli();

	/* Enable the PLL Wakeup bit in SIC IWR */
	iwr0 = bfin_read32(SIC_IWR0);
	iwr1 = bfin_read32(SIC_IWR1);
	iwr2 = bfin_read32(SIC_IWR2);
	/* Only allow PPL Wakeup) */
	bfin_write32(SIC_IWR0, IWR_ENABLE(0));
	bfin_write32(SIC_IWR1, 0);
	bfin_write32(SIC_IWR2, 0);

	bfin_write16(PLL_CTL, val);
	asm("SSYNC;");
	asm("IDLE;");

	bfin_write32(SIC_IWR0, iwr0);
	bfin_write32(SIC_IWR1, iwr1);
	bfin_write32(SIC_IWR2, iwr2);

	__hw_sti();
}

/* Writing to VR_CTL initiates a PLL relock sequence. */
__init static inline void bfin_write_VR_CTL(unsigned int val)
{
	unsigned long flags, iwr0, iwr1, iwr2;

	if (val == bfin_read_VR_CTL())
		return;

	__hw_cli();
	/* Enable the PLL Wakeup bit in SIC IWR */
	iwr0 = bfin_read32(SIC_IWR0);
	iwr1 = bfin_read32(SIC_IWR1);
	iwr2 = bfin_read32(SIC_IWR2);
	/* Only allow PPL Wakeup) */
	bfin_write32(SIC_IWR0, IWR_ENABLE(0));
	bfin_write32(SIC_IWR1, 0);
	bfin_write32(SIC_IWR2, 0);

	bfin_write16(VR_CTL, val);
	asm("SSYNC;");
	asm("IDLE;");

	bfin_write32(SIC_IWR0, iwr0);
	bfin_write32(SIC_IWR1, iwr1);
	bfin_write32(SIC_IWR2, iwr2);
	local_irq_restore_hw(flags);
	__hw_sti();
}

/*
 * set_PLL():
 *
 *    Set the PLL MMR to adjust the core and system clock.
 */
__init void set_PLL(void)
{
   uint16_t cclk_mult = CONFIG_CCLK / CONFIG_CLKIN;
   uint16_t sclk_div = CONFIG_CCLK / CONFIG_SCLK;

   bfin_write_PLL_CTL((cclk_mult << 9) | ENABLE_DF);
   bfin_write_PLL_DIV(sclk_div);
}


/*
 * init_core_timer():
 *
 *    Set the periodic timer for the kernel.
 */
__init void init_core_timer(void)
{
   // Setup the core timer
   bfin_write_TCNTL(0);
   bfin_write_TSCALE(0);
   bfin_write_TPERIOD(CONFIG_CTMR*(CONFIG_CCLK/USEC_DIV));
   bfin_write_TCNTL(TAUTORLD | TMPWR | TMREN);

   // Install the isr
   bfin_write_EVT6(evt_timer);
   bfin_write_IMASK(bfin_read_IMASK() | MK_BMSK_(6));
}

void init_timer()
{

}




