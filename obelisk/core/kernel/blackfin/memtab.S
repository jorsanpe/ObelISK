/*
 * $FILE: memtab.S
 *
 * ObelISK memory region table. Flags for the memory region table
 * are a combination of the Blackfin xCPLB_DATAx Register flags and
 * extended flags to describe region properties
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

#include <autoconf.h>
#include <processor.h>
#include <arch/sysmem.h>

.macro DEFREG _BASE, _LENGTH, _FLAGS
.long \_BASE
.long \_LENGTH
.long \_FLAGS
.endm

#define DEFINE_REGION(BASE, LENGTH, FLAGS) DEFREG BASE, LENGTH, FLAGS

/*
 * IMPORTANT NOTE: The memory regions defined here must be ordered, from the
 * lowest addresses to the highest ones. Should this order be altered, the system
 * would have unexpected behaviour on the memory exception handler, possibly
 * causing a fatal error. Please, have a look at the code on "core/kernel/blackfin/sysmem.c"
 */
.section .memtab, "wa"
DEFINE_REGION(RAM_B0_K_BASE,
              RAM_B0_K_LENGTH,
              PAGE_SIZE_4MB | CPLB_LOCK | CPLB_MEM_LEV | MEM_REG_DATA | MEM_REG_CODE);

DEFINE_REGION(RAM_B0_U_BASE,
              RAM_END-RAM_B0_U_BASE,
              PAGE_SIZE_4MB | CPLB_MEM_LEV | MEM_REG_USER | MEM_REG_DATA | MEM_REG_CODE);

DEFINE_REGION(ASYNC_BANK0_BASE,
              ASYNC_BANK0_LENGTH,
              PAGE_SIZE_4MB | CPLB_MEM_LEV | MEM_REG_DATA | MEM_REG_CODE);

DEFINE_REGION(ASYNC_BANK1_BASE,
              ASYNC_BANK1_LENGTH,
              PAGE_SIZE_4MB | CPLB_MEM_LEV | MEM_REG_DATA);

DEFINE_REGION(ASYNC_BANK2_BASE,
              ASYNC_BANK2_LENGTH,
              PAGE_SIZE_4MB | CPLB_MEM_LEV | MEM_REG_DATA);

DEFINE_REGION(ASYNC_BANK3_BASE,
              ASYNC_BANK3_LENGTH,
              PAGE_SIZE_4MB | CPLB_MEM_LEV | MEM_REG_DATA);

DEFINE_REGION(_sl2kdata,
              _l2kdatasz,
              PAGE_SIZE_4KB | CPLB_LOCK | MEM_REG_DATA | MEM_REG_CODE);

DEFINE_REGION(__kfastpool_base,
              __kfastpool_size,
              PAGE_SIZE_4KB | MEM_REG_PRIO | MEM_REG_DATA | MEM_REG_CODE);

DEFINE_REGION(_sl2udata,
              _l2udatasz,
              PAGE_SIZE_4KB | MEM_REG_USER | MEM_REG_DATA | MEM_REG_CODE);

DEFINE_REGION(_sl1kdata,
              _l1kdatasz,
              PAGE_SIZE_4KB | CPLB_LOCK | MEM_REG_DATA);

DEFINE_REGION(_sl1udata,
              _l1udatasz,
              PAGE_SIZE_4KB | MEM_REG_USER | MEM_REG_DATA);

DEFINE_REGION(L1_DATA_B_BASE,
              L1_DATA_B_LENGTH,
              PAGE_SIZE_4KB | MEM_REG_USER | MEM_REG_DATA);

DEFINE_REGION(_sl1kcode,
              _l1kcodesz,
              PAGE_SIZE_4KB | CPLB_LOCK | MEM_REG_CODE);

DEFINE_REGION(_sl1ucode,
              _l1ucodesz,
              PAGE_SIZE_4KB | MEM_REG_USER | MEM_REG_CODE);

DEFINE_REGION(L1_KSCRATCH_BASE,
              L1_KSCRATCH_LENGTH,
              PAGE_SIZE_1KB | MEM_REG_DATA);

DEFINE_REGION(L1_USCRATCH_BASE,
              L1_USCRATCH_LENGTH,
              PAGE_SIZE_1KB | MEM_REG_USER | MEM_REG_DATA);

DEFINE_REGION(MAP_MMR_BASE,
              MAP_MMR_LENGTH,
              PAGE_SIZE_1MB | CPLB_LOCK | MEM_REG_DATA);

DEFINE_REGION(0xFFFFFFFF, 0, 0);
.previous
