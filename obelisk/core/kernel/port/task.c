/*
 * $FILE: task.c
 *
 * ObelISK Task management functions
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
#include <string.h>
#include <sched.h>
#include <tlsf.h>
#include <task.h>
#include <arch/stack.h>
#include <arch/sysmem.h>

#define MAX_TASKS (CONFIG_MAX_TASKS+2)
// The task pool is defined statically
__kobject static bitmap_t taskbmap;
__kobject static task_t task_pool[MAX_TASKS+2];
//__kobjects static linked_list_t *active_tsk;

/*
 * create_main_task()
 *
 *    The main task is created with maximum priority. It should
 *    be used only for initialization purposes
 */
extern void main();
__init void create_main_task(void)
{
   task_t *main_task;
   int32_t tskid;

   // Allocate an ID for the main task
   tskid = _ffc(taskbmap);
   main_task = &task_pool[tskid];
   taskbmap = setbit(taskbmap, tskid);

   // Assign main task attributes
   main_task->tskpri      = MAX_TASK_PRIORITY;
   main_task->tskid       = tskid+1;
   main_task->task        = main;
   main_task->tskstat     = TTS_RDY;
   main_task->rdyque.this = main_task->synque.this = main_task->slpque.this = main_task;
   main_task->stk         = USER_STACK;
   main_task->stksz       = USER_STACK_SIZE;
   main_task->exinf       = NULL;

   init_stack(main_task);

   rq_insert(main_task);

#ifdef CONFIG_SYSLOG_ACTIVE
   main_task->actlst.this = main_task;
   //insert_linked_list(&active_tsk, &main_task->actlst);
#endif
}

/*
 * create_idle_task()
 *
 *    Idle task has the lowest priority (actually not necessary). It is
 *    executed only if no other task is ready so it is not inserted on
 *    the ready queue. The idle task can perform some tasks.
 */
__init void create_idle_task(void)
{
   int32_t tskid;

   // At this time the idle task uses a slot from the task pool
   // Allocate an ID for the idle task
   tskid = _ffc(taskbmap);
   idle_task = &task_pool[tskid];
   taskbmap = setbit(taskbmap, tskid);

   // Minimum priority (actually not needed)
   idle_task->tskpri = MIN_TASK_PRIORITY;
   idle_task->tskid = tskid+1;
   idle_task->exinf = (void *)TSK_KERNEL;
   current_task = idle_task;
}

/*
 * init_task_pool()
 *
 *    Task pool initialization
 */
__init void init_task_pool(void)
{
   taskbmap = clearbitmap();
   memset(task_pool, 0, sizeof(task_pool));
}

/*
 * get_task_by_id()
 *
 *    Returns the pointer to the task structure
 */
task_t *get_task_by_id(int32_t tskid)
{
   int32_t idx;

   if (tskid < 1 || tskid > MAX_TASKS) {
      return NULL;
   }
   idx = tskid - 1;
   if (!isbitset(taskbmap, idx)) {
      return NULL;
   }
   return &task_pool[idx];
}

/*
 * acre_tsk()
 *
 *    Task creation with automatic ID assignment
 *
 *    WCET: Constant
 */
ER acre_tsk_sys(T_CTSK *pk_ctsk)
{
   task_t *tsk;
   int32_t idx, tskid;

   // Check invalid pk_ctsk pointer
   if (!pk_ctsk) {
      return E_PAR;
   }

   // Check invalid pk_ctsk values
   if (!pk_ctsk->task ||
       pk_ctsk->stksz < 0 ||
       pk_ctsk->itskpri < 0 ||
       pk_ctsk->stk == NULL ||
       pk_ctsk->itskpri > MIN_TASK_PRIORITY) {
      return E_PAR;
   }

   __hw_cli();

   // Check if any ID is available
   idx = _ffc(taskbmap);
   tskid = idx + 1;

   // No task ID available
   if (tskid < 1 || tskid > MAX_TASKS) {
      __hw_sti();
      return E_NOID;
   }

   // Get the task from the pool
   tsk = &task_pool[idx];
   memset(tsk, 0, sizeof(task_t));

   // Assign task properties
   tsk->tskatr      = pk_ctsk->tskatr;
   tsk->exinf       = pk_ctsk->exinf;
   tsk->task        = pk_ctsk->task;
   tsk->extdat      = pk_ctsk->extdat;
   tsk->tskstat     = TTS_DMT;
   tsk->tskpri      = pk_ctsk->itskpri;
   tsk->rdyque.this = tsk->synque.this = tsk->slpque.this = tsk;
   tsk->tskid       = tskid;

   // Initialize stack
   tsk->stksz = pk_ctsk->stksz;
   tsk->stk = pk_ctsk->stk;
   init_stack(tsk);

   taskbmap = setbit(taskbmap, idx);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_ACRE_TSK, tskid, 0);
   tsk->actlst.this = tsk;
   //insert_linked_list(&active_tsk, &tsk->actlst);
#endif

   __hw_sti();

   return tskid;
}

/*
 * act_tsk_sys()
 *
 * WCET: Constant
 */
ER act_tsk_sys(ID tskid)
{
   task_t *tsk;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   __hw_cli();

   // Get task from the task pool
   if (tskid == TSK_SELF) {
      tsk = current_task;
   } else {
      // Check if the task exists
      if (!isbitset(taskbmap, idx)) {
         __hw_sti();
         return E_NOEXS;
      }
      tsk = &task_pool[idx];
   }

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_ACT_TSK, tskid, tsk->actcnt);
#endif

   // Activate task or increase activation request count
   if (tsk->tskstat == TTS_DMT) {
      rq_insert(tsk);
      tsk->tskstat = TTS_RDY;
   } else {
      tsk->actcnt++;
   }

   __hw_sti();

   scheduler();

   return E_OK;
}

/*
 * can_act_sys()
 *
 *    Cancel activation requests
 *
 *    WCET: Constant
 */
ER_UINT can_act_sys(ID tskid)
{
   task_t *tsk;
   ER actcnt;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   __hw_cli();

   // Get task from the task pool
   if (tskid == TSK_SELF) {
      tsk = current_task;
   } else {
      // Check if the task exists
      if (!isbitset(taskbmap, idx)) {
         __hw_sti();
         return E_NOEXS;
      }
      tsk = &task_pool[idx];
   }

   // Clear activation requests
   actcnt = tsk->actcnt;
   tsk->actcnt = 0;

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_CAN_ACT, tskid, actcnt);
#endif

   __hw_sti();

   return actcnt;
}

/*
 * can_wup_sys()
 *
 *    Cancel wakeup requests
 */
ER_UINT can_wup_sys(ID tskid)
{
   task_t *tsk;
   ER_UINT wupcnt;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   __hw_cli();

   // Get task from the task pool
   if (tskid == TSK_SELF) {
      tsk = current_task;
   } else {
      // Check if the task exists
      if (!isbitset(taskbmap, idx)) {
         __hw_sti();
         return E_NOEXS;
      }
      tsk = &task_pool[idx];
   }

   // Clear wakeup requests
   wupcnt = tsk->wupcnt;
   tsk->wupcnt = 0;

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_CAN_WUP, tskid, wupcnt);
#endif

   __hw_sti();

   return wupcnt;
}

/*
 * chg_pri_sys()
 *
 *    Change task priority
 *
 *    WCET: Constant
 */
ER chg_pri_sys(ID tskid, PRI tskpri)
{
   task_t *tsk;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   // Check invalid priority
   if (tskpri < 0 || tskpri > MIN_TASK_PRIORITY) {
      return E_PAR;
   }

   __hw_cli();

   // Get task from the task pool
   if (tskid == TSK_SELF) {
      tsk = current_task;
   } else {
      // Check if the task exists
      if (!isbitset(taskbmap, idx)) {
         __hw_sti();
         return E_NOEXS;
      }
      tsk = &task_pool[idx];
   }

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_CHG_PRI, tskid, tskpri);
#endif

   // If the task was ready change its location on the ready queue
   if (tsk->tskstat == TTS_RDY) {
      rq_relocate(tsk, tskpri);
      tsk->tskpri = tskpri;
   }

   __hw_sti();

   // Call the scheduler
   scheduler();

   return E_OK;
}

/*
 * cre_tsk_sys()
 *
 *    Create task
 *
 *    WCET: Constant
 */
ER cre_tsk_sys(ID tskid, T_CTSK *pk_ctsk)
{
   task_t *tsk;
   int32_t idx = tskid - 1;

   // Check invalid ID number
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   // Check invalid pk_ctsk pointer
   if (!pk_ctsk) {
      return E_PAR;
   }

   // Check invalid pk_ctsk values
   if (!pk_ctsk->task ||
       pk_ctsk->stksz < 0 ||
       pk_ctsk->itskpri < 0 ||
       pk_ctsk->itskpri > MIN_TASK_PRIORITY) {
      return E_PAR;
   }

   __hw_cli();

   // Check if ID is in use
   if (isbitset(taskbmap, idx)) {
      __hw_sti();
      return E_OBJ;
   }

   // Get the task from the pool
   tsk = &task_pool[idx];
   memset(tsk, 0, sizeof(task_t));

   // Assign task properties
   tsk->tskatr      = pk_ctsk->tskatr;
   tsk->exinf       = pk_ctsk->exinf;
   tsk->task        = pk_ctsk->task;
   tsk->extdat      = pk_ctsk->extdat;
   tsk->tskstat     = TTS_DMT;
   tsk->tskpri      = pk_ctsk->itskpri;
   tsk->rdyque.this = tsk->synque.this = tsk->slpque.this = tsk;
   tsk->tskid       = tskid;

   // Initialize stack
   tsk->stksz = pk_ctsk->stksz;
   tsk->stk = pk_ctsk->stk;
   init_stack(tsk);

   taskbmap = setbit(taskbmap, idx);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_CRE_TSK, tskid, E_OK);
   tsk->actlst.this = tsk;
   //insert_linked_list(&active_tsk, &tsk->actlst);
#endif

   __hw_sti();

   return E_OK;
}

/*
 * del_tsk_sys()
 *
 *    Delete task
 *
 *    WCET: Constant
 */
ER del_tsk_sys(ID tskid)
{
   task_t *tsk;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   __hw_cli();

   // Check if the task exists
   if (!isbitset(taskbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get task from the task pool
   tsk = &task_pool[idx];

   // Task is not in the DORMANT state
   if (tsk->tskstat != TTS_DMT) {
      __hw_sti();
      return E_OBJ;
   }

   taskbmap = clearbit(taskbmap, idx);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_DEL_TSK, tskid, E_OK);
#endif

   __hw_sti();

   return E_OK;
}

/*
 * ext_tsk_sys
 *
 *    Finish the execution of the task
 *
 *    WCET: Constant
*/
void ext_tsk_sys(void)
{
   __hw_cli();

   // Remove current tasks from the ready queue
   rq_remove(current_task);

   // Change task state
   current_task->tskstat = TTS_DMT;

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_EXT_TSK, current_task->tskid, 0);
#endif

   __hw_sti();

   // Call the scheduler (never returns)
   scheduler();
}

/*
 * exd_tsk_sys
 *
 *    Finish the execution of the task and
 *    release the ID used
 *
 *    WCET: Constant
*/
void exd_tsk_sys(void)
{
   __hw_cli();

   // Remove current tasks from the ready queue
   rq_remove(current_task);

   // Change task state
   current_task->tskstat = TTS_DMT;
   taskbmap = clearbit(taskbmap, current_task->tskid - 1);

   // Release stack
   //free_ex(current_task->stk);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_EXD_TSK, current_task->tskid, 0);
#endif

   __hw_sti();

   // Call the scheduler (never returns)
   scheduler();
}

/*
 * get_pri_sys()
 *
 *    Get task current priority
 *
 *    WCET: Constant
 */
ER get_pri_sys(ID tskid, PRI *p_tskpri)
{
   task_t *tsk;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   // Check return pointer
   if (!p_tskpri || (uint32_t)p_tskpri & 0x3) {
      return E_PAR;
   }

   __hw_cli();

   // Get task from the task pool
   if (tskid == TSK_SELF) {
      tsk = current_task;
   } else {
      // Check if the task exists
      if (!isbitset(taskbmap, idx)) {
         __hw_sti();
         return E_NOEXS;
      }
      tsk = &task_pool[idx];
   }

   // Task is in the DORMANT state
   if (tsk->tskstat == TTS_DMT) {
      __hw_sti();
      return E_OBJ;
   }

   *p_tskpri = tsk->tskpri;

   __hw_sti();

   return E_OK;
}

/*
 * slp_tsk_sys()
 *
 *    Sleep invoking task. Task is put into WAITING state
 *
 *    WCET: Constant
 */
ER slp_tsk_sys(void)
{
   __hw_cli();

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_SLP_TSK, current_task->tskid, 0);
#endif

   // Change task state to WAITING
   current_task->tskstat = TTS_WAI;
   current_task->tskwait = TTW_SLP;
   rq_remove(current_task);

   __hw_sti();

   // Call the scheduler
   scheduler();

   return E_OK;
}

/*
 * tslp_tsk_sys()
 *
 *    Sleep invoking task for tmout ticks
 *
 *    WCET: Constant
 */
ER tslp_tsk_sys(TMO tmout)
{
   int32_t tmo = tmout + get_system_time();

   // If tmo = TMO_FEVR tslp_tsk() acts the same as slp_tsk()
   if (tmout == TMO_FEVR) {
      return slp_tsk_sys();
   }

   // Wait by polling
   if (tmout == TMO_POL) {
      while (1) {
         if (current_task->wupcnt > 0)
            return E_OK;
      }
   }

   // Invalid timeout
   if (tmout < 1) {
      return E_PAR;
   }

   __hw_cli();

   // Insert into the timeout heap
   tq_insert(current_task, tmout);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_TSLP_TSK, current_task->tskid, tmout);
#endif

   // Change task state to WAITING
   current_task->tskstat = TTS_WAI;
   current_task->tskwait = TTW_SLP;
   rq_remove(current_task);

   __hw_sti();

   // Call the scheduler
   scheduler();

   return E_OK;

}

/*
 * wup_tsk_sys()
 *
 *    Wake up task
 */
ER wup_tsk_sys(ID tskid) {
   return E_OK;
}

/*
 * sus_tsk_sys()
 */
ER sus_tsk_sys(ID tskid)
{

}

/*
 * ref_tsk_sys()
 */
ER ref_tsk_sys(ID tskid, T_RTSK *pk_rtsk)
{
   task_t *tsk;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   // Check if a task is terminating itself
   if (!pk_rtsk == TSK_SELF) {
      return E_PAR;
   }

   __hw_cli();

   // Get task from the task pool
   if (tskid == TSK_SELF) {
      tsk = current_task;
   } else {
      // Check if the task exists
      if (!isbitset(taskbmap, idx)) {
         __hw_sti();
         return E_NOEXS;
      }
      tsk = &task_pool[idx];
   }

   // Save task state
   pk_rtsk->tskstat = tsk->tskstat;
   pk_rtsk->tskpri  = tsk->tskpri;
   pk_rtsk->tskbpri = tsk->tskbpri;
   pk_rtsk->tskwait = tsk->tskwait;
   pk_rtsk->wobjid  = tsk->wobjid;
   pk_rtsk->actcnt  = tsk->actcnt;
   pk_rtsk->wupcnt  = tsk->wupcnt;
   pk_rtsk->suscnt  = tsk->suscnt;

   // Remaining timeout calculated on-line (WARNING: lefttmo is 32 bits wide)
   pk_rtsk->lefttmo = (uint32_t)(tsk->wuptim - get_system_time());

   __hw_sti();

   return E_OK;
}

/*
 * ret_tst_sys()
 */
ER ref_tst_sys(ID tskid, T_RTST *pk_rtst)
{
   task_t *tsk;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   // Check if a task is terminating itself
   if (!pk_rtst == TSK_SELF) {
      return E_PAR;
   }

   __hw_cli();

   // Get task from the task pool
   if (tskid == TSK_SELF) {
      tsk = current_task;
   } else {
      // Check if the task exists
      if (!isbitset(taskbmap, idx)) {
         __hw_sti();
         return E_NOEXS;
      }
      tsk = &task_pool[idx];
   }

   // Save simplified task state
   pk_rtst->tskstat = tsk->tskstat;
   pk_rtst->tskwait = tsk->tskwait;

   __hw_sti();

   return E_OK;
}

/*
 * rel_wai_sys()
 *
 *    Release task from waiting
 */
ER rel_wai_sys(ID tskid)
{
   task_t *tsk;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   // A task cannot specify its own task ID
   if (tskid == TSK_SELF) {
      return E_OBJ;
   }

   __hw_cli();

   // Check if the task exists
   if (!isbitset(taskbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get the task from the pool
   tsk = &task_pool[idx];

   // Task is in the DORMANT state
   if (!(tsk->tskstat & TTS_WAI)) {
      __hw_sti();
      return E_OBJ;
   }

   // Release from waiting
   tsk->tskstat &= ~TTS_WAI;

   if (!tsk->tskstat) {
      rq_insert(tsk);
      tsk->tskstat = TTS_RDY;
   }

   __hw_sti();

   scheduler();

   return E_OK;
}

/*
 * ter_tsk_sys()
 *
 *    TODO: Finish the implementation of this function
 */
ER ter_tsk_sys(ID tskid)
{
   task_t *tsk;
   int32_t idx = tskid - 1;

   // Check invalid ID
   if (tskid < 1 || tskid > MAX_TASKS) {
      return E_ID;
   }

   // Check if a task is terminating itself
   if (tskid == TSK_SELF) {
      return E_ILUSE;
   }

   __hw_cli();

   // Check if the task exists
   if (!isbitset(taskbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get the task from the pool
   tsk = &task_pool[idx];

   // Task is in the DORMANT state
   if (tsk->tskstat == TTS_DMT) {
      __hw_sti();
      return E_OBJ;
   }
}








