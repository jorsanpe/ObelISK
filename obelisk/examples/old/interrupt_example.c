/*
 * $FILE: interrupt_example.c
 *
 * Interrupt handling example.
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

#include "itron.h"
#include "processor.h"
#include "clock.h"

extern duration_t system_time;

static void led_handler(void)
{
   bfin_write_TIMER_STATUS0(bfin_read_TIMER_STATUS0() | TIMIL0);
   bfin_write_PORTH(bfin_read_PORTH() ^ PH5);
}

int main(void)
{
   T_DINH *pk_dinh;
   SYSTIM tcurr, tprev;
   cycles_t start, end;

   bfin_write_PORTH_DIR_SET(PH5 | PH6);
   bfin_write_PORTH(~PH5 & ~PH6);

   // Task 1 attributes
   get_mpl(0, sizeof(T_DINH), (VP *)&pk_dinh);
   pk_dinh->inthdr = led_handler;
   pk_dinh->inhatr = 0;
   def_inh(11, pk_dinh);
   rel_mpl(0, pk_dinh);

   // Timer values
   bfin_write_TIMER0_CONFIG(IRQ_ENA | PERIOD_CNT | OUT_DIS | PWM_OUT);
   bfin_write_TIMER0_WIDTH(1);
   bfin_write_TIMER0_PERIOD(62500000);
   bfin_write_SIC_IMASK2(bfin_read_SIC_IMASK2() | (1<<22));
   bfin_write_TIMER_ENABLE0(bfin_read_TIMER_ENABLE0() | TIMEN0);
   SSYNC();

   get_tim(&tcurr);
   tprev = tcurr;
   while (1) {
      while ((tcurr-tprev) < 500) {
         get_tim(&tcurr);
      }
      tprev = tcurr;

      //tslp_tsk(500);
      bfin_write_PORTH(bfin_read_PORTH() ^ PH6);
   }

   ext_tsk();
}
