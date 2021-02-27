/*
 * $FILE: stack.c
 *
 * Blackfin architecture specific stack configuration
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
#include <mempool.h>
#include <processor.h>
#include <sched.h>
#include <tlsf.h>


/*
 * Despite its use of explicit registers, this assembly code
 * is startup and registers do not carry any information at all. Thus,
 * it's safe to use any register we want without defining clobbers
 */
#define JUMP_USER_TASK(entry, arg) \
   __asm__ __volatile__("p0 = %0;\n\t" \
                        "r0 = %1;\n\t" \
                        "reti = p0;\n\t" \
                        "rti;\n\t" \
                        : :"r"(entry), "r"(arg));

#define JUMP_KERNEL_TASK(entry, arg) \
   __asm__ __volatile__("p0 = %0;\n\t" \
                        "r0 = %1;\n\t" \
                        "call (p0);\n\t" \
                        : :"r"(entry), "r"(arg));

/*
 * startup_fn()
 */
static void startup_fn(void)
{
   // Check if the task function should receive any arguments

   if ((uint32_t)current_task->exinf & TSK_KERNEL) {
      __hw_sti();
      JUMP_KERNEL_TASK(current_task->task, current_task->extdat);
   } else {
      __hw_sti();
      JUMP_USER_TASK(current_task->task, current_task->extdat);
   }

   /*
    if ((uint32_t)current_task->exinf & TSK_VPARG) {
      void *(*task_vparg)(void *) = (void *(*)(void *))current_task->task;

      __hw_sti();

   } else {
      __hw_sti();
      JUMP_TASK(current_task->task, 0);
   }
   */

   // Safety instruction. If the task was correctly implemented
   // this code should never be executed
   exd_tsk_sys();
}

/*
 * init_stack()
 *
 *    Setup task stack
 */
__inocache int init_stack(task_t *tsk)
{
   if ((uint32_t)tsk->exinf & TSK_KERNEL) {
      tsk->kstk = tsk->stk + tsk->stksz - 4;
      tsk->kstk -= 4;
      *((void **)tsk->kstk) = startup_fn;
   } else {
      tsk->stk = tsk->stk + tsk->stksz - 4;
      tsk->kstk = malloc_ex(CONFIG_KSTACK_SIZE, kfast_pool) + CONFIG_KSTACK_SIZE - 4;
      tsk->kstk -= 4;
      *((void **)tsk->kstk) = startup_fn;
   }



   return 0;
}

