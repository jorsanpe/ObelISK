/*
 * $FILE: semaphore.c
 *
 * ObelISK Semaphores management functions
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
#include <processor.h>
#include <linkage.h>
#include <semaphore.h>
#include <sched.h>
#include <tlsf.h>
#include <string.h>

static bitmap_t sembmap;
__kobject static semaphore_t sem_pool[CONFIG_MAX_SEM];
static linked_list_t *active_sem;

/*
 * init_sem_pool()
 *
 *    Semaphore pool initialization
 */
void __init init_sem_pool(void)
{
   active_sem = NULL;
   sembmap = clearbitmap();
   memset(sem_pool, 0, sizeof(sem_pool));
   /*for (i=0; i<CONFIG_MAX_SEM; i++) {
    sem_pool[i] = malloc_ex(sizeof(semaphore_t), system_pool);
    }*/
}

/*
 * acre_sem_sys()
 *
 *    Create semaphore with automatid ID assignment
 *
 *    WCET: Constant
 */
ER acre_sem_sys(T_CSEM *pk_csem)
{
   semaphore_t *sem;
   int32_t idx, semid;

   // Check invalid pk_csem pointer
   if (!pk_csem) {
      return E_PAR;
   }

   // Check invalid pk_csem values
   if (pk_csem->maxsem < pk_csem->isemcnt || pk_csem->maxsem == 0) {
      return E_PAR;
   }

   __hw_cli();

   // Check if any ID is available
   idx = _ffc(sembmap);
   semid = idx + 1;

   // No semaphore ID available
   if (semid < 1 || semid > CONFIG_MAX_SEM) {
      __hw_sti();
      return E_NOID;
   }

   // Get semaphore from the pool
   sem = &sem_pool[idx];
   memset(sem, 0, sizeof(semaphore_t));

   // Assign semaphore attributes
   sem->sematr = pk_csem->sematr;
   sem->semcnt = pk_csem->isemcnt;
   sem->maxsem = pk_csem->maxsem;

   sembmap = setbit(sembmap, idx);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_ACRE_SEM, semid, E_OK);
   sem->actlst.this = sem;
   insert_linked_list(&active_sem, &sem->actlst);
#endif

   __hw_sti();

   return semid;
}

/*
 * cre_sem_sys()
 *
 *    Create semaphore
 *
 *    WCET: Constant
 */
ER cre_sem_sys(ID semid, T_CSEM *pk_csem)
{
   semaphore_t *sem;
   int32_t idx = semid - 1;

   // Check invalid semaphore ID
   if (semid < 1 || semid > CONFIG_MAX_SEM) {
      return E_ID;
   }

   // Check invalid pk_csem pointer
   if (!pk_csem) {
      return E_PAR;
   }

   // Check invalid pk_csem values
   if (pk_csem->maxsem < pk_csem->isemcnt || pk_csem->maxsem == 0) {
      return E_PAR;
   }

   __hw_cli();

   // ID already in use
   if (isbitset(sembmap, idx)) {
      __hw_sti();
      return E_OBJ;
   }

   // Get semaphore from the pool
   sem = &sem_pool[idx];
   //memset(sem, 0, sizeof(semaphore_t));
   memset(sem, 0, sizeof(semaphore_t));

   // Assign semaphore attributes
   sem->sematr = pk_csem->sematr;
   sem->semcnt = pk_csem->isemcnt;
   sem->maxsem = pk_csem->maxsem;

   sembmap = setbit(sembmap, idx);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_CRE_SEM, semid, E_OK);
   sem->actlst.this = sem;
   insert_linked_list(&active_sem, &sem->actlst);
#endif

   __hw_sti();

   return E_OK;
}

/*
 * del_sem_sys()
 *
 *    Delete semaphore
 *
 *    WCET: Non Constant
 */
ER del_sem_sys(ID semid)
{
   task_t *tsk;
   semaphore_t *sem;
   int32_t idx = semid - 1;

   // Check invalid eventflag ID
   if (semid < 1 || semid > CONFIG_MAX_SEM) {
      return E_ID;
   }

   __hw_cli();

   // Semaphore does not exist
   if (!isbitset(sembmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get semaphore from the pool
   sem = &sem_pool[idx];

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_DEL_SEM, semid, E_OK);
#endif

   // Wakeup waiting tasks
   while (tsk = queue_extract(&sem->semque)) {
      rq_insert(tsk);
      tsk->wobjid = E_DLT;
   }

   // Clear bitmap to mark as available ID
   sembmap = clearbit(sembmap, idx);

   __hw_sti();

   return E_OK;
}

/*
 * sig_sem_sys()
 *
 *    Release semaphore
 *
 *    WCET: Constant
 */
ER sig_sem_sys(ID semid)
{
   semaphore_t *sem;
   task_t *tsk;
   int32_t idx = semid - 1;

   // Bad semaphore ID
   if (semid < 1 || semid > CONFIG_MAX_SEM) {
      return E_ID;
   }

   __hw_cli();

   // No such semaphore
   if (!isbitset(sembmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get semaphore from the pool
   sem = &sem_pool[idx];

   // Queuing overflow
   if (sem->semcnt == sem->maxsem) {
      __hw_sti();
      return E_QOVR;
   }

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_SIG_SEM, semid, E_OK);
#endif

   // Check if there were tasks on the wait queue of the semaphore
   tsk = queue_extract(&sem->semque);
   if (tsk != NULL) {
      tsk->tskstat = TTS_RDY;
      rq_insert(tsk);
   } else {
      sem->semcnt++;
   }

   __hw_sti();

   scheduler();

   return E_OK;
}

/*
 * wai_sem_sys()
 *
 *    Acquire semaphore
 *    TODO: Consider semaphore wait queue managed by priorities (TA_TPRI)
 *
 *    WCET: Constant
 */
ER wai_sem_sys(ID semid)
{
   semaphore_t *sem;
   int32_t idx = semid - 1;

   // Bad semaphore ID
   if (semid < 1 || semid > CONFIG_MAX_SEM) {
      return E_ID;
   }

   __hw_cli();

   // No such semaphore
   if (!isbitset(sembmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get semaphore from the pool
   sem = &sem_pool[idx];

   // Check current count
   if (sem->semcnt >= 1) { // Semaphore available

      // Decrement semaphore count
      sem->semcnt--;

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_WAI_SEM, semid, E_OK);
#endif
      __hw_sti();
      return E_OK;
   } else { // Semaphore unavailable

      // Put the current task on the semaphore's wait queue
      queue_insert(&sem->semque, &current_task->synque);

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_WAI_SEM, semid, TTS_WAI);
#endif

      // Change task state to WAITING
      current_task->tskstat = TTS_WAI;
      current_task->tskwait = TTW_SEM;
      current_task->wobjid = semid;
      rq_remove(current_task);
   }

   __hw_sti();

   scheduler();

   __hw_cli();

   // Semaphore was deleted while waiting
   if (current_task->wobjid == E_DLT) {
      __hw_sti();
      return E_DLT;
   }

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_WAI_SEM, semid, E_OK);
#endif

   __hw_sti();

   return E_OK;
}

/*
 * wai_sem_sys()
 *
 *    Acquire semaphore with timeout
 *    TODO: Consider semaphore wait queue managed by priorities (TA_TPRI)
 *
 *    WCET: Constant
 */
ER twai_sem_sys(ID semid, TMO tmout)
{
   semaphore_t *sem;
   int32_t idx = semid - 1;

   // If tmo = TMO_FEVR twai_sem() acts the same as wai_sem()
   if (tmout == TMO_FEVR) {
      return wai_sem_sys(semid);
   }

   // Bad semaphore ID
   if (semid < 1 || semid > CONFIG_MAX_SEM) {
      return E_ID;
   }

   __hw_cli();

   // No such semaphore
   if (!isbitset(sembmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get semaphore from the pool
   sem = &sem_pool[idx];

   // Check current count
   if (sem->semcnt >= 1) { // Semaphore available

      // Decrement semaphore count
      sem->semcnt--;

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_TWAI_SEM, semid, E_OK);
#endif
      __hw_sti();
      return E_OK;
   } else { // Semaphore unavailable
      // Put the current task on the semaphore's wait queue
      queue_insert(&sem->semque, &current_task->synque);

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_TWAI_SEM, semid, TTS_WAI);
#endif

      // Change task state to WAITING
      current_task->tskstat = TTS_WAI;
      current_task->tskwait = TTW_SEM;
      current_task->wobjid = semid;
      rq_remove(current_task);
      tq_insert(current_task, tmout);
   }

   __hw_sti();

   scheduler();

   __hw_cli();

   // Semaphore was deleted while waiting
   if (current_task->wobjid == E_DLT) {
      __hw_sti();
      return E_DLT;
   }

   // Timeout expired
   if (current_task->wobjid == E_TMOUT) {
      __hw_sti();
      return E_TMOUT;
   }

   // Remove from timeout queue
   tq_remove(current_task);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_TWAI_SEM, semid, E_OK);
#endif

   __hw_sti();

   return E_OK;
}

