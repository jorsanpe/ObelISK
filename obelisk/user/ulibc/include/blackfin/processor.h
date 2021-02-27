/*
 * $FILE: processor.h
 *
 * Processor definitions
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

#ifndef _ARCH_PROCESSOR_H_
#define _ARCH_PROCESSOR_H_

#ifndef __ASSEMBLY__

#include <kernel_inc/arch/arch_types.h>
#include <arch/cdefBF548.h>

#define IDLE()       __asm__ __volatile__("idle;")
#define SSYNC()      __asm__ __volatile__("ssync;")
#define CSYNC()      __asm__ __volatile__("csync;")
#define ONES(V)      ({uint32_t val; \
                     __asm__ __volatile__("%0 = ONES %1;" : "=r"(val) : "r"(V)) \
                     val;})

extern uint32_t rea8_mmr(uint32_t mmr, uint8_t *p_val);
extern uint32_t wri8_mmr(uint32_t mmr, uint8_t val);
extern uint32_t rea16_mmr(uint32_t mmr, uint16_t *p_val);
extern uint32_t wri16_mmr(uint32_t mmr, uint16_t val);
extern uint32_t rea32_mmr(uint32_t mmr, uint32_t *p_val);
extern uint32_t wri32_mmr(uint32_t mmr, uint32_t val);

#else    //__ASSEMBLY__

#include <arch/defBF548.h>

#define LO(con32) ((con32) & 0xFFFF)
#define lo(con32) ((con32) & 0xFFFF)
#define HI(con32) (((con32) >> 16) & 0xFFFF)
#define hi(con32) (((con32) >> 16) & 0xFFFF)

#define ssync(x) SSYNC(x)
#define csync(x) CSYNC(x)

#define SSYNC(scratch) SSYNC;
#define CSYNC(scratch) CSYNC;

#endif   //__ASSEMBLY__
#endif   //_ARCH_PROCESSOR_H_



