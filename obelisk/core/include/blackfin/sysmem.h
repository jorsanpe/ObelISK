/*
 * $FILE: memory.c
 *
 * ObelISK memory management
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

#ifndef _SYSMEM_H_
#define _SYSMEM_H_

#include <arch/cplb.h>

/*
 * Memory Map
 */
// MMRs
#define MAP_MMR_BASE       0xFFC00000
#define MAP_MMR_LENGTH     0x40000

// Scratchpad
#define L1_KSCRATCH_BASE    0xFFB00000
#define L1_KSCRATCH_LENGTH  0x800
#define L1_USCRATCH_BASE    0xFFB00800
#define L1_USCRATCH_LENGTH  0x800


// L1 Data A
#define L1_DATA_A_BASE     0xFF800000
#ifdef CONFIG_BFIN_DCACHE
#define L1_DATA_A_LENGTH   0x4000
#else
#define L1_DATA_A_LENGTH   0x8000
#endif

// L1 Data B
#define L1_DATA_B_BASE     0xFF900000
#ifdef CONFIG_BFIN_DCACHEB
#define L1_DATA_B_LENGTH   0x4000
#else
#define L1_DATA_B_LENGTH   0x8000
#endif

// L1 Code
#define L1_CODE_BASE       0xFFA00000
#ifdef CONFIG_BFIN_ICACHE
#define L1_CODE_LENGTH     0x8000
#else
#define L1_CODE_LENGTH     0x12000
#endif

// L2 SRAM
#define L2_BASE            0xFEB00000
#define L2_LENGTH          0x20000

// External Memory
#define RAM_BASE           0x00000000
#define RAM_LENGTH         ((CONFIG_RAM_SIZE)*1024*1024)
#define RAM_END            (RAM_BASE+RAM_LENGTH)

#define RAM_B0_K_BASE      0x00000000
#define RAM_B0_K_LENGTH    0x00400000
#define RAM_B0_U_BASE      0x00400000
#define RAM_B0_U_LENGTH    0x00C00000
#define RAM_B1_BASE        0x01000000
#define RAM_B1_LENGTH      0x01000000
#define RAM_B2_BASE        0x02000000
#define RAM_B2_LENGTH      0x01000000
#define RAM_B3_BASE        0x03000000
#define RAM_B3_LENGTH      0x01000000

// Async Memory Banks
#define ASYNC_BANK3_BASE   0x2C000000  // Async Bank 3
#define ASYNC_BANK3_LENGTH   0x04000000  // 64M
#define ASYNC_BANK2_BASE   0x28000000  // Async Bank 2
#define ASYNC_BANK2_LENGTH   0x04000000  // 64M
#define ASYNC_BANK1_BASE   0x24000000  // Async Bank 1
#define ASYNC_BANK1_LENGTH   0x04000000  // 64M
#define ASYNC_BANK0_BASE   0x20000000  // Async Bank 0
#define ASYNC_BANK0_LENGTH   0x04000000  // 64M

// Extended flags
#define MEM_REG_USER        0x80000000 // Region is user accesible
#define MEM_REG_DATA        0x40000000 // Region contains data
#define MEM_REG_CODE        0x20000000 // Region contains code
#define MEM_REG_CACHE       0x10000000 // Region is cacheable in L1
#define MEM_REG_PRIO        0x08000000 // Region has higher priority to be mapped on CPLB
#define MEM_REG_EFLAGS_MASK 0xF8000000
#define MEM_REG_PGSIZE_MASK 0x000F0000

// User main stack
#define USER_STACK 		((void*)(L1_USCRATCH_BASE))
#define USER_STACK_SIZE (L1_USCRATCH_LENGTH)

// Memory Page flags
#ifndef __ASSEMBLY__

struct mem_region {
   uint32_t base;
   uint32_t size;
   uint32_t flags;
};

struct mem_page {
   void *addr;
   uint32_t flags;
};

extern void cplb_init(void);

#endif

#endif //_SYSMEM_H_
