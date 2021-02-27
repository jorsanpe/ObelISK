/*
 * $FILE: insterrupts.S
 *
 * Kernel interrupt wrappers
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

#include <processor.h>
#include <linkage.h>
#include <asm-offsets.h>
#include "context.S"

.macro BUILD_EXCEPTION _exNr
.endm

.macro BUILD_INTERRUPT _irqNr
   SAVE_CONTEXT
   p4 = \_irqNr (z);
   jump _common_interrupt_body;
.endm

.section .irqtab, "ax"
ENTRY(_exception_entry)
   EX_EARLY_PUSH

   // Get the exception handler from the table
   r7 = SEQSTAT;
   r6 = 0x3f (z);
   r7 = r7 & r6;
   cc = r7 == 0;   // Fast system call check
   if cc jump _syscall_prolog;

   EX_PUSH_REMAINING
   p5.h = _ex_table;
   p5.l = _ex_table;
   p4 = r7;
   p5 = p5 + (p4 << 2);
   p4 = [p5];

   // Call the exception handler
   sp += -12;
   call (p4);
   sp += 12;

   EX_RESTORE_CTXT
   rtx;
ENDPROC(_exception_entry)

ENTRY(_interrupt_vector_7)
    BUILD_INTERRUPT 0

ENTRY(_interrupt_vector_8)
    BUILD_INTERRUPT 1

ENTRY(_interrupt_vector_9)
    BUILD_INTERRUPT 2

ENTRY(_interrupt_vector_10)
    BUILD_INTERRUPT 3

ENTRY(_interrupt_vector_11)
    BUILD_INTERRUPT 4

ENTRY(_interrupt_vector_12)
    BUILD_INTERRUPT 5

ENTRY(_interrupt_vector_13)
    BUILD_INTERRUPT 6

.previous
