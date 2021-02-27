/*
 * $FILE: stack.h
 *
 * Blackfin architecture specific registers configuration
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

#ifndef _STACK_H_
#define _STACK_H_

#ifndef _KERNEL_
#error   Internal file. Do not include it in your sources.
#endif

extern int setup_stack(task_t *tsk);

struct stk_trace {
   unsigned long seqstat;
   unsigned long rete;
   unsigned long retn;
   unsigned long retx;
   unsigned long reti;
   unsigned long rets;
   unsigned long astat;
   unsigned long lb1;
   unsigned long lb0;
   unsigned long lt1;
   unsigned long lt0;
   unsigned long lc1;
   unsigned long lc0;
   unsigned long a1w;
   unsigned long a1x;
   unsigned long a0w;
   unsigned long a0x;
   unsigned long b3;
   unsigned long b2;
   unsigned long b1;
   unsigned long b0;
   unsigned long l3;
   unsigned long l2;
   unsigned long l1;
   unsigned long l0;
   unsigned long m3;
   unsigned long m2;
   unsigned long m1;
   unsigned long m0;
   unsigned long i3;
   unsigned long i2;
   unsigned long i1;
   unsigned long i0;
   unsigned long usp;
   unsigned long fp;
   unsigned long p5;
   unsigned long p4;
   unsigned long p3;
   unsigned long p2;
   unsigned long p1;
   unsigned long p0;
   unsigned long r7;
   unsigned long r6;
   unsigned long r5;
   unsigned long r4;
   unsigned long r3;
   unsigned long r2;
   unsigned long r1;
   unsigned long r0;
   unsigned long syscfg;
};

#endif   //_STACK_H_
