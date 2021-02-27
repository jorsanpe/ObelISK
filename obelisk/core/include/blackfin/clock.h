/*
 * $FILE: clock.h
 *
 * Blackfin high resolution time measurement
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
#ifndef _CLOCK_H_
#define _CLOCK_H_

#ifndef AUTOCONF_INCLUDED
#include <autoconf.h>
#endif

#define NSEC_DIVIDER           (1000000000)

typedef unsigned long long cycles_t;
typedef unsigned long long nanoseconds_t;

/*
 * get_cycles()
 */
static inline cycles_t get_cycles(void)
{
   unsigned long tmp, tmp2;
   __asm__ __volatile__("%0 = cycles; %1 = cycles2;" : "=d"(tmp), "=d"(tmp2));
   return tmp | ((cycles_t)tmp2 << 32);
}

/*
 * cycles_2_ns()
 */
static inline nanoseconds_t cycles_to_ns(cycles_t cyc)
{
   return cyc * (NSEC_DIVIDER / CONFIG_CCLK);
}

/*
 * usleep()
 */
static inline void usleep(unsigned int usec)
{
   cycles_t c1, c2;
   nanoseconds_t stime = usec * 1000;

   c2 = c1 = get_cycles();
   while (cycles_to_ns(c2 - c1) < stime) {
      c2 = get_cycles();
   }
}

#endif   //_CLOCK_H_

