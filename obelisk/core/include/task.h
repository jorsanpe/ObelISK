/*
 * $FILE: task.h
 *
 * Core task management
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

#ifndef _TASK_H_
#define _TASK_H_

#ifndef _KERNEL_
#error   Internal file. Do not include it in your sources.
#endif

#include <itron.h>
#include <linked_list.h>
#include <types.h>
#include <processor.h>

typedef struct task_struct {

   /* task id */
   int32_t     tskid;

   /* extended information */
   void        *exinf;

   /* task attributes */
   uint32_t    tskatr;

   /* base priority */
   int32_t     tskbpri;

   /* current priority */
   int32_t     tskpri;

   /* task state */
   uint32_t    tskstat;

   /* object ID number for which the task is waiting */
   /* used to notify of object deletions while waiting */
   int32_t     wobjid;

   /* cause of wait */
   uint32_t    tskwait;

   /* Wakeup time */
   uint64_t    wuptim;

   /* wait pattern for eventflags */
   uint32_t    wflgptn;

   /* wait mode for eventflags */
   uint32_t    wflgatr;

   /* activation request count */
   int32_t     actcnt;

   /* wakeup request count */
   int32_t     wupcnt;

   /* suspend request count */
   int32_t     suscnt;

   /* extended data that this task can receive */
   void        *extdat;

   /* task start address */
   void        (*task)(void);

   /* stack size */
   int32_t     stksz;

   /* task period (=deadline) */
   uint32_t    tskper;

   /* task compute time */
   uint32_t    tskcom;

   /* Task Stacks (User and Kernel) */
   void        *stk;
   void        *kstk;

   /* queues where this task can be inserted */
   linked_list_t rdyque, synque, slpque, actlst;

   //uint32_t kstack_area[CONFIG_KSTACK_SIZE];
} task_t;

extern task_t *get_task_by_id(int32_t tskid);

#endif   //_TASK_H_
