/*
 * $FILE: mempool.c
 *
 * ObelISK dynamic memory management.
 *
 * Variable-sized memory blocks are managed with
 * TLSF --> http://rtportal.upv.es/rtmalloc/
 *
 * Fixed-sized memory blocks managed with mpool.c
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
#include <mempool.h>
#include <mpool.h>
#include <tlsf.h>
#include <sched.h>
#include <string.h>

__kobject static bitmap_t mpfbmap;
__kobject static mpfpool_t mpf_pool[CONFIG_MAX_FMPOOL];

__kobject static bitmap_t mplbmap;
__kobject static mplpool_t mpl_pool[CONFIG_MAX_VMPOOL];

/*
 * init_mpl_pool()
 *
 *    Initialize variable sized memory pools
 */
void __init init_mpl_pool(void)
{
   int i;

   mplbmap = clearbitmap();
   mpl_pool[0] = (mplpool_t) {
            .base = &_userpool1_base,
            .length = (uint32_t)&_userpool1_size,
            .id = 1,
   };
   mpl_pool[1] = (mplpool_t) {
            .base = &_userpool2_base,
            .length = (uint32_t)&_userpool2_size,
            .id = 2,
   };
   mpl_pool[2] = (mplpool_t) {
            .base = &_userpool3_base,
            .length = (uint32_t)&_userpool3_size,
            .id = 3,
   };
   mpl_pool[3] = (mplpool_t) {
            .base = &_userpool4_base,
            .length = (uint32_t)&_userpool4_size,
            .id = 4,
   };

   // Two Level Segregated Fit initialization
   for (i=0; i<4; i++) {
      init_memory_pool(mpl_pool[i].length, mpl_pool[i].base);
      mplbmap = setbit(mplbmap, i);
   }

   // Kernel region for stack storage
   //init_kregion((void *) &_kmem_start, &_kmem_size);
}

/*
 * init_mpf_pool()
 *
 *    Initialize fixed size memory pools
 */
void __init init_mpf_pool(void)
{
   mpfbmap = clearbitmap();
   memset(mpf_pool, 0, sizeof(mpf_pool));
}

/*
 * cre_mpf_sys()
 *
 *    Create fixed-size memory pool
 *
 *    WCET: Non Constant
 */
ER cre_mpf_sys(ID mpfid, T_CMPF *pk_cmpf)
{
   mpfpool_t *mpf;
   int32_t idx = mpfid - 1;

   // Check invalid memory pool ID
   if (mpfid < 1 || mpfid > CONFIG_MAX_FMPOOL) {
      return E_PAR;
   }

   // Check invalid pk_cmpf pointer
   if (!pk_cmpf) {
      return E_PAR;
   }

   // Check invalid pk_cmpf parameters
   if (!pk_cmpf->blksz) {
      return E_PAR;
   }

   __hw_cli();

   // Check if ID is in use
   if (isbitset(mpfbmap, idx)) {
      __hw_sti();
      return E_OBJ;
   }

   // Get memory pool info
   mpf = &mpf_pool[idx];

   // Zero memory pool structure
   memset(mpf, 0, sizeof(mpfpool_t));

   // Align block size to the next 32-bit boundary
   mpf->blksz = ALIGN32(pk_cmpf->blksz);

   // Assign memory pool attributes
   mpf->mpf    = pk_cmpf->mpf;
   mpf->blkcnt = pk_cmpf->blkcnt;
   mpf->mpfatr = pk_cmpf->mpfatr;

   // Initialize pool
   init_pool(mpf->mpf, mpf->blksz, mpf->blkcnt);

   // Mark memory pool ID as used
   mpfbmap = setbit(mpfbmap, idx);

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_CRE_MPF, mpfid, E_OK);
#endif

   __hw_sti();

   return E_OK;
}

/*
 * del_mpf_sys()
 *
 *    Delete fixed-sized memory pool
 *
 *    WCET: Non Constant
 */
ER del_mpf_sys(ID mpfid)
{
   mpfpool_t *mpf;
   task_t *tsk;
   int32_t idx = mpfid - 1;

   // Check invalid memory pool ID
   if (mpfid < 1 || mpfid > CONFIG_MAX_FMPOOL) {
      return E_PAR;
   }

   __hw_cli();

   // Check if the pool exists
   if (!isbitset(mpfbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get memory pool info
   mpf = &mpf_pool[idx];

#ifdef CONFIG_SYSLOG_ACTIVE
   write_log(TFN_DEL_MPF, mpfid, E_OK);
#endif

   // Check the memory pool queue for waiting tasks
   if (!queue_is_empty(&mpf->mpfque)) {
      while (tsk=queue_extract(&mpf->mpfque)) {
         tsk->wobjid = E_DLT;
         tsk->tskstat = TTS_RDY;
         rq_insert(tsk);
      }
   }

   // Clear bitmap to mark as available ID
   mpfbmap = clearbit(mpfbmap, idx);

   __hw_sti();

   scheduler();

   return E_OK;
}

/*
 * get_mpf_sys()
 *
 *    Acquire fixed-size memory pool block
 *
 *    WCET: Constant
 */
ER get_mpf_sys(ID mpfid, VP *p_blk)
{
   mpfpool_t *mpf;
   int32_t idx = mpfid - 1;
   struct mpf_block *blk;

   // Check invalid memory pool ID
   if (mpfid < 1 || mpfid > CONFIG_MAX_FMPOOL) {
      return E_PAR;
   }

   // Check invalid p_blk pointer
   if (!p_blk) {
      return E_PAR;
   }

   __hw_cli();

   // Check if the memory pool exists
   if (!isbitset(mpfbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get memory pool info
   mpf = &mpf_pool[idx];

   // Try to get a free block from the list
   blk = mpalloc(mpf->mpf);

   if (!blk) {  // No free blocks available
      // TODO: Consider memory pool wait queue managed by priorities (TA_TPRI)
      // Put the task into the WAITING state
      queue_insert(&mpf->mpfque, &current_task->synque);

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_GET_MPF, mpfid, TTS_WAI);
#endif

      // Change task state to WAITING
      current_task->tskstat = TTS_WAI;
      current_task->tskwait = TTW_MPF;
      rq_remove(current_task);

   } else {             // Free block available
      *p_blk = blk;
      __hw_sti();
      return E_OK;
   }

   __hw_sti();

   scheduler();

   __hw_cli();

   // Memory pool was deleted while waiting
   if (current_task->wobjid == E_DLT) {
      __hw_sti();
      return E_DLT;
   }

   // Return the block address
   *p_blk = current_task->extdat;

   __hw_sti();

   return E_OK;
}

/*
 * rel_mpf_sys()
 *
 *    Release fixed-size memory pool block
 *
 *    WCET: Constant for each mpf
 */
ER rel_mpf_sys(ID mpfid, VP blk)
{
   mpfpool_t *mpf;
   task_t *tsk;
   int32_t blkpos;
   int32_t idx = mpfid - 1;


   // Check invalid memory pool ID
   if (mpfid < 1 || mpfid > CONFIG_MAX_FMPOOL) {
      return E_PAR;
   }

   // Check invalid blk pointer
   if (!blk) {
      return E_PAR;
   }

   __hw_cli();

   // Check if the memory pool exists
   if (!isbitset(mpfbmap, idx)) {
      __hw_sti();
      return E_NOEXS;
   }

   // Get memory pool info
   mpf = &mpf_pool[idx];

   // Check invalid blk pointer
   if ((uint32_t)blk < (uint32_t)mpf->mpf ||
       (uint32_t)blk > ((uint32_t)mpf->mpf + mpf->blkcnt*mpf->blksz)) {
      __hw_sti();
      return E_PAR;
   }

   // Check if the blk is actually being used
   /*if (CONTAINER_OF(blk)->next != BLOCK_USED) {
      __hw_sti();
      return E_PAR;
   }*/

   // Check the memory pool queue for waiting tasks
   if (!queue_is_empty(&mpf->mpfque)) {

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_REL_MPF, mpfid, E_OK);
#endif
      // Wake-up task at the head of the queue
      tsk = queue_extract(&mpf->mpfque);
      tsk->extdat = blk;
      rq_insert(tsk);

   } else {
      // Insert the block on the free list
      mpfree(mpf->mpf, blk);
   }

   __hw_sti();

   scheduler();

   return E_OK;
}

/*
 * cre_mpl_sys()
 *
 *    Create variable-sized memory pool block (TLSF)
 */
ER cre_mpl_sys(ID mplid, T_CMPL *pk_cmpl)
{
   mplpool_t *mpl;
   int32_t idx = mplid - 1;

   // Check memory pool ID
   if (mplid < 1 || mplid > CONFIG_MAX_VMPOOL) {
      return E_ID;
   }

   // Check invalid p_blk pointer
   if (!pk_cmpl) {
      return E_PAR;
   }

   // Check invalid p_blk values
   if (!pk_cmpl->mpl) {
      return E_PAR;
   }

   __hw_cli();

   // Check if ID is in use
   if (isbitset(mplbmap, idx)) {
      __hw_sti();
      return E_OBJ;
   }

   // Get memory pool
   mpl = &mpl_pool[idx];

   mpl->length = pk_cmpl->mplsz;
   mpl->base = pk_cmpl->mpl;

   // Initialize variable-size memory pool using TLSF
   init_memory_pool(mpl->length, mpl->base);

   __hw_sti();

   return E_OK;
}

/*
 * get_mpl_sys()
 *
 *    Acquire variable-sized memory pool block (TLSF)
 *
 *    WCET: Constant
 */
ER get_mpl_sys(ID mplid, UINT blksz, VP *p_blk)
{
   mplpool_t *mpl;
   int32_t idx = mplid - 1;

   // Check memory pool ID
   if (mplid < 1 || mplid > CONFIG_MEM_POOLS) {
      return E_ID;
   }

   // Check invalid p_blk pointer
   if (!p_blk) {
      return E_PAR;
   }

   __hw_cli();

   // Get memory pool
   mpl = &mpl_pool[idx];

   if (blksz > mpl->length) {
      __hw_sti();
      return E_PAR;
   }


   if (!(*p_blk = malloc_ex(blksz, mpl->base))) {
      // TODO: Place the invoking task into the waiting queue
      // of the memory pool
      // Not enough memory. Place the invoking task into the waiting
      // queue of the memory pool
   }

   __hw_sti();

   return E_OK;
}

/*
 * rel_mpl_sys()
 *
 *    Release variable-sized memory pool block
 *
 *    WCET: Constant
 */
ER rel_mpl_sys(ID mplid, VP blk)
{
   mplpool_t *mpl;
   int32_t idx = mplid - 1;

   // Check memory pool ID
   if (mplid < 1 || mplid > CONFIG_MEM_POOLS) {
      return E_ID;
   }

   // Check alignment
   if ((uint32_t)blk & 0x3) {
      return E_PAR;
   }

   __hw_cli();

   // Get memory pool
   mpl = &mpl_pool[idx];

   free_ex(blk, mpl->base);

   // TODO: Check the queue to see if there is a task
   // waiting which can acquire memory

   __hw_sti();

   return E_OK;
}




