/*
 * $FILE: task_example.c
 *
 * A simple task creation example.
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
#include <arch/cdefBF548.h>

extern duration_t system_time;

enum {
   MAIN_TASK=0,
   FIRST_TASK,
   SECOND_TASK
};

void task1(void)
{
   int i;

   bfin_write_PORTH_DIR_SET(PH5);

   for (i=0; i<50; i++) {
      tslp_tsk(500);
      bfin_write_PORTH(bfin_read_PORTH() ^ PH5);
   }
   ext_tsk();
}

int main(void)
{
   T_CTSK *pk_ctsk;

   get_mpl(0, sizeof(T_CTSK), &pk_ctsk);
   pk_ctsk->task = task1;
   pk_ctsk->itskpri = 5;
   pk_ctsk->stksz = 512;

   cre_tsk(FIRST_TASK, pk_ctsk);
   act_tsk(FIRST_TASK);

   ext_tsk();
}
