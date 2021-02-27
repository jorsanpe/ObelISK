/*
 * $FILE: sysmem.h
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

#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#ifndef _KERNEL_
#error   Internal file. Do not include it in your sources.
#endif

#include <queue.h>
#include <stddef.h>

#define CONFIG_MEM_POOLS   4
#define ALIGN32(X) (((X) + 0x3) & ~0x3)

// Memory pool table struct for TLSF
typedef struct vmpool_struct {
   /* Size of the memory pool */
   uint32_t length;

   /* Base address */
   void *base;

   /* ID of the memory pool */
   int32_t id;
} mplpool_t;

// Fixed memory pool struct for ITRON
typedef struct fmpool_struct {
   /* Fixed-size memory pool atribute */
   uint32_t mpfatr;

   /* Total number of memory blocks */
   uint32_t blkcnt;

   /* Memory block size (bytes) */
   uint32_t blksz;

   /* Start address of the memory pool*/
   void *mpf;

   /* Task waiting queue */
   queue_t mpfque;
} mpfpool_t;

extern unsigned long _userpool1_base;
extern unsigned long _userpool1_size;
extern unsigned long _userpool2_base;
extern unsigned long _userpool2_size;
extern unsigned long _userpool3_base;
extern unsigned long _userpool3_size;
extern unsigned long _userpool4_base;
extern unsigned long _userpool4_size;

extern void *system_pool;
extern void *kfast_pool;
extern unsigned long _user_mem_start;
extern unsigned long _physical_mem_end;

extern void init_mem(void);

#endif   //_MEMPOOL_H_
