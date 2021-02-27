/*
 * $FILE: asm-offsets.c
 *
 * Assembler offsets generation
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
#include <stddef.h>
#include <task.h>
#include <types.h>
#include <arch/stack.h>

void assembler_offset_generator(void)
{
   DEFINE(TASK_USTACK, offsetof(task_t, stk));
   DEFINE(TASK_KSTACK, offsetof(task_t, kstk));
   DEFINE(TASK_EXINF, offsetof(task_t, exinf));
   DEFINE(R0_ON_STACK, offsetof(struct stk_trace, r0));
}
