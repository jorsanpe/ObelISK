/*
 * $FILE: head.S
 *
 * Blackfin processor boot procedure
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

#include <linkage.h>
#include <processor.h>
#include <asm-offsets.h>
#include <autoconf.h>
#include <arch/sysmem.h>

.section .init.text, "ax"
ENTRY(__start)
	/* Enable Cycle Counter and Nesting Of Interrupts */
#ifdef CONFIG_BFIN_SCRATCH_REG_CYCLES
	R0 = SYSCFG_SNEN;
#else
	R0 = SYSCFG_SNEN | SYSCFG_CCEN;
#endif
	SYSCFG = R0;
	R0 = 0;

	/* Clear Out All the data and pointer Registers */
	R1 = R0;
	R2 = R0;
	R3 = R0;
	R4 = R0;
	R5 = R0;
	R6 = R0;

	P0 = R0;
	P1 = R0;
	P2 = R0;
	P3 = R0;
	P4 = R0;
	P5 = R0;

	// Clear loop counters to disable hardware loops
	LC0 = r0;
	LC1 = r0;

	// Clear the DAG Length regs, to force linerar addressing
	l0 = r0;
	l1 = r0;
	l2 = r0;
	l3 = r0;

	// Clear Out All the DAG Registers
	B0 = r0;
	B1 = r0;
	B2 = r0;
	B3 = r0;

	I0 = r0;
	I1 = r0;
	I2 = r0;
	I3 = r0;

	M0 = r0;
	M1 = r0;
	M2 = r0;
	M3 = r0;

	p0.L = LO(DTEST_COMMAND);
	p0.H = HI(DTEST_COMMAND);
	[p0] = R0;
	[p0 + (ITEST_COMMAND - DTEST_COMMAND)] = R0;
	CSYNC;

	// Turn off the icache
	p0.l = LO(IMEM_CONTROL);
	p0.h = HI(IMEM_CONTROL);
	R1 = [p0];
	R0 = ~ENICPLB;
	R0 = R0 & R1;
	[p0] = R0;
	SSYNC;

	// Turn off the dcache
	p0.l = LO(DMEM_CONTROL);
	p0.h = HI(DMEM_CONTROL);
	R1 = [p0];
	R0 = ~ENDCPLB;
	R0 = R0 & R1;
	[p0] = R0;
	SSYNC;

	// Initialize stack pointer
	sp.l = LO(L1_KSCRATCH_BASE + L1_KSCRATCH_LENGTH - 8);
	sp.h = HI(L1_KSCRATCH_BASE + L1_KSCRATCH_LENGTH - 8);
	fp = sp;

	// Jump to the start code keeping kernel mode
	p0.l = lo(EVT15);
	p0.h = hi(EVT15);
	p1.l = _real_start;
	p1.h = _real_start;
	[p0] = p1;
	csync;

	r0 = EVT_IVG15 (z);
	sti r0;

	raise 15;
	p0.l = .LWAIT_HERE;
	p0.h = .LWAIT_HERE;
	reti = p1;
#if ANOMALY_05000281
	nop; nop; nop;
#endif
	rti;

.LWAIT_HERE:
	idle;
	jump .LWAIT_HERE;
ENDPROC(__start)

ENTRY(__init_clear_bss)
	r2 = r2 - r1;
	cc = r2 == 0;
	if cc jump .L_bss_done;
	r2 >>= 2;
	p1 = r1;
	p2 = r2;
	lsetup (1f, 1f) lc0 = p2;
1:	[p1++] = r0;
.L_bss_done:
	rts;
ENDPROC(__init_clear_bss)

ENTRY(_sdram_setup)
	P0.L = EBIU_RSTCTL & 0xFFFF;
	P0.H = (EBIU_RSTCTL >> 16) & 0xFFFF;
	R0 = W[P0];
	R1 = 3;
	R0 = R0 | R1;
	W[P0] = R0;
	SSYNC;

	P0.L = EBIU_AMGCTL & 0xFFFF;
	P0.H = (EBIU_AMGCTL >> 16) & 0xFFFF;
	R0 = 0x000E(Z);
	W[P0] = R0;
	SSYNC;

	P0.L = EBIU_DDRCTL0 & 0xFFFF;
	P0.H = (EBIU_DDRCTL0 >> 16) & 0xFFFF;
	R0.L = 0x8287;
	//R0.L = 0x8411;
	//R0.L = 0x83FE;
	R0.H = 0x218A;
	[P0] = R0;
	SSYNC;
	P0.L = EBIU_DDRCTL1 & 0xFFFF;
	P0.H = (EBIU_DDRCTL1 >> 16) & 0xFFFF;
	R0.L = 0x2222;
	R0.H = 0x2002;
	[P0] = R0;
	SSYNC;
	P0.L = EBIU_DDRCTL2 & 0xFFFF;
	P0.H = (EBIU_DDRCTL2 >> 16) & 0xFFFF;
	R0.L = 0x0021;
	R0.H = 0x0000;
	[P0] = R0;
	SSYNC;
	rts;
ENDPROC(_sdram_setup)

ENTRY(_real_start)
	/* Enable nested interrupts */
	[--sp] = reti;

	/* Clear bss sections */
	r0 = 0 (z);

/*
#ifdef CONFIG_USING_LWIP
	r1.l = _sbss_lwip;
	r1.h = _sbss_lwip;
	r2.l = _ebss_lwip;
	r2.h = _ebss_lwip;
	call __init_clear_bss;
#endif*/

	r1.l = _skbss;
	r1.h = _skbss;
	r2.l = _ekbss;
	r2.h = _ekbss;
	call __init_clear_bss;

    /* Basic hardware initializations */
    sp += -12;
	call _set_PLL;
	call _sdram_setup;
	call _install_default_handlers;
	sp += 12;

	/* watchdog off for now */
	p0.l = lo(WDOG_CTL);
	p0.h = hi(WDOG_CTL);
	r0 = 0xAD6(z);
	w[p0] = r0;
	ssync;

	fp = sp;
	jump _start_kernel;
ENDPROC(_real_start)

.previous
