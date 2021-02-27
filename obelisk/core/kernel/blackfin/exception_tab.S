/*
 * $FILE: exception_tab.S
 *
 * Exception Table
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
 */

#include <processor.h>
#include <linkage.h>

.section .extable, "wa"
ENTRY(_ex_table)
	.long _syscall_prolog /* 0x00 - System Call */
	.long _trap_c       /* 0x01 - User Defined */
	.long _trap_c   	/* 0x02 - User Defined */
	.long _trap_c       /* 0x03 - User Defined */
	.long _trap_c       /* 0x04 - User Defined */
	.long _trap_c   	/* 0x05 - User Defined */
	.long _trap_c   	/* 0x06 - User Defined */
	.long _trap_c   	/* 0x07 - User Defined */
	.long _trap_c   	/* 0x08 - User Defined */
	.long _trap_c   	/* 0x09 - User Defined */
	.long _trap_c   	/* 0x0A - User Defined */
	.long _trap_c   	/* 0x0B - User Defined */
	.long _trap_c   	/* 0x0C - User Defined */
	.long _trap_c   	/* 0x0D - User Defined */
	.long _trap_c   	/* 0x0E - User Defined */
	.long _trap_c   	/* 0x0F - User Defined */
	.long _trap_c   	/* 0x10 - HW Single step */
	.long _trap_c       /* 0x11 - Trace Buffer Full */
	.long _trap_c       /* 0x12 - Reserved */
	.long _trap_c       /* 0x13 - Reserved */
	.long _trap_c       /* 0x14 - Reserved */
	.long _trap_c       /* 0x15 - Reserved */
	.long _trap_c       /* 0x16 - Reserved */
	.long _trap_c       /* 0x17 - Reserved */
	.long _trap_c       /* 0x18 - Reserved */
	.long _trap_c       /* 0x19 - Reserved */
	.long _trap_c       /* 0x1A - Reserved */
	.long _trap_c       /* 0x1B - Reserved */
	.long _trap_c       /* 0x1C - Reserved */
	.long _trap_c       /* 0x1D - Reserved */
	.long _trap_c       /* 0x1E - Reserved */
	.long _trap_c       /* 0x1F - Reserved */
	.long _trap_c       /* 0x20 - Reserved */
	.long _trap_c       /* 0x21 - Undefined Instruction */
	.long _trap_c       /* 0x22 - Illegal Instruction Combination */
	.long _trap_c       /* 0x23 - Data CPLB Protection Violation */
	.long _trap_c       /* 0x24 - Data access misaligned */
	.long _trap_c       /* 0x25 - Unrecoverable Event */
	.long _dcplb_miss   /* 0x26 - Data CPLB Miss */
	.long _trap_c       /* 0x27 - Data CPLB Multiple Hits - Linux Trap Zero */
	.long _trap_c       /* 0x28 - Emulation Watchpoint */
	.long _trap_c       /* 0x29 - Instruction fetch access error (535 only) */
	.long _trap_c       /* 0x2A - Instruction fetch misaligned */
	.long _trap_c       /* 0x2B - Instruction CPLB protection Violation */
	.long _icplb_miss 	/* 0x2C - Instruction CPLB miss */
	.long _trap_c       /* 0x2D - Instruction CPLB Multiple Hits */
	.long _trap_c       /* 0x2E - Illegal use of Supervisor Resource */
	.long _trap_c       /* 0x2E - Illegal use of Supervisor Resource */
	.long _trap_c       /* 0x2F - Reserved */
	.long _trap_c       /* 0x30 - Reserved */
	.long _trap_c       /* 0x31 - Reserved */
	.long _trap_c       /* 0x32 - Reserved */
	.long _trap_c       /* 0x33 - Reserved */
	.long _trap_c       /* 0x34 - Reserved */
	.long _trap_c       /* 0x35 - Reserved */
	.long _trap_c       /* 0x36 - Reserved */
	.long _trap_c       /* 0x37 - Reserved */
	.long _trap_c       /* 0x38 - Reserved */
	.long _trap_c       /* 0x39 - Reserved */
	.long _trap_c       /* 0x3A - Reserved */
	.long _trap_c       /* 0x3B - Reserved */
	.long _trap_c       /* 0x3C - Reserved */
	.long _trap_c       /* 0x3D - Reserved */
	.long _trap_c       /* 0x3E - Reserved */
	.long _trap_c       /* 0x3F - Reserved */
END(_ex_table)
.previous
