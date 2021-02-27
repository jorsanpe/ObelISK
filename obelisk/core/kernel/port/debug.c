/*
 * $FILE: debug.c
 *
 * ObelISK kernel debugging functions
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

#include <time.h>
#include <tlsf.h>

#ifdef DEBUG

#define EVT_TMR_LOG_SIZE   512

int *evt_tmr_log;
int evt_tmr_idx;
static cycles_t tmr_start;

void init_tmr_log(void) {
   evt_tmr_log = malloc_ex(EVT_TMR_LOG_SIZE*sizeof(int), system_pool);
}

void log_tmr_start(int cycl, int cych) {
   tmr_start = cycl | ((cycles_t)cych << 32);
}

void log_tmr_stop(int cycl, int cych) {
   cycles_t stop = cycl | ((cycles_t)cych << 32);

   evt_tmr_log[evt_tmr_idx] = (int) (stop - tmr_start);
   evt_tmr_idx = (evt_tmr_idx + 1) % EVT_TMR_LOG_SIZE;
}

#endif
