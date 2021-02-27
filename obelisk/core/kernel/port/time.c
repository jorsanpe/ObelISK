/*
 * $FILE: time.c
 *
 * Kernel time management
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
#include <stddef.h>
#include <time.h>
#include <arch/timer.h>

#define CPU_USAGE_TIME_SLOT   500

static SYSTIM system_time;
cycles_t tot_idle;
float cpu_usage;

/*
 * get_system_time()
 */
SYSTIM get_system_time(void)
{
   return system_time;
}

/*
 * evt_tmr_isr():
 *
 *    Core periodic timer interrupt service routine. By the time
 *    system_time restarts its count we will live on Mars
 */
void evt_tmr_isr(void)
{
   // Increase system time
   system_time = system_time + 1;

   // Measure CPU usage for this period
   if (system_time == CPU_USAGE_TIME_SLOT) {
      //cpu_usage = (float)CPU_USAGE_TIME_SLOT / (cycles_to_ns(tot_idle) / 1000000.0f);
      tot_idle = 0;
   }

   // Check the sleep queue
   tq_wakeup();

   // TODO: Decrement task compute remaining time
}

/*
 *  get_tim_sys()
 *
 *    Get System Clock
 */
ER get_tim_sys(SYSTIM *pk_tim)
{
   // Check invalid pk_tim pointer
   if (!pk_tim) {
      return E_PAR;
   }

   // Check invalid pk_tim pointer alignment
   if (((uint32_t) pk_tim) & 0x3) {
      return E_PAR;
   }

   // If SYSTIM is 64-bit wide, then we have to protect
   // the assignment to ensure coherence on the result
   __hw_cli();

   *pk_tim = system_time;

   __hw_sti();

   return E_OK;
}

/*
 * init_time()
 */
__init void init_time(void)
{
   system_time = 0;
   init_core_timer();
}
