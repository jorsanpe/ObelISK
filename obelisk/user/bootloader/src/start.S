/*
 * $FILE: head.S
 *
 * Blackfin processor minimal boot procedure
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

__section(.init.text), "ax"
ENTRY(_start)
ENTRY(start)

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

	/* Clear Out All the DAG Registers */
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

	/* Turn off the icache */
	p0.l = LO(IMEM_CONTROL);
	p0.h = HI(IMEM_CONTROL);
	R1 = [p0];
	R0 = ~ENICPLB;
	R0 = R0 & R1;
	[p0] = R0;
	SSYNC;

	/* Turn off the dcache */
	p0.l = LO(DMEM_CONTROL);
	p0.h = HI(DMEM_CONTROL);
	R1 = [p0];
	R0 = ~ENDCPLB;
	R0 = R0 & R1;
	[p0] = R0;
	SSYNC;

	/* Initialize sdram */
	call _sdram_setup;

	/* Initialize boot stack pointer */
	sp.l = _boot_stack;
	sp.h = _boot_stack;
	fp = sp;
	usp = sp;

	/* Jump to the bootloader main code */
	call _boot_main;

	jump _halt_system;

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

ENTRY(_halt_system)
    jump _halt_system;
ENDPROC(_halt_system)

