/*
 * $FILE: entry.S
 *
 * Kernel low-level entry routines
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

#include <asm-offsets.h>
#include <linkage.h>
#include <processor.h>
#include <sched.h>
#include <syscalls.h>

#include "context.S"

.section .irqtab, "ax"
// This function lowers interrupt priority as the exception handler
// runs at the highest priority among system maskable interrupts. Otherwise,
// we wouldn't be able to preempt the kernel
ENTRY(_syscall_prolog)
   [--sp] = p5;
   [--sp] = p4;
   p4.l = lo(EVT15);
   p4.h = hi(EVT15);
   p5.l = _system_call;
   p5.h = _system_call;
   [p4] = p5;
   raise 15;
   p4 = [sp++];
   p5 = [sp++];
   r6 = [sp++];
   r7 = [sp++];
   astat = [sp++];
   rtx;
ENDPROC(_syscall_prolog)
   /*
     p0 - syscall_nr
     r0 - arg1 & retval
     r1 - arg2
     ...
   */
ENTRY(_system_call)
   SAVE_IRQ_CONTEXT

   // Check syscall_nr
   p1 = NR_SYSCALLS;
   cc = p1 < p0;
   if cc jump 1f;
   cc = p0 < 0;
   if cc jump 1f;

   p5.h = _syscall_table;
   p5.l = _syscall_table;

   p5 = p5 + (p0 << 2);
   p4 = [p5];

   // Check syscall pointer
   cc = p4 == 0
   if cc jump 1f

   // TODO: Check Blackfin ABI as parameters 4 and beyond
   // are not passed through the data registers
   sp += -12;
   call (p4);
   sp += 12;

   jump 2f;
1:
   r0 = UNKNOWN_SYSCALL (x);
2:
   [sp + R0_ON_STACK] = r0;  // Return value
   RESTORE_IRQ_CONTEXT
   rti;
ENDPROC(_system_call)

ENTRY(_common_interrupt_body)
   // Increase interrupt nesting counter
   p3.l = _int_nest_count;
   p3.h = _int_nest_count;
   r0 = [p3];
   r0 += 1;
   [p3] = r0;

   // Log the ocurrence of the interrupt
   r0 = 0xEE (x);         // event, interrupt code
   r1 = p4;               // arg1, interrupt number
   r2 = 0 (z);            // arg2, null
   p5.l = _write_log;
   p5.h = _write_log;
   sp += -12;
   call (p5);
   sp += 12;

   // Call the installed handler
   p5.l = _interrupt_table;
   p5.h = _interrupt_table;
   p5 = p5 + (p4 << 2);
   p5 = [p5];
   sp += -12;
   call (p5);
   sp += 12;

   // Decrease interrupt nesting counter
   p3.l = _int_nest_count;
   p3.h = _int_nest_count;
   r0 = [p3];
   r0 += -1;
   [p3] = r0;

   // Check if we are on a nested interrupt
   cc = r0 == 0;
   if !cc jump return_from_int;

   // Log again the interrupt to mark exit
   r0 = 0xEE (x);         // event, interrupt code
   r1 = p4;               // arg1, interrupt number
   r2 = 0 (z);            // arg2, null
   p5.l = _write_log;
   p5.h = _write_log;
   sp += -12;
   call (p5);
   sp += 12;

leaving_interrupt_scope:
   // Check if we need to reschedule
   p0.l = _need_resched;
   p0.h = _need_resched;
   r0 = [p0];
   cc = r0 == 1;
   if !cc jump return_from_int;

entering_scheduler_from_int:
   // Lower interrupt level to 15 to call scheduler
   p4.l = lo(EVT15);
   p4.h = hi(EVT15);
   p5.l = _schedule_from_int;
   p5.h = _schedule_from_int;
   [p4] = p5;
   raise 15;

return_from_int:
   RESTORE_CONTEXT
   rti;
ENDPROC(_common_interrupt_body)

ENTRY(_schedule_from_int)

   SAVE_IRQ_CONTEXT

   sp += -12;
   call _scheduler;
   sp += 12;

   // Check if kernel task or user task
   /*p0.l = _current_task;
   p0.h = _current_task;
   p0 = [p0];
   r0 = [p0 + TASK_EXINF];
   r1 = TSK_KERNEL;
   r0 = r0 & r1;
   cc = r0 == 0;  // TSK_KERNEL = 2
   if cc jump return_from_int;

   // Return to kernel task
   p4.l = lo(EVT14);
   p4.h = hi(EVT14);
   p5.l = return_to_kernel;
   p5.h = return_to_kernel;
   [p4] = p5;
   raise 14;*/

return_to_kernel:
   RESTORE_IRQ_CONTEXT
   rti;
ENDPROC(_schedule_from_int)


ENTRY(_evt_timer)
   SAVE_CONTEXT

   sp += -12;
   call _evt_tmr_isr;
   sp += 12;

   // Log the ocurrence of the interrupt
   /*r0 = 0xEE (x);         // event, interrupt code
   r1 = 16;               // arg1, interrupt number
   r2 = 0 (z);            // arg2, null
   p5.l = _write_log;
   p5.h = _write_log;
   sp += -12;
   call (p5);
   sp += 12;*/

   // Lower interrupt level to 15 to call scheduler
   jump entering_scheduler_from_int;
ENDPROC(_evt_timer)

// r0 carries current task pointer, r1 carries new task
ENTRY(_context_switch)
	SAVE_CONTEXT

	p0 = r0;
	p0 = [p0];
	p1 = r1;

	// Push program counter
	r1.l = _resume;
	r1.h = _resume;
	[--sp] = r1;

	// Save current task stack pointers
	p2 = usp;
	[p0+TASK_USTACK] = p2;
	[p0+TASK_KSTACK] = sp;

	// Restore new task stack pointer
	p2 = [p1+TASK_USTACK];
	usp = p2;
	sp = [p1+TASK_KSTACK];

	// Restore program counter
	p0 = [sp++];

	// Update current task
	p2 = r0;
	[p2] = p1;

	// Jump to current task
	jump (p0);

	// Tasks will resume from here
_resume:
   RESTORE_CONTEXT
   rts;
ENDPROC(_context_switch)

#ifdef CONFIG_JOB_DISPATCHER
DQueueLauncherStartup:

#endif

// An unhandled interrupt ocurred
ENTRY(_unhandled_interrupt)
block_in_unhandled_interrupt:
   jump block_in_unhandled_interrupt;
ENDPROC(_unhandled_interrupt)


// Kernel Panic. This may be due to several reasons.
// At this point, we only consider the possibility of
// waiting on a semaphore while being on the dispatching
// disabled state
ENTRY(_kernel_panic)
_kernel_panic_stop:
	idle;
	jump _kernel_panic_stop;
ENDPROC(_kernel_panic)
.previous
