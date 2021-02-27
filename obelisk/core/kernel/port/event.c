/*
 * $FILE: event.c
 *
 * ObelISK Eventflag management functions
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
 */

#include <itron.h>
#include <processor.h>
#include <linkage.h>
#include <event.h>
#include <sched.h>
#include <tlsf.h>
#include <string.h>

__kobject static bitmap_t flgbmap;
__kobject static eventflag_t flg_pool[CONFIG_MAX_EVENT];
static linked_list_t *active_flg;

/*
 * init_flg_pool()
 *
 *    Eventflag pool initialization
 */
void __init init_flg_pool(void)
{
   active_flg = NULL;
   flgbmap = clearbitmap();
   memset(flg_pool, 0, sizeof(flg_pool));
   /*for (i=0; i<CONFIG_MAX_EVENT; i++) {
      flg_pool[i] = malloc_ex(sizeof(eventflag_t), system_pool);
   }*/
}

/*
 * clr_flg_sys()
 *
 *    Clear eventflag
 *
 *    WCET: Constant
 */
ER clr_flg_sys(ID flgid, FLGPTN clrptn)
{
   eventflag_t *flg;
   int32_t idx = flgid - 1;

   // No action required
   if (clrptn == -1) {
      return E_OK;
   }

   // Check invalid eventflag ID
   if (flgid < 1 || flgid > CONFIG_MAX_EVENT) {
      return E_ID;
   }

   __hw_cli();

   // Flag does not exist
   if (!isbitset(flgbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get the flag from the pool
   flg = &flg_pool[idx];

   // Clear the flag pattern
   flg->flgptn &= clrptn;

//#ifdef CONFIG_SYSLOG_ACTIVE
   //write_log(TFN_CLR_FLG, flgid, E_OK);
//#endif

   __hw_sti();

   return E_OK;
}

/*
 * cre_flg_sys()
 *
 *    Create Eventflag
 *
 *    WCET: Constant
 */
ER cre_flg_sys(ID flgid, T_CFLG *pk_cflg)
{
   eventflag_t *flg;
   int32_t idx = flgid - 1;

   // Check invalid eventflag ID
   if (flgid < 1 || flgid > CONFIG_MAX_EVENT) {
      return E_ID;
   }

   // Check invalid pk_cflg pointer
   if (!pk_cflg) {
      return E_PAR;
   }

   __hw_cli();

   // ID already in use
   if (isbitset(flgbmap, idx)) {
      __hw_sti();
      return E_OBJ;
   }

   // Get eventflag from the pool
   flg = &flg_pool[idx];
   memset(flg, 0, sizeof(eventflag_t));

   // Assign eventflag attributes
   flg->flgatr = pk_cflg->flgatr;

   // Mark as used
   flgbmap = setbit(flgbmap, idx);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_CRE_FLG, flgid, E_OK);
   flg->actlst.this = flg;
   insert_linked_list(&active_flg, &flg->actlst);
#endif

   __hw_sti();

   return E_OK;
}

/*
 * del_flg_sys()
 *
 *    Delete eventflag
 *
 *    WCET: Non Constant
 */
ER del_flg_sys(ID flgid)
{
   eventflag_t *flg;
   task_t *tsk;
   int32_t idx = flgid - 1;

   // Check invalid eventflag ID
   if (flgid < 1 || flgid > CONFIG_MAX_EVENT) {
      return E_ID;
   }

   __hw_cli();

   // Flag does not exist
   if (!isbitset(flgbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get eventflag from the pool
   flg = &flg_pool[idx];

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_DEL_FLG, flgid, E_OK);
#endif

   // Wakeup waiting tasks
   while (tsk=queue_extract(&flg->flgque)) {
      tsk->tskstat = TTS_RDY;
      rq_insert(tsk);
      tsk->wobjid = E_DLT;
   }

   // Clear bitmap to mark as available ID
   flgbmap = clearbit(flgbmap, idx);

   __hw_sti();

   scheduler();

   return E_OK;
}

/*
 * set_flg_sys()
 *
 *    Set eventflag
 *
 *    WCET: Non Constant
 */
ER set_flg_sys(ID flgid, FLGPTN setptn)
{
   eventflag_t *flg;
   task_t *tsk;
   linked_list_t *slist;
   FLGPTN ptnmatch;
   int32_t idx = flgid - 1;

   // No action required
   if (!setptn) {
      return E_OK;
   }

   // Check invalid eventflag ID
   if (flgid < 1 || flgid > CONFIG_MAX_EVENT) {
      return E_ID;
   }

   __hw_cli();

   // Flag does not exist
   if (!isbitset(flgbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get the flag from the pool
   flg = &flg_pool[idx];

   // Update the flag pattern to the bitwise OR
   flg->flgptn |= setptn;

   // Write to the log
#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_SET_FLG, flgid, E_OK);
#endif

   // Look for tasks satisfying its release condition
   if (!queue_is_empty(&flg->flgque)) {
      for (slist=flg->flgque.head; slist!=NULL; slist=slist->next) {
         tsk = slist->this;

         // Check pattern
         ptnmatch = tsk->wflgptn & flg->flgptn;
         if (ptnmatch) {
            // If it's an AND wait, check if it satisfies the condition
            if (!(tsk->wflgatr & TWF_ORW)) {
               if (ptnmatch != tsk->wflgptn) {
                  continue;
               }
            }
            // Release condition satisfied
            // Wakeup task
            queue_remove(&flg->flgque, &tsk->synque);
            tsk->tskstat = TTS_RDY;
            rq_insert(tsk);

            // If the TA_CLR attribute is set, clear the bit pattern
            if (flg->flgatr & TA_CLR) {
               flg->flgptn = 0;
               break;
            }

            // Multiple wakeup not allowed. Actually not needed
            if (!(flg->flgatr & TA_WMUL)) {
               break;
            }
         }
      }
   }
   __hw_sti();

   scheduler();

   return E_OK;
}

/*
 * wai_flg_sys()
 *
 *    Wait for eventflag
 *    TODO: Consider eventflag wait queue managed by priorities (TA_TPRI)
 *
 *    WCET: Constant
 */
ER wai_flg_sys(ID flgid, FLGPTN waiptn, UINT wfmode, FLGPTN *p_flgptn)
{
   eventflag_t *flg;
   FLGPTN ptnmatch;
   int32_t idx = flgid - 1;

   // Check invalid wait pattern
   if (!waiptn) {
      return E_PAR;
   }

   // Check invalid wait mode
   if (wfmode > 1) {
      return E_PAR;
   }

   // Check invalid p_flgptn pointer
   if (!p_flgptn) {
      return E_PAR;
   }

   // Check invalid eventflag ID
   if (flgid < 1 || flgid > CONFIG_MAX_EVENT) {
      return E_ID;
   }

   __hw_cli();

   // Flag does not exist
   if (!isbitset(flgbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get the flag from the pool
   flg = &flg_pool[idx];

   // There is already a task waiting for the eventflag
   // with the TA_WSLG attribute
   if (!(flg->flgatr & TA_WMUL)) {
      if (!queue_is_empty(&flg->flgque)) {
         __hw_sti();
         return E_ILUSE;
      }
   }

   // Check pattern
   ptnmatch = waiptn & flg->flgptn;
   if (ptnmatch) {
      // If it's an AND wait, check if it satisfies the condition
      if (!(wfmode & TWF_ORW)) {
         if (ptnmatch != waiptn) {
            ptnmatch = 0;
         }
      }
   }

   // Check release condition
   if (ptnmatch) {           // Satisfied
      // Save return pattern
      *p_flgptn = flg->flgptn;
      if (flg->flgatr & TA_CLR) {
         flg->flgptn = 0;
      }
#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_WAI_FLG, flgid, E_OK);
#endif
      __hw_sti();
      return E_OK;
   } else {                  // Not satisfied
      current_task->wflgptn = waiptn;

      // Put the current task on the eventflag's wait queue
      queue_insert(&flg->flgque, &current_task->synque);

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_WAI_FLG, flgid, TTS_WAI);
#endif

      // Change task state to WAITING
      current_task->tskstat = TTS_WAI;
      current_task->tskwait = TTW_FLG;
      rq_remove(current_task);
   }

   __hw_sti();

   scheduler();

   __hw_cli();

   // Eventflag was deleted while waiting
   if (current_task->wobjid == E_DLT) {
#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_WAI_FLG, flgid, E_DLT);
#endif
      __hw_sti();
      return E_DLT;
   }

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_WAI_FLG, flgid, E_OK);
#endif

   __hw_sti();

   return E_OK;
}







