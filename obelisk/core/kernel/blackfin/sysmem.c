/*
 * $FILE: sysmem.c
 *
 * System memory management
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
#include <arch/sysmem.h>
#include <arch/cplb.h>

#define MAP_DCPLB(CPLB, ADDR, DATA) \
do { \
   bfin_write32(DCPLB_ADDR0 + (CPLB)*4, ADDR); \
   bfin_write32(DCPLB_DATA0 + (CPLB)*4, DATA); \
} while(0)

#define MAP_ICPLB(CPLB, ADDR, DATA) \
do { \
   bfin_write32(ICPLB_ADDR0 + (CPLB)*4, ADDR); \
   bfin_write32(ICPLB_DATA0 + (CPLB)*4, DATA); \
} while(0)

extern struct mem_region memtab[];
extern uint32_t memtabsz;
__dnocache static uint32_t page_size_lut[4] = {0x00000400, 0x00001000, 0x00100000, 0x00400000};
__dnocache static uint32_t first_unlocked_dcplb, first_unlocked_icplb, dcplb_idx, icplb_idx;

// CPLB virtual table
//#define CPLB_VTAB_CODE 0
//#define CPLB_VTAB_DATA 1
//__dnocache static struct cplb_entry current_cplb_map[2][16];
//__dnocache static struct mem_region *mem_reg_tab;
//__dnocache static uint32_t mem_tab_size;

__inocache static uint32_t dcplb_flags(uint32_t mflags)
{
   uint32_t flags = mflags;

   flags |= CPLB_SUPV_WR | CPLB_VALID;
   if (flags & MEM_REG_USER) {
      flags |= CPLB_USER_RD | CPLB_USER_WR;
   }
#ifdef CONFIG_BFIN_DCACHE
   flags |= CPLB_L1_CHBL | CPLB_WT;
#endif
}

__inocache static uint32_t icplb_flags(uint32_t mflags)
{
   uint32_t flags = mflags;

   flags |= CPLB_VALID;
   if (flags & MEM_REG_USER) {
      flags |= CPLB_USER_RD;
   }
#ifdef CONFIG_BFIN_CACHE
   flags |= CPLB_L1_CHBL;
#endif
}

// Binary search for the memory region to which the address belong
__inocache static struct mem_region *find_mem_reg(uint32_t addr)
{
   uint32_t l, u, c;
   struct mem_region *reg;

   l = 0;
   u = memtabsz-1;

   while (l <= u) {
      c = (u + l) >> 1;
      reg = &memtab[c];
      if (addr < reg->base) {
         u = c - 1;
      } else if (addr > (reg->base+reg->size)) {
         l = c + 1;
      } else {
         return reg;
      }
   }
   return NULL;
}

/*
 * mmu_init()
 *
 *    MMU initialization
 *
 *    Anomaly notes:
 *       05000287 - We implement workaround #2 - Change the DMEM_CONTROL
 *       register, so that the port preferences for DAG0 and DAG1 are set
 *       to port B
 */
__init void init_mmu(void)
{
   uint32_t addr, size, data;
   uint32_t icplb, dcplb;
   struct mem_region *entry;

   //mem_reg_tab = (struct mem_region *)&memtab;
   //mem_tab_size = (uint32_t)&memtabsz / sizeof(struct mem_region);

   // Map first the critical CPLBs
   icplb = dcplb = 0;
   for (entry=memtab; entry->base!=0xffffffff; entry++) {
      if (entry->flags & CPLB_LOCK) {
         size = page_size_lut[(entry->flags & MEM_REG_PGSIZE_MASK) >> 16];
         for (addr=entry->base; addr<entry->base+entry->size; addr+=size) {
            if (entry->flags & MEM_REG_DATA) {
               data = dcplb_flags(entry->flags);
               MAP_DCPLB(dcplb, addr, data & ~MEM_REG_EFLAGS_MASK);
               ++dcplb;
            }
            if (entry->flags & MEM_REG_CODE) {
               data = icplb_flags(entry->flags);
               MAP_ICPLB(icplb, addr, data & ~MEM_REG_EFLAGS_MASK);
               ++icplb;
            }
            // Panic! Locked CPLBs exceed CPLB registers capacity
            // We do not allow more than 14 locked CPLB entries
            if (dcplb > 14 || icplb > 14) {
               kernel_panic();
            }
         }
      }
   }

   // Save indexes for replacement policies
   dcplb_idx = first_unlocked_dcplb = dcplb;
   icplb_idx = first_unlocked_icplb = icplb;

   // Map first page of each region until we exhaust the CPLB register table
   for (entry=memtab; entry->base!=0xffffffff; entry++) {
      if (!(entry->flags & CPLB_LOCK)) {
         size = page_size_lut[(entry->flags & MEM_REG_PGSIZE_MASK) >> 16];
         addr = entry->base;
         if (dcplb < 16) {
            if (entry->flags & MEM_REG_DATA) {
               data = dcplb_flags(entry->flags);
               MAP_DCPLB(dcplb, addr, data & ~MEM_REG_EFLAGS_MASK);
               ++dcplb;
            }
         }
         if (icplb < 16) {
            if (entry->flags & MEM_REG_CODE) {
               data = icplb_flags(entry->flags);
               MAP_ICPLB(icplb, addr, data & ~MEM_REG_EFLAGS_MASK);
               ++icplb;
            }
         }
         // CPLB register table exhausted
         if (dcplb > 15 && icplb > 15) {
            break;
         }
      }
   }

#ifdef CONFIG_BFIN_ICACHE
   ctrl = bfin_read_IMEM_CONTROL();
   ctrl |= IMC | ENICPLB;
   CSYNC();
   bfin_write_IMEM_CONTROL(ctrl);
   SSYNC();
#endif

#ifdef CONFIG_BFIN_DCACHE
   ctrl = bfin_read_DMEM_CONTROL();
#ifdef CONFIG_BFIN_DCACHEB
   ctrl |= ACACHE_BCACHE;
#endif
   ctrl |= ACACHE_BSRAM;
   ctrl |= PORT_PREF0 | (ANOMALY_05000287 ? PORT_PREF1 : 0);
   CSYNC();
   bfin_write_DMEM_CONTROL(ctrl);
   SSYNC();
#endif
}

/*
 * dcplb_miss()
 *
 *    Handle access to data address which is not covered by a
 *    data CPLB entry
 */
__inocache int32_t dcplb_miss(uint32_t cpu)
{
   uint32_t addr = bfin_read_DCPLB_FAULT_ADDR();
   uint32_t status = bfin_read_DCPLB_STATUS();
   uint32_t data;
   struct mem_region *entry;

   // Memory address not valid
   if (!(entry = find_mem_reg(addr))) {
      // check if coming from kernel or user
      // if kernel -> kernel_panic(DATA_BAD_ADDRESS)
      // if  user  -> notice_task_error
      return -1;
   }

   // Instruction access to code-only region
   if (!(entry->flags & MEM_REG_DATA)) {
      // check if coming from kernel or user
      // if kernel -> kernel_panic(DATA_INVALID_ACCESS)
      // if  user  -> notice_task_error
      return -1;
   }

   disable_dcplb();

   // Round-robin replacement policy
   data = dcplb_flags(entry->flags);

   // Page base address
   addr &= ~(page_size_lut[(data & MEM_REG_PGSIZE_MASK) >> 16] - 1);

   // Write CPLB
   MAP_DCPLB(dcplb_idx, addr, data & ~MEM_REG_EFLAGS_MASK);

   dcplb_idx++;
   if (dcplb_idx >= MAX_CPLBS) {
      dcplb_idx = first_unlocked_dcplb;
   }

   enable_dcplb();

   return 0;
}

/*
 * icplb_miss()
 *
 *    Handle access to data address which is not covered by an
 *    instruction CPLB entry
 */
__inocache int32_t icplb_miss(uint32_t cpu)
{
   uint32_t addr = bfin_read_ICPLB_FAULT_ADDR();
   uint32_t status = bfin_read_ICPLB_STATUS();
   uint32_t data;
   struct mem_region *entry;

   // Memory address not valid
   if (!(entry = find_mem_reg(addr))) {
      // check if coming from kernel or user
      // if kernel -> kernel_panic(PANIC_CODE_BAD_ADDRESS)
      // if  user  -> notice_task_error
      return -1;
   }

   // Instruction access to data-only region
   if (!(entry->flags & MEM_REG_CODE)) {
      // check if coming from kernel or user
      // if kernel -> kernel_panic(PANIC_CODE_INVALID_ACCESS)
      // if  user  -> notice_task_error
      return -1;
   }

   disable_icplb();

   // Round-robin replacement policy
   data = icplb_flags(entry->flags);

   // Page base address
   addr &= ~(page_size_lut[(data & MEM_REG_PGSIZE_MASK) >> 16] - 1);

   // Write CPLB
   MAP_ICPLB(icplb_idx, addr, data & ~MEM_REG_EFLAGS_MASK);

   icplb_idx++;
   if (icplb_idx == MAX_CPLBS) {
      icplb_idx = first_unlocked_icplb;
   }

   enable_icplb();

   return 0;
}

uint32_t unhandled_exception()
{

}





