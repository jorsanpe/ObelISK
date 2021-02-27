/*
 * $FILE: sched.h
 *
 * Scheduler header file
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

#ifndef _SCHED_H_
#define _SCHED_H_

#ifndef _KERNEL_
#error   Internal file. Do not include it in your sources.
#endif

// Thread types (User/Kernel)
#define TSK_NOARG    0  /* No parameter is passed to the task function */
#define TSK_VPARG    1  /* A void pointer parameter is passed as an argument to */
#define TSK_KERNEL   2  /* Kernel task */

#ifndef __ASSEMBLY__

#include <autoconf.h>
#include <bitmap.h>
#include <queue.h>
#include <task.h>
#include <heap.h>

#define  PRIORITY_LEVELS      32
#define  MIN_TASK_PRIORITY    (PRIORITY_LEVELS-1)
#define  MAX_TASK_PRIORITY    0
#define  IDLE_TASK_ID         (CONFIG_MAX_TASKS)

typedef struct ready_queue_struct {
   queue_t prioqueue[PRIORITY_LEVELS];
   bitmap_t priobmap;
} ready_queue_t;

extern task_t *current_task;
extern task_t *idle_task;

extern void rq_init(void);
extern void rq_insert(task_t *tsk);
extern void rq_remove(task_t *tsk);
extern task_t *rq_get_head(void);

extern void tq_insert(task_t *tsk, uint32_t tmout);
extern void tq_wakeup(void);
extern void tq_remove(task_t *tsk);

extern void init_sched(void);
extern void dispatcher(void);
extern void scheduler(void);
extern void create_idle_task(void);
extern void init_task_pool(void);

#endif //__ASSEMBLY__

#endif   //_SCHED_H_
