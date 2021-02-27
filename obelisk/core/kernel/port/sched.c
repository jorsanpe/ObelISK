/*
 * $FILE: sched.c
 *
 * Scheduler
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
#include <linkage.h>
#include <sched.h>
#include <string.h>
#include <task.h>
#include <time.h>
#include <arch/sysmem.h>

// Scheduler related variables
__kobject static ready_queue_t ready_queue;
//__kobjects static heap_t *timeout_heap;
__kobject static linked_list_t *timeout_queue;
__kobject task_t *current_task=NULL;
__kobject task_t *idle_task;
__kobject static int dispatching;
__kobject static cycles_t start_idle, end_idle;

#ifdef CONFIG_SCHED_EDF
static heap_t *ready_queue;
#endif

/*
 * rq_init()
 */
void rq_init(void)
{
   int i;
   for (i=0; i<PRIORITY_LEVELS; i++) {
      queue_init(&ready_queue.prioqueue[i]);
   }
   ready_queue.priobmap = clearbitmap ();

#ifdef CONFIG_SCHED_EDF
   ready_queue = heap_create(CONFIG_MAX_TASKS);
#endif
}

/*
 * rq_insert()
 */
void rq_insert(task_t *tsk)
{
   queue_insert(&ready_queue.prioqueue[tsk->tskpri], &tsk->rdyque);
   ready_queue.priobmap = setbit (ready_queue.priobmap, tsk->tskpri);

//#ifdef CONFIG_SCHED_EDF
   //heap_insert(ready_queue, tsk, tmout + get_system_time());
//#endif

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_RQ_INSERT, tsk->tskid, tsk->tskstat, 0, 0);
#endif
}

/*
 * rq_get_head()
 */
task_t *rq_get_head(void)
{
   int i;

   if (!ready_queue.priobmap) {
      return NULL;
   }

   i = _ffs(ready_queue.priobmap);
   return ready_queue.prioqueue[i].head->this;
}

/*
 * rq_remove()
 */
void rq_remove(task_t *tsk)
{
   int i = tsk->tskpri;
   queue_t *sq = &ready_queue.prioqueue[i];

   queue_remove(sq, &tsk->rdyque);

   if (!sq->head) {
      ready_queue.priobmap = clearbit(ready_queue.priobmap, i);
   }

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_RQ_REMOVE, tsk->tskid, tsk->tskstat, 0, 0);
#endif
}

/*
 * rq_relocate()
 */
void rq_relocate(task_t *tsk, int32_t newpri)
{
   int i = tsk->tskpri;
   queue_t *sq = &ready_queue.prioqueue[i];

   // Extract the task from the ready queue
   queue_remove(sq, &tsk->rdyque);

   if (!sq->head) {
      ready_queue.priobmap = clearbit(ready_queue.priobmap, i);
   }
   tsk->tskpri = newpri;

   // Insert with its new priority
   queue_insert(&ready_queue.prioqueue[newpri], &tsk->rdyque);
   ready_queue.priobmap = setbit (ready_queue.priobmap, newpri);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_RQ_RELOCATE, tsk->tskid, newpri, 0, 0);
#endif
}

/*
 * tq_insert()
 *
 *    Insert task into the timeout queue
 */
void tq_insert(task_t *tsk, uint32_t tmout)
{
   task_t *t;
   linked_list_t *l, *s;

   tsk->wuptim = tmout + get_system_time();

   s = &tsk->slpque;

   // Case 1: Empty
   if (!timeout_queue) {
      s->next = NULL;
      s->prev = NULL;
      timeout_queue = s;
      return;
   }

   // Case 2: Head
   t = timeout_queue->this;
   if (tsk->wuptim <= t->wuptim) {
      s->next = timeout_queue;
      s->prev = NULL;
      timeout_queue->prev = s;
      timeout_queue = s;
      return;
   }

   // Case 3: Body
   for (l=timeout_queue->next; l!=NULL; l=l->next) {
      t = l->this;
      if (tsk->wuptim <= t->wuptim) {
         s->next = l;
         s->prev = l->prev;
         l->prev->next = s;
         l->prev = s;
         return;
      }
   }

   // Case 4: Tail (t holds the last task on the queue)
   s->next = NULL;
   s->prev = &t->slpque;
   t->slpque.next = s;
}

/*
 * tq_wakeup()
 *
 *    Wakeup tasks whose timeout has expired
 */
void tq_wakeup(void)
{
   task_t *tsk;

   while (timeout_queue) {
      tsk = timeout_queue->this;
      if (tsk->wuptim <= get_system_time()) {
         remove_linked_list (&timeout_queue, &tsk->slpque);
         rq_insert(tsk);
         tsk->wobjid = E_TMOUT;
      } else {
         return;
      }
   }
}

/*
 * tq_remove()
 *
 *    Remove from timeout queue
 */
void tq_remove(task_t *tsk)
{
   remove_linked_list (&timeout_queue, &tsk->slpque);
}

/*
 * th_insert()
 *
 *    Insert task into the timeout heap
 */
/*
void th_insert(task_t *tsk, uint32_t tmout)
{
   tsk->wuptim = tmout + get_system_time();
   heap_insert(timeout_heap, tsk, tsk->wuptim);
}
*/
/*
 * th_wakeup()
 *
 *    Wakeup tasks whose timeout has expired
 *    TODO: The funciton cost could be very high if there 
 *    are several functions whose timeout has expired at once
 */
/*
void th_wakeup(void)
{
   task_t *tsk;
   uint32_t wuptim;

   // Look for tasks whose timeout has expired
   while ((wuptim=heap_getmin(timeout_heap)) == get_system_time()) {
      // Wake up task
      tsk = heap_extract(timeout_heap);
      rq_insert(tsk);
      tsk->wobjid = E_TMOUT;
   }
}
*/
/*
 * scheduler()
 *
 * WCET: Constant
 */
void scheduler(void)
{
   task_t *new_tsk;

   // If the nesting count is greater than one, we are calling
   // the scheduler from an interrupt handler. Avoid context switch
   // but register if rescheduling is needed
   if (int_nest_count > 0) {
      need_resched = 1;
      return;
   }

   __hw_cli();

   // Dispatching disabled.
   if (!dispatching) {
      __hw_sti();
      return;
   }

   // Get the highest priority task
   new_tsk = rq_get_head();

   // Idle if there are not ready tasks
   if (!new_tsk) {
      new_tsk = idle_task;
      start_idle = get_cycles();
   }

   // There is a new task with higher priority. Perform context switch
   if (new_tsk != current_task) {

      // If we were on the idle task, compute time spent
      // for CPU usage purposes
      if (current_task == idle_task) {
         end_idle = get_cycles();
         tot_idle += end_idle - start_idle;
      }

      // If task has not changed its state, then it has been preempted
      // by a higher priority task and its new state is TTS_RDY
      if (current_task->tskstat == TTS_RUN) {
         current_task->tskstat = TTS_RDY;
      }
      // Set current task running state
      new_tsk->tskstat = TTS_RUN;

      // TODO: Stack Resource Policy is applicable here
      /*if (SRP_Enabled) {
      }*/

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_SCHEDULER, current_task->tskid, new_tsk->tskid);
#endif

      // At this point, we are going to perform a context switch so
      // there's no need to re-schedule
      need_resched = 0;

      // Context switch
      context_switch(&current_task, new_tsk);
   }

   // At this point, current_task and new_tsk are the same
   __hw_sti();
}

/*
 * dis_dsp_sys()
 *
 *    Disable dispatching
 *
 *    WCET: Constant
 */
ER dis_dsp_sys()
{
   __hw_cli();
   dispatching = FALSE;
   __hw_sti();

   return E_OK;
}

/*
 * ena_dsp_sys()
 *
 *    Enable dispatching
 *
 *    WCET: Constant
 */
ER ena_dsp_sys()
{
   __hw_cli();
   dispatching = TRUE;
   __hw_sti();

   return E_OK;
}

/*
 * sns_dsp_sys()
 *
 *    Reference Dispatching Disabled State
 */
BOOL sns_dsp_sys()
{
   return dispatching == FALSE;
}

/*
 * init_sched()
 */
__init void init_sched(void) {
   memset(&ready_queue, 0, sizeof(ready_queue_t));
   //timeout_heap = heap_create(CONFIG_MAX_TASKS);
   timeout_queue = NULL;
   dispatching = FALSE;
}
