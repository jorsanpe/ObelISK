/*
 * $FILE: cache_force_example.c
 *
 * (NON FUNCTIONAL) This example measures the benefits
 * of having cached and non cached memory. For this example to work,
 * the kernel has been modified to have the first 8 MB of external
 * memory uncached. The buffers have been linked manually on the
 * Linker Description Script
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

#include <itron.h>
#include <arch/clock.h>
#include <string.h>

cycles_t start, end;
nanoseconds_t cached_time, uncached_time;

__attribute__((section(".cached"))) int cached_buffer[1024];
__attribute__((section(".uncached"))) int uncached_buffer[1024];

static unsigned long next = 1;
/* RAND_MAX assumed to be 32767 */
int myrand(void) {
    next = next * 1103515245 + 12345;
    return((unsigned)(next/65536) % 32768);
}
void mysrand(unsigned seed) {
    next = seed;
}

int main(void)
{
   int i, k;
   nanoseconds_t stim, archstim;

   //memclr(cached_buffer, sizeof(cached_buffer));
   //memclr(uncached_buffer, sizeof(cached_buffer));

   start = get_cycles();
   memset(cached_buffer, 0, sizeof(cached_buffer));
   end = get_cycles();
   stim = cycles_to_ns(end - start);

   start = get_cycles();
   memset(uncached_buffer, 0, sizeof(uncached_buffer));
   end = get_cycles();
   archstim = cycles_to_ns(end - start);

   k = 0;
   start = get_cycles();
   for (i=0; i<1024*1024; i++) {
      cached_buffer[k] = ((cached_buffer[k] >> 4) + i);
      cached_buffer[k] = cached_buffer[k] * (myrand() % 10);
      cached_buffer[k] = cached_buffer[k] + (myrand() % 1024);
      k = (k + 1) % 1024;
   }
   end = get_cycles();
   cached_time = cycles_to_ns(end - start);

   k = 0;
   start = get_cycles();
   for (i=0; i<1024*1024; i++) {
      uncached_buffer[k] = ((uncached_buffer[k] >> 4) + i);
      uncached_buffer[k] = uncached_buffer[k] * (myrand() % 10);
      uncached_buffer[k] = uncached_buffer[k] + (myrand() % 1024);
      k = (k + 1) % 1024;
   }
   end = get_cycles();
   uncached_time = cycles_to_ns(end - start);

   return 0;
}


