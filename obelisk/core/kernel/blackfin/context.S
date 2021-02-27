/*
 * $FILE: context.S
 *
 * Context management macros
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

/*.macro SAVE_IRQ_CTXT
   sp = usp;
   [--sp] = (r7:0, p5:0);
   [--sp] = rets;
   [--sp] = astat;
   [--sp] = fp;
   [--sp] = reti;  // Re-enable interrupts
   usp = sp;
.endm

.macro RESTORE_IRQ_CTXT
   reti = [sp++];  // Disable interrupts
   fp = [sp++];
   astat = [sp++];
   rets = [sp++];
   (r7:0, p5:0) = [sp++];
   usp = sp;
.endm*/

.macro EX_EARLY_PUSH
   [--sp] = astat;
   [--sp] = r7;
   [--sp] = r6;
.endm

.macro EX_PUSH_REMAINING
   [--sp] = r5;
   [--sp] = r4;
   [--sp] = r3;
   [--sp] = r2;
   [--sp] = r1;
   [--sp] = r0;
   [--sp] = p5;
   [--sp] = p4;
   [--sp] = p3;
   [--sp] = p2;
   [--sp] = p1;
   [--sp] = p0;
   [--sp] = rets;
   [--sp] = fp;
.endm

.macro EX_RESTORE_CTXT
   fp    = [sp++];
   rets  = [sp++];
   p0    = [sp++];
   p1    = [sp++];
   p2    = [sp++];
   p3    = [sp++];
   p4    = [sp++];
   p5    = [sp++];
   r0    = [sp++];
   r1    = [sp++];
   r2    = [sp++];
   r3    = [sp++];
   r4    = [sp++];
   r5    = [sp++];
   r6    = [sp++];
   r7    = [sp++];
   astat = [sp++];
.endm

.macro SAVE_IRQ_CONTEXT
	[--sp] = SYSCFG;

	[--sp] = (R7:0, P5:0);
	[--sp] = fp;
	[--sp] = usp;

	[--sp] = i0;
	[--sp] = i1;
	[--sp] = i2;
	[--sp] = i3;

	[--sp] = m0;
	[--sp] = m1;
	[--sp] = m2;
	[--sp] = m3;

	[--sp] = l0;
	[--sp] = l1;
	[--sp] = l2;
	[--sp] = l3;

	[--sp] = b0;
	[--sp] = b1;
	[--sp] = b2;
	[--sp] = b3;
	[--sp] = a0.x;
	[--sp] = a0.w;
	[--sp] = a1.x;
	[--sp] = a1.w;

	[--sp] = LC0;
	[--sp] = LC1;
	[--sp] = LT0;
	[--sp] = LT1;
	[--sp] = LB0;
	[--sp] = LB1;

	[--sp] = ASTAT;
	[--sp] = RETS;
	[--sp] = RETI;
	[--sp] = RETX;
	[--sp] = RETN;
	[--sp] = RETE;
	[--sp] = SEQSTAT;
.endm

.macro RESTORE_IRQ_CONTEXT
	SEQSTAT = [sp++];
	RETE = [sp++];
	RETN = [sp++];
	RETX = [sp++];
	RETI = [sp++];
	RETS = [sp++];
	ASTAT = [sp++];

	LB1 = [sp++];
	LB0 = [sp++];
	LT1 = [sp++];
	LT0 = [sp++];
	LC1 = [sp++];
	LC0 = [sp++];

	a1.w = [sp++];
	a1.x = [sp++];
	a0.w = [sp++];
	a0.x = [sp++];
	b3 = [sp++];
	b2 = [sp++];
	b1 = [sp++];
	b0 = [sp++];

	l3 = [sp++];
	l2 = [sp++];
	l1 = [sp++];
	l0 = [sp++];

	m3 = [sp++];
	m2 = [sp++];
	m1 = [sp++];
	m0 = [sp++];

	i3 = [sp++];
	i2 = [sp++];
	i1 = [sp++];
	i0 = [sp++];

	usp = [sp++];
	fp = [sp++];

	(R7:0,P5:0) = [sp++];
	csync;
	SYSCFG = [sp++];
	csync;
.endm

.macro SAVE_CONTEXT
	[--sp] = SYSCFG;

	[--sp] = (R7:0, P5:0);
	[--sp] = fp;
	[--sp] = usp;

	[--sp] = i0;
	[--sp] = i1;
	[--sp] = i2;
	[--sp] = i3;

	[--sp] = m0;
	[--sp] = m1;
	[--sp] = m2;
	[--sp] = m3;

	[--sp] = l0;
	[--sp] = l1;
	[--sp] = l2;
	[--sp] = l3;

	[--sp] = b0;
	[--sp] = b1;
	[--sp] = b2;
	[--sp] = b3;
	[--sp] = a0.x;
	[--sp] = a0.w;
	[--sp] = a1.x;
	[--sp] = a1.w;

	[--sp] = LC0;
	[--sp] = LC1;
	[--sp] = LT0;
	[--sp] = LT1;
	[--sp] = LB0;
	[--sp] = LB1;

	[--sp] = ASTAT;
	[--sp] = RETS;
	r7 = RETI;
	[--sp] = r7;
	[--sp] = RETX;
	[--sp] = RETN;
	[--sp] = RETE;
	[--sp] = SEQSTAT;
.endm

.macro RESTORE_CONTEXT
	SEQSTAT = [sp++];
	RETE = [sp++];
	RETN = [sp++];
	RETX = [sp++];
	r7   = [sp++];
	RETI = r7;
	RETS = [sp++];
	ASTAT = [sp++];

	LB1 = [sp++];
	LB0 = [sp++];
	LT1 = [sp++];
	LT0 = [sp++];
	LC1 = [sp++];
	LC0 = [sp++];

	a1.w = [sp++];
	a1.x = [sp++];
	a0.w = [sp++];
	a0.x = [sp++];
	b3 = [sp++];
	b2 = [sp++];
	b1 = [sp++];
	b0 = [sp++];

	l3 = [sp++];
	l2 = [sp++];
	l1 = [sp++];
	l0 = [sp++];

	m3 = [sp++];
	m2 = [sp++];
	m1 = [sp++];
	m0 = [sp++];

	i3 = [sp++];
	i2 = [sp++];
	i1 = [sp++];
	i0 = [sp++];

	usp = [sp++];
	fp = [sp++];

	(R7:0,P5:0) = [sp++];
	csync;
	SYSCFG = [sp++];
	csync;
.endm
