/*
 * $FILE: ints.h
 *
 * Blackfin interrupt handling
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

#ifndef _INTS_H_
#define _INTS_H_

#ifndef _KERNEL_
#error  Internal file. Do not include it in your sources.
#endif

#ifndef __ASSEMBLY__

#include <arch/processor.h>
#include <types.h>

#define __hw_cli()  __asm__ __volatile__("cli %0;\n\t": "=d" (interrupt_flags))
#define __hw_sti() __asm__ __volatile__("sti %0;\n\t": : "d" (interrupt_flags))
#define mask_interrupt(inhno) bfin_write_IMASK(bfin_read_IMASK() | MK_BMSK_(inhno))
#define unmask_interrupt(inhno) bfin_write_IMASK(bfin_read_IMASK() | ~MK_BMSK_(inhno))

// Interrupt management
extern uint32_t interrupt_flags;
extern uint32_t int_nest_count;
extern uint32_t need_resched;
extern void *interrupt_table[7];

extern void install_handler(uint32_t intn, void *phdr);
extern void common_interrupt_body(void);

#endif //__ASSEMBLY__
#endif
