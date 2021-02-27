/*
 * $FILE: time.h
 *
 * Kernel Time
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

#ifndef _TIME_H_
#define _TIME_H_

#include <arch/clock.h>
#include <itron.h>

#ifdef _KERNEL_
SYSTIM get_system_time(void); // Internal function for kernel timing
#endif

static inline unsigned int ticks_to_ms(SYSTIM ticks)
{
   return (ticks*CONFIG_CTMR)/1000;
}

extern cycles_t tot_idle;  // diagnostic variable

#endif
