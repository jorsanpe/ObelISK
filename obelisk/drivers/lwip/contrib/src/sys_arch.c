/*
 * $FILE: sys_arch.c
 *
 * Architecture wrapper for lwIP porting to ObelISK
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

#include <linkage.h>
#include <mempool.h>
#include <sched.h>
#include <string.h>
#include <time.h>
#include <tlsf.h>
#include <arch/cc.h>
#include <arch/sys_arch.h>
#include <lwip/debug.h>
#include <lwip/sys.h>
#include <lwip/opt.h>
#include <lwip/stats.h>

#define MAX_SEM_COUNT      0x7FFFFFFF

struct sys_timeouts *lwip_system_timeouts;         // Default timeouts list for lwIP
struct thread_struct_wrapper *lwip_system_threads; // a list of all threads created by lwIP

/*
 * sys_init()
 */
void sys_init(void)
{
   lwip_system_timeouts = NULL;
   lwip_system_threads = NULL;
}

/*
 * sys_thread_new()
 *
 *    Thread creation for lwIP
 */
sys_thread_t sys_thread_new(char *name, void (* thread)(void *arg), void *arg, int stacksize, int prio)
{
   T_CTSK pk_ctsk;
   sys_thread_t newthread;
   SYS_ARCH_DECL_PROTECT(old_val);

   // allocate the space for the thread wrapper
   //get_mpl_sys(0, sizeof(struct thread_struct_wrapper), (void *)&newthread);
   newthread = sys_malloc(sizeof(struct thread_struct_wrapper));

   // Actually not needed, as get_mpl should perform the relevant checks
   if (newthread==NULL) {
      return NULL;
   }

   memset(newthread, 0, sizeof(struct thread_struct_wrapper));

   SYS_ARCH_PROTECT(old_val); // Need to protect this -- preemption here could be a problem!
   newthread->next = lwip_system_threads;
   lwip_system_threads = newthread;
   SYS_ARCH_UNPROTECT(old_val);

   // initialize the linked list to NULL
   newthread->timeouts = NULL;

   // Assign values to the task creation packet
   memset(&pk_ctsk, 0, sizeof(pk_ctsk));
   pk_ctsk.task = (void (*)(void))thread;
   pk_ctsk.stksz = stacksize;
   pk_ctsk.itskpri = prio;
   pk_ctsk.exinf = (void *)(TSK_VPARG | TSK_KERNEL);
   pk_ctsk.extdat = arg;
   pk_ctsk.stk = sys_malloc(stacksize);
   //get_mpl_sys(0, pk_ctsk.stksz, &pk_ctsk.stk);

   // Create task with automatic ID assignment
   newthread->thread = acre_tsk_sys(&pk_ctsk);
   act_tsk_sys(newthread->thread);

   return newthread;
}

/*
 * sys_arch_timeouts()
 *
 *    Get current thread timeout list
 */
struct sys_timeouts *sys_arch_timeouts(void) {
  sys_thread_t thread;
  ID self = current_task->tskid;

  // Search the threads list for the thread that is currently running
  for (thread=lwip_system_threads ; thread!=NULL; thread=thread->next) {
     if (thread->thread == self)
        //return thread->timeouts;
        return (struct sys_timeouts *)&thread->timeouts;
  }

  // No match, so just return the system-wide default version
  return lwip_system_timeouts;
}

/*
 * sys_sem_new()
 *
 *    Create new semaphore
 */
sys_sem_t sys_sem_new(u8_t count)
{
   T_CSEM pk_csem;
   sys_sem_t sem;

   pk_csem.sematr = 0;
   pk_csem.isemcnt = count;
   pk_csem.maxsem = MAX_SEM_COUNT;

   sem = acre_sem_sys(&pk_csem);

   if (sem < 0) {
      return SYS_SEM_NULL;
   }
   return sem;
}

/*
 * sys_sem_free()
 *
 *    Delete semaphore
 */
void sys_sem_free(sys_sem_t sem)
{
   del_sem_sys(sem);
}

/*
 * sys_sem_signal()
 *
 *    Signal semaphore
 */
void sys_sem_signal(sys_sem_t sem)
{
   sig_sem_sys(sem);
}

/*
 * sys_arch_sem_wait()
 *
 *    Acquire semaphore. If timeout = 0 the function should
 *    block indefinitely. The function returns the number of
 *    milliseconds expired while waiting or SYS_ARCH_TIMEOUT
 *    if the timeout expired.
 *
 */
u32_t sys_arch_sem_wait(sys_sem_t sem, u32_t timeout)
{
   SYSTIM extim;      // Expire time
   ER retval;

   extim = get_system_time();

   // Acquire semaphore
   retval = twai_sem_sys(sem, timeout);

   // Check timeout
   if (retval == E_TMOUT) {
      return SYS_ARCH_TIMEOUT;
   }
   return (u32_t)ticks_to_ms(get_system_time() - extim);
}

/*
 * sys_mbox_new()
 *
 *    Create new mailbox
 */
sys_mbox_t sys_mbox_new(int size)
{
   T_CMBX pk_cmbx;
   sys_mbox_t mbox;

   pk_cmbx.mbxatr = TA_TFIFO;
   pk_cmbx.maxmpri = 0;
   pk_cmbx.mprihd = NULL;

   mbox = acre_mbx_sys(&pk_cmbx);

   if (mbox < 0) {
      return SYS_MBOX_NULL;
   }
   return mbox;
}

/*
 * sys_mbox_free()
 *
 *    Delete mailbox
 */
void sys_mbox_free(sys_mbox_t mbox)
{
   del_mbx_sys(mbox);
}

/*
 * sys_mbox_post()
 *
 *    Post a message to a mailbox
 */
void sys_mbox_post(sys_mbox_t mbox, void *msg)
{
   T_MSG *pk_msg = msg;

   // Allocate the message queue header. Use the
   // system pool which has ID 0
   //get_mpl_sys(0, sizeof(T_MSG), (void *)&pk_msg);
   //pk_msg = sys_malloc(sizeof(T_MSG));
   //memset(pk_msg, 0, sizeof(T_MSG));

   //pk_msg->msgdat = msg;

   snd_mbx_sys(mbox, pk_msg);
}

/*
 * sys_mbox_trypost()
 *
 *    Post a message to a mailbox
 */
err_t sys_mbox_trypost(sys_mbox_t mbox, void *msg)
{
   T_MSG *pk_msg = msg;

   // Allocate the message queue header. Use the
   // system pool which has ID 0
   //get_mpl_sys(0, sizeof(T_MSG), (void *)&pk_msg);
   //pk_msg = sys_malloc(sizeof(T_MSG));
   //memset(pk_msg, 0, sizeof(T_MSG));
   //pk_msg->msgdat = msg;

   if (snd_mbx_sys(mbox, pk_msg) == E_OK) {
      return ERR_OK;
   }
   return ERR_MEM;
}

/*
 * sys_arch_mbox_fetch()
 *
 *    Blocks the thread until a message arrives in the mailbox
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t mbox, void **msg, u32_t timeout)
{
   SYSTIM extim;     // Expire time
   ER retval;        // Return value
   T_MSG *pk_msg;
   SYS_ARCH_DECL_PROTECT(old_val);

   extim = get_system_time();

   // Acquire semaphore
   retval = trcv_mbx_sys(mbox, &pk_msg, timeout);

   // Check timeout
   if (retval == E_TMOUT) {
      return SYS_ARCH_TIMEOUT;
   }

   // If msg is NULL message is dropped
   if (msg != NULL) {
      *msg = pk_msg;
   }

   // Return elapsed milliseconds
   SYS_ARCH_PROTECT(old_val);
   extim = ticks_to_ms(get_system_time() - extim);
   SYS_ARCH_UNPROTECT(old_val);

   return (u32_t)extim;
}

/*
 * sys_arch_mbox_tryfetch()
 *
 *    The function has a critical region as if the invoking task
 *    is preempted and another task fetches the mailbox and
 *    empties it, the invoking task could get blocked
 */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t mbox, void **msg)
{
   T_RMBX pk_rmbx;
   T_MSG *pk_msg;

   // Disable dispatching
   dis_dsp_sys();

   // Reference mailbox state
   if (ref_mbx_sys(mbox, &pk_rmbx) == E_OK) {
      if (pk_rmbx.pk_msg != NULL) {

         // Get the message at the head of the mailbox
         rcv_mbx_sys(mbox, &pk_msg);

         // If msg is NULL message is dropped
         if (msg != NULL) {
            *msg = pk_msg;
         }

         // Release message header
         //sys_free(pk_msg);
      } else {
         // Mailbox was empty, re-enable dispatching
         ena_dsp_sys();
         return SYS_MBOX_EMPTY;
      }
   }

   // Re-enable dispatching
   ena_dsp_sys();
}

/*
 * sys_malloc()
 *
 *    Memory allocation wrapper
 */
void *sys_malloc(size_t size)
{
   /*void *ptr;
   if (get_mpl_sys(0, size, (void *)&ptr) == E_OK)
      return ptr;*/
   return malloc_ex(size, system_pool);
}

/*
 * sys_calloc()
 *
 *    Memory allocation wrapper with initialization
 */
void *sys_calloc(size_t size)
{
   /*void *ptr;
   if (get_mpl_sys(0, size, (void *)&ptr) == E_OK) {
      memset(ptr, 0, size);
      return ptr;
   }
   return NULL;
   */

   return calloc_ex(size, 1, system_pool);
}

/*
 * sys_free()
 *
 *    Memory deallocation wrapper
 */
void sys_free(void *ptr)
{
   //rel_mpl_sys(0, ptr);
   free_ex(ptr, system_pool);
}




