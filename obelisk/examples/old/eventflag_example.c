/*
 * $FILE: flag_example.c
 *
 * Eventflag example. Task 2 waits eventflag 0 to have a
 * certain pattern. Task 1 sets this pattern each 500 ms.
 * The activation of the TA_CLR attribute causes the eventflag
 * to be cleared once task 2 resumes so it gets blocked
 * on the following wait.
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

extern duration_t system_time;

enum {
   MAIN_TASK=0,
   FIRST_TASK,
   SECOND_TASK,
   THIRD_TASK,
};

enum {
   EVENTFLAG=0
};

enum {
   PATTERNA=0x01,
   PATTERNB=0x02,
};

void task1(void)
{
   int i;
   SYSTIM tcurr, tprev;

   for (i=0; i<50; i++) {
      tslp_tsk(50);
      set_flg(EVENTFLAG, PATTERNA);
   }
   ext_tsk();
}

void task3(void)
{
   int i;
   SYSTIM tcurr, tprev;

   for (i=0; i<50; i++) {
      tslp_tsk(50);
      set_flg(EVENTFLAG, PATTERNB);
   }
   ext_tsk();
}

void task2(void)
{
   int i;
   SYSTIM tcurr, tprev;
   FLGPTN flg;

   for (i=0; i<50; i++) {
      wai_flg(EVENTFLAG, PATTERNA | PATTERNB, TWF_ANDW, &flg);
      bfin_write_PORTH(bfin_read_PORTH() ^ PH5);
   }
   ext_tsk();
}

int main(void)
{
   T_CTSK *pk_ctsk;
   T_CFLG *pk_cflg;

   bfin_write_PORTH_DIR_SET(PH5);

   // Task 1 attributes
   get_mpl(0, sizeof(T_CTSK), &pk_ctsk);
   pk_ctsk->task = task1;
   pk_ctsk->itskpri = 4;
   pk_ctsk->stksz = 512;
   cre_tsk(FIRST_TASK, pk_ctsk);

   // Task 2 attributes
   pk_ctsk->task = task2;
   pk_ctsk->itskpri = 5;
   pk_ctsk->stksz = 512;
   cre_tsk(SECOND_TASK, pk_ctsk);

   // Task 3 attributes
   pk_ctsk->task = task3;
   pk_ctsk->itskpri = 4;
   pk_ctsk->stksz = 512;
   cre_tsk(THIRD_TASK, pk_ctsk);

   // Eventflag attributes
   get_mpl(0, sizeof(T_CFLG), &pk_cflg);
   pk_cflg->flgatr = TA_CLR;
   pk_cflg->iflgptn = 0;
   cre_flg(EVENTFLAG, pk_cflg);

   act_tsk(FIRST_TASK);
   act_tsk(SECOND_TASK);
   act_tsk(THIRD_TASK);

   rel_mpl(0, pk_ctsk);
   rel_mpl(0, pk_cflg);

   ext_tsk();
}
