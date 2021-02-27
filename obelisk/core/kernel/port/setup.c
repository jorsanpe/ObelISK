/*
 * $FILE: setup.c
 *
 * Kernel setup procedure
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
#include <time.h>
#include <arch/ints.h>
#include <arch/sysmem.h>

extern uint32_t _syspool_base;
extern uint32_t _syspool_size;
void *system_pool;
extern uint32_t _kfastpool_base;
extern uint32_t _kfastpool_size;
void *kfast_pool;

__init static init_syspools(void)
{
   system_pool = &_syspool_base;
   kfast_pool = &_kfastpool_base;
   init_memory_pool(&_syspool_size, system_pool);
   init_memory_pool(&_kfastpool_size, kfast_pool);
}

__init void inicalls(void)
{
   extern initcall_t __initcalls_start[], __initcalls_end[];
   initcall_t *call;

   for (call=__initcalls_start; call<__initcalls_end; ++call) {
      if ((*call)() < 0) {
#ifdef CONFIG_SYSLOG
         write_log(TFN_DEL_FLG, call, -1);
#endif
      }
   }
}

/*
 * start_kernel()
 *
 *    Kernel initialization procedure
 */
__init void start_kernel(void)
{
   // Setup Memory
   init_mmu();

   // Init system and fast memory pools
   init_syspools();

   // Scheduler init
   init_sched();

   // System log init
   init_syslog();

   inicalls();

   ////////////////////
   // Only for debug //
   //init_tmr_log();
   ////////////////////

   // Initialize the mpf and mpl pools
   init_mpf_pool();
   init_mpl_pool();

   // Initialize the task pool
   init_task_pool();

   // Initialize the semaphores pool
   init_sem_pool();

   // Initialize eventflags pool
   init_flg_pool();

   // Initialize the mailbox pool
   init_mbox_pool();

   // Initialize devices
   init_devices();

   // Create idle task
   create_idle_task();

#ifdef CONFIG_USING_LWIP
   // Initialize lwIP
   init_lwip_stack();
#endif

   // Create main task
   create_main_task();

   // Start kernel timer
   init_time();

   // Enable dispatching
   ena_dsp_sys();

   // Call the scheduler to start the main task
   scheduler();

   while (1) {
      //__hw_sti();
      asm volatile("idle;\n\t");
   }
}



