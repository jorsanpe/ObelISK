/*
 * $FILE: mailbox.c
 *
 * ObelISK Mailbox management functions
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
#include <mailbox.h>
#include <mempool.h>
#include <sched.h>
#include <tlsf.h>
#include <string.h>

struct mbox_msg_head {
   void *msgdat;
   linked_list_t msgque;
};

// The mailbox pool is defined statically
__kobject static bitmap_t mbxbmap;
__kobject static mailbox_t mbx_pool[CONFIG_MAX_MAILBOX];
__kobject static struct mpool *msg_pool;

/*
 * init_mbox_pool()
 *
 *    Mailbox pool initialization
 */
void __init init_mbox_pool(void)
{
   mbxbmap = clearbitmap();
   memset(mbx_pool, 0, sizeof(mbx_pool));
   msg_pool = malloc_ex(POOL_SIZE(sizeof(struct mbox_msg_head), CONFIG_MAX_MAILBOX_MSG), kfast_pool);
   init_pool(msg_pool, sizeof(struct mbox_msg_head), CONFIG_MAX_MAILBOX_MSG);
}

/*
 * acre_mbx_sys()
 *
 *    Create mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ID acre_mbx_sys(T_CMBX *pk_cmbx)
{
   mailbox_t *mbx;
   int32_t idx, mbxid;

   // Check invalid pk_cmbx pointer
   if (!pk_cmbx) {
      return E_PAR;
   }

   // TODO: Check mailbox attributes

   __hw_cli();

   // Check if any ID is available
   idx = _ffc(mbxbmap);
   mbxid = idx + 1;

   // No mailbox ID available
   if (mbxid < 1 || mbxid > CONFIG_MAX_MAILBOX) {
      __hw_sti();
      return E_NOID;
   }

   // Get mailbox from the pool
   mbx = &mbx_pool[idx];
   memset(mbx, 0, sizeof(mailbox_t));

   // Assign mailbox attributes
   mbx->mbxatr  = pk_cmbx->mbxatr;
   mbx->maxmpri = pk_cmbx->maxmpri;

   // TODO: Mailbox messages handled by priorities
   /*// Check if mailbox messages are handled by priorities
   if (pk_cmbx->mbxatr & TA_TPRI) {
      // If the start address for the mailbox message header area is not
      // specified, allocate it
      if (!pk_cmbx->mprihd) {
         // This should allocate a default number of list elements
         if (!(mbx->mprihd=calloc_ex(CONFIG_DEFAULT_MBOXHD_SIZE, sizeof(T_MSG), system_pool))) {
#ifdef CONFIG_SYSLOG_ACTIVE
            write_log(TFN_CRE_MBX, mbxid, E_NOMEM);
#endif
            __hw_sti();
            return E_NOMEM;
         }
      } else {
         mbx->mprihd  = pk_cmbx->mprihd;
      }
   }*/

   // Mark as used
   mbxbmap = setbit(mbxbmap, idx);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_ACRE_MBX, mbxid, E_OK);
   /*mbx->actlst.this = mbx;
   insert_linked_list(&active_mbx, &mbx->actlst);*/
#endif

   __hw_sti();

   return mbxid;
}

/*
 * cre_mbx_sys()
 *
 *    Create mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ER cre_mbx_sys(ID mbxid, T_CMBX *pk_cmbx)
{
   mailbox_t *mbx;
   int32_t idx = mbxid - 1;

   // Check invalid mailbox ID
   if (mbxid < 1 || mbxid > CONFIG_MAX_MAILBOX) {
      return E_ID;
   }

   // Check invalid pk_cmbx pointer
   if (!pk_cmbx) {
      return E_PAR;
   }

   // TODO: Check mailbox attributes

   __hw_cli();

   // ID already in use
   if (isbitset(mbxbmap, idx)) {
      __hw_sti();
      return E_OBJ;
   }

   // Get mailbox from the pool
   mbx = &mbx_pool[idx];
   memset(mbx, 0, sizeof(mailbox_t));

   // Assign mailbox attributes
   mbx->mbxatr  = pk_cmbx->mbxatr;
   mbx->maxmpri = pk_cmbx->maxmpri;

   // TODO: Mailbox messages handled by priorities
   /*// Check if mailbox messages are handled by priorities
   if (pk_cmbx->mbxatr & TA_TPRI) {
      // If the start address for the mailbox message header area is not
      // specified, allocate it
      if (!pk_cmbx->mprihd) {
         // This should allocate a default number of list elements
         if (!(mbx->mprihd=calloc_ex(CONFIG_DEFAULT_MBOXHD_SIZE, sizeof(T_MSG), system_pool))) {
            __hw_sti();
            return E_NOMEM;
         }
      } else {
         mbx->mprihd  = pk_cmbx->mprihd;
      }
   }*/

   // Mark as used
   mbxbmap = setbit(mbxbmap, idx);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_CRE_MBX, mbxid, E_OK);
   //mbx->actlst.this = mbx;
   //insert_linked_list(&active_mbx, &mbx->actlst);
#endif

   __hw_sti();

   return E_OK;
}

/*
 * del_mbx_sys()
 *
 *    Delete mailbox
 *
 *    TODO: Check any memory that has been allocated by the kernel
 *    to release it. Case of mailboxes handled by priorities
 *
 *    WCET: Non Constant
 */
ER del_mbx_sys(ID mbxid)
{
   mailbox_t *mbx;
   task_t *tsk;
   struct mbox_msg_head *msg;
   int32_t idx = mbxid - 1;

   // Check invalid mailbox ID
   if (mbxid < 1 || mbxid > CONFIG_MAX_MAILBOX) {
      return E_ID;
   }

   __hw_cli();
   // Mailbox does not exist
   if (!isbitset(mbxbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get mailbox from the pool
   mbx = &mbx_pool[idx];

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_DEL_MBX, mbxid, E_OK);
#endif

   // Wake-up waiting tasks
   while (tsk = queue_extract(&mbx->mbxque)) {
      tsk->tskstat = TTS_RDY;
      rq_insert(tsk);
      tsk->wobjid = E_DLT;
   }

   // Release messages
   while (msg = queue_extract(&mbx->msgque)) {
      mpfree(msg_pool, msg);
   }

   // Clear bitmap to mark as available ID
   mbxbmap = clearbit(mbxbmap, idx);

   __hw_sti();

   scheduler();

   return E_OK;
}

/*
 * rcv_mbx_sys()
 *
 *    Receive from mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ER rcv_mbx_sys(ID mbxid, T_MSG **ppk_msg)
{
   mailbox_t *mbx;
   task_t *tsk;
   linked_list_t *list;
   struct mbox_msg_head *msg;
   int32_t idx = mbxid - 1;

   // Check invalid mailbox ID
   if (mbxid < 1 || mbxid > CONFIG_MAX_MAILBOX) {
      return E_ID;
   }

   // Check invalid ppk_msg pointer
   if (!ppk_msg) {
      return E_PAR;
   }

   __hw_cli();

   // No such mailbox
   if (!isbitset(mbxbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get the mailbox from the mailbox pool
   mbx = &mbx_pool[idx];

   // Check if there is any message on the mailbox queue
   if (queue_is_empty(&mbx->msgque)) { // No message available

      // TODO: Consider mailbox wait queue managed by priorities (TA_TPRI)

      // Put the current task on the mailbox's wait queue
      queue_insert(&mbx->mbxque, &current_task->synque);

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_RCV_MBX, mbxid, TTS_WAI);
#endif

      // Change task state to WAITING
      current_task->tskstat = TTS_WAI;
      current_task->tskwait = TTW_MBX;
      current_task->wobjid = mbxid;
      rq_remove(current_task);
   } else {                            // Message available
      // Get message from the queue
      msg = queue_extract(&mbx->msgque);
      *ppk_msg = msg->msgdat;
      mpfree(msg_pool, msg);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_TRCV_MBX, mbxid, E_OK);
#endif
      __hw_sti();
      return E_OK;
   }

   __hw_sti();

   scheduler();

   // Task has been released from waiting
   __hw_cli();

   // Mailbox was deleted while waiting
   if (current_task->wobjid == E_DLT) {
#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_RCV_MBX, mbxid, E_DLT);
#endif
      current_task->wobjid = 0;
      __hw_sti();
      return E_DLT;
   }

   // A message has been received
   msg = current_task->extdat;
   *ppk_msg = msg->msgdat;
   mpfree(msg_pool, msg);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_RCV_MBX, mbxid, E_OK);
#endif

   __hw_sti();

   return E_OK;
}

/*
 * trcv_mbx_sys()
 *
 *    Receive from mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ER trcv_mbx_sys(ID mbxid, T_MSG **ppk_msg, TMO tmout)
{
   mailbox_t *mbx;
   task_t *tsk;
   linked_list_t *list;
   struct mbox_msg_head *msg;
   int32_t idx = mbxid - 1;

   // If tmo = TMO_FEVR twai_sem() acts the same as wai_sem()
   if (tmout == TMO_FEVR) {
      return rcv_mbx_sys(mbxid, ppk_msg);
   }

   // Check invalid mailbox ID
   if (mbxid < 1 || mbxid > CONFIG_MAX_MAILBOX) {
      return E_ID;
   }

   // Check invalid ppk_msg pointer
   if (!ppk_msg) {
      return E_PAR;
   }

   __hw_cli();

   // No such mailbox
   if (!isbitset(mbxbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get the mailbox from the mailbox pool
   mbx = &mbx_pool[idx];

   // Check if there is any message on the mailbox queue
   if (queue_is_empty(&mbx->msgque)) { // No message available

      // TODO: Consider mailbox wait queue managed by priorities (TA_TPRI)

      // Put the current task on the mailbox's wait queue
      queue_insert(&mbx->mbxque, &current_task->synque);

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_TRCV_MBX, mbxid, tmout);
#endif

      // Change task state to WAITING
      current_task->tskstat = TTS_WAI;
      current_task->tskwait = TTW_MBX;
      current_task->wobjid = mbxid;
      rq_remove(current_task);
      tq_insert(current_task, tmout);
   } else {                            // Message available
      // Get message from the queue
      msg = queue_extract(&mbx->msgque);
      *ppk_msg = msg->msgdat;
      mpfree(msg_pool, msg);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_TRCV_MBX, mbxid, tmout);
#endif
      __hw_sti();
      return E_OK;
   }

   __hw_sti();

   scheduler();

   // Task has been released from waiting
   __hw_cli();

   // Timeout expired
   if (current_task->wobjid == E_TMOUT) {
      queue_remove(&mbx->mbxque, &current_task->synque);
      current_task->wobjid = 0;
      __hw_sti();
      return E_TMOUT;
   }

   // Mailbox was deleted while waiting
   if (current_task->wobjid == E_DLT) {
#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_TRCV_MBX, mbxid, E_DLT);
#endif
      current_task->wobjid = 0;
      __hw_sti();
      return E_DLT;
   }

   // A message has been received
   msg = current_task->extdat;
   *ppk_msg = msg->msgdat;
   mpfree(msg_pool, msg);

   // Remove from timeout queue
   tq_remove(current_task);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_TRCV_MBX, mbxid, tmout);
#endif

   __hw_sti();

   return E_OK;
}

/*
 * ref_mbx_sys()
 *
 *    Reference mailbox state
 *
 *    WCET: Constant
 */
ER ref_mbx_sys(ID mbxid, T_RMBX *pk_rmbx)
{
   mailbox_t *mbx;
   task_t *tsk;
   struct mbox_msg_head *msg;
   int32_t idx = mbxid - 1;

   // Check invalid mailbox ID
   if (mbxid < 1 || mbxid > CONFIG_MAX_MAILBOX) {
      return E_ID;
   }

   // Check invalid pk_rmbx pointer
   if (!pk_rmbx) {
      return E_PAR;
   }

   __hw_cli();

   // No such mailbox
   if (!isbitset(mbxbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get the mailbox from the mailbox pool
   mbx = &mbx_pool[idx];

   // Check if mailbox task queue is empty
   pk_rmbx->wtskid = TSK_NONE;
   if (tsk=queue_get_head(&mbx->mbxque)) {
      pk_rmbx->wtskid = tsk->tskid;
   }

   // Get the message at the head of the mailbox queue
   msg = queue_get_head(&mbx->msgque);
   pk_rmbx->pk_msg = (msg != NULL ? msg->msgdat : NULL);

   __hw_sti();

   return E_OK;
}

/*
 * snd_mbx_sys()
 *
 *    Send to mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ER snd_mbx_sys(ID mbxid, T_MSG *pk_msg)
{
   mailbox_t *mbx;
   task_t *tsk;
   struct mbox_msg_head *msg;
   int32_t idx = mbxid - 1;

   // Check invalid mailbox ID
   if (mbxid < 1 || mbxid > CONFIG_MAX_MAILBOX) {
      return E_ID;
   }

   // Check invalid pk_msg pointer
   if (!pk_msg) {
      return E_PAR;
   }

   __hw_cli();

   // No such mailbox
   if (!isbitset(mbxbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Set mailbox message queue pointer
   msg = mpalloc(msg_pool);
   msg->msgdat = pk_msg;
   msg->msgque.this = msg;

   // Get the mailbox from the mailbox pool
   mbx = &mbx_pool[idx];

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_SND_MBX, mbxid, E_OK);
#endif

   // Check if there are tasks waiting on the mailbox queue
   if (!queue_is_empty(&mbx->mbxque)) {

      // Wake-up the task on the head of the mailbox queue
      tsk = queue_extract(&mbx->mbxque);
      tsk->tskstat = TTS_RDY;

      // Pass the message to the task
      tsk->extdat = msg;
      rq_insert(tsk);
   } else {
      // No task waiting, insert the message on the
      // mailbox message queue
      queue_insert(&mbx->msgque, &msg->msgque);
   }

   __hw_sti();

   scheduler();

   return E_OK;
}




