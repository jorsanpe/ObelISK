/*
 * $FILE: mailbox_example.c
 *
 * Mailbox example. Task 2 waits for a message on the mailbox
 * 0. Task 1 writes messages each 500 ms on the mailbox, thus
 * waking-up task 2.
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
unsigned int mailbox_data;

enum {
   MAIN_TASK=0,
   FIRST_TASK,
   SECOND_TASK
};

enum {
   MAILBOX=0
};

enum {
   SYSTEM_POOL,
   USER_POOL1
};

void task1(void)
{
   int i;
   T_MSG *msg;

   // Allocate shared memory for the mailbox
   get_mpl(SYSTEM_POOL, sizeof(T_MSG), (void *)&msg);
   msg->msgdat = &mailbox_data;

   for (i=0; i<50; i++) {
      tslp_tsk(500);
      snd_mbx(MAILBOX, msg);
   }

   rel_mpl(SYSTEM_POOL, msg);
   ext_tsk();
}

void task2(void)
{
   int i;
   T_MSG *msg;

   // Allocate shared memory for the mailbox
   get_mpl(SYSTEM_POOL, sizeof(T_MSG), (void *)&msg);

   for (i=0; i<50; i++) {
      rcv_mbx(MAILBOX, &msg);
      bfin_write_PORTH(bfin_read_PORTH() ^ PH5);
   }

   rel_mpl(SYSTEM_POOL, msg);
   ext_tsk();
}

int main(void)
{
   T_CTSK *pk_ctsk;
   T_CMBX *pk_cmbx;

   bfin_write_PORTH_DIR_SET(PH5);

   // Task 1 attributes
   get_mpl(SYSTEM_POOL, sizeof(T_CTSK), (void *)&pk_ctsk);
   pk_ctsk->task = task1;
   pk_ctsk->itskpri = 4;
   pk_ctsk->stksz = 512;
   cre_tsk(FIRST_TASK, pk_ctsk);

   // Task 2 attributes
   pk_ctsk->task = task2;
   pk_ctsk->itskpri = 5;
   pk_ctsk->stksz = 512;
   cre_tsk(SECOND_TASK, pk_ctsk);

   // Mailbox attributes
   get_mpl(SYSTEM_POOL, sizeof(T_CFLG), (void *)&pk_cmbx);
   cre_mbx(MAILBOX, pk_cmbx);

   // Activate tasks
   act_tsk(FIRST_TASK);
   act_tsk(SECOND_TASK);

   // Release memory
   rel_mpl(SYSTEM_POOL, pk_ctsk);
   rel_mpl(SYSTEM_POOL, pk_cmbx);

   ext_tsk();
}


