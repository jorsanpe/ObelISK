/*
 * $FILE: semaphore_example.c
 *
 * Semaphores example. Producer consumer example. Task 1
 * waits until task 2 consumes all of its items. It then
 * produces a new item and signals the producer semaphore.
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
   SECOND_TASK
};

enum {
   SEM_PROD,
   SEM_CONS,
};

void task1(void)
{
   SYSTIM tcurr, tprev;
   int i;

   for (i=0; i<50; i++) {
      // Wait for the consumer to consume the item
      wai_sem(SEM_CONS);
      get_tim(&tcurr);
      tprev = tcurr;

      while ((tcurr-tprev) < 500) {
         get_tim(&tcurr);
      }
      bfin_write_PORTH(bfin_read_PORTH() | PH5);

      // Produce new item
      sig_sem(SEM_PROD);

      tprev = tcurr;
   }
   ext_tsk();
}

void task2(void)
{
   int i;
   SYSTIM tcurr, tprev;

   for (i=0; i<50; i++) {

      // Wait for a new item
      wai_sem(SEM_PROD);
      get_tim(&tcurr);
      tprev = tcurr;

      while ((tcurr-tprev) < 500) {
         get_tim(&tcurr);
      }
      bfin_write_PORTH(bfin_read_PORTH() & ~PH5);

      // Consume new item
      sig_sem(SEM_CONS);

      tprev = tcurr;
   }
   ext_tsk();
}

int main(void)
{
   T_CTSK *pk_ctsk;
   T_CSEM *pk_csem;

   bfin_write_PORTH_DIR_SET(PH5);

   // Task 1 attributes
   get_mpl(0, sizeof(T_CTSK), &pk_ctsk);
   pk_ctsk->task = task1;
   pk_ctsk->itskpri = 5;
   pk_ctsk->stksz = 512;
   cre_tsk(FIRST_TASK, pk_ctsk);

   // Task 2 attributes
   pk_ctsk->task = task2;
   pk_ctsk->itskpri = 4;
   pk_ctsk->stksz = 512;
   cre_tsk(SECOND_TASK, pk_ctsk);

   // Producer semaphore attributes
   get_mpl(0, sizeof(T_CSEM), &pk_csem);
   pk_csem->sematr = TA_TFIFO;
   pk_csem->isemcnt = 0;
   pk_csem->maxsem = 1;
   cre_sem(SEM_PROD, pk_csem);

   pk_csem->sematr = TA_TFIFO;
   pk_csem->isemcnt = 1;
   pk_csem->maxsem = 1;
   cre_sem(SEM_CONS, pk_csem);

   act_tsk(FIRST_TASK);
   act_tsk(SECOND_TASK);

   rel_mpl(0, pk_ctsk);
   rel_mpl(0, pk_csem);

   ext_tsk();
}
