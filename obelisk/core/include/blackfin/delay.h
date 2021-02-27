/*
 * $FILE: delay.h
 *
 * High resolution delays
 *
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
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

#include <arch/clock.h>

/*
 * udelay()
 */
static inline void udelay(unsigned int usec) {
   cycles_t cyc1, cyc2;
   nanoseconds_t nsec = usec * 1000;

   cyc1 = cyc2 = get_cycles();

   while (cycles_to_ns(cyc2 - cyc1) < nsec) {
      cyc2 = get_cycles();
   }
}

#define mdelay(n)       udelay((n)*1000)
