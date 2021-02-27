/*
 * $FILE: timer.h
 *
 * Timer header
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
#ifndef _TIMER_H_
#define _TIMER_H_

#ifndef _KERNEL_
#error   Internal file. Do not include it in your sources.
#endif

#include <arch/arch_types.h>

#define ENABLE_DF                 0x0

extern duration_t read_sys_time(void);
extern void init_clocks(void);
extern void evt_tmr_isr(void);
extern void init_core_timer(void);

#endif   //_TIMER_H_
