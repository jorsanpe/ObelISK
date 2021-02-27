/*
 * $FILE: syscalls.h
 *
 * User System Calls
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

#ifndef _ULIBC_SYSCALLS_
#define _ULIBC_SYSCALLS_

#define _syscall0(syscall_nr) \
({ \
  long __res; \
  __asm__ __volatile__ ( \
  "p0 = %1;\n\t" \
  "excpt 0;\n\t" \
  "%0=r0;\n\t" \
  : "=da" (__res) \
  : "d" (syscall_nr) \
  : "CC", "P0"); \
  __res; \
})

#define _syscall1(syscall_nr,a1) \
({ \
  long __res; \
  __asm__ __volatile__ ( \
  "r0=%2;\n\t"     \
  "p0=%1;\n\t"     \
  "excpt 0;\n\t"   \
  "%0=r0;\n\t"     \
        : "=da" (__res)  \
        : "d" (syscall_nr), \
     "a" ((long)(a1)) \
   : "CC", "R0", "P0");    \
  __res;     \
})

#define _syscall2(syscall_nr,a1,a2) \
({ \
  long __res; \
  __asm__ __volatile__ ( \
  "r1=%3;\n\t" \
  "r0=%2;\n\t" \
  "p0=%1;\n\t" \
  "excpt 0;\n\t" \
  "%0=r0;\n\t" \
        : "=da" (__res)  \
        : "d" (syscall_nr), \
     "a" ((long)(a1)), \
     "a" ((long)(a2)) \
   : "CC", "R0","R1", "P0");  \
  __res; \
})

#define _syscall3(syscall_nr,a1,a2,a3) \
({ \
  long __res; \
  __asm__ __volatile__ ( \
  "r2=%4;\n\t" \
  "r1=%3;\n\t" \
  "r0=%2;\n\t" \
  "p0=%1;\n\t" \
  "excpt 0;\n\t" \
  "%0=r0;\n\t" \
        : "=da" (__res)  \
        : "d"   (syscall_nr),  \
     "a"   ((long)(a1)), \
     "a"   ((long)(a2)), \
     "a"   ((long)(a3))  \
        : "CC", "R0","R1","R2", "P0"); \
  __res; \
})

#define _syscall4(syscall_nr,a1,a2,a3,a4) \
({ \
  long __res; \
  __asm__ __volatile__ ( \
  "[--sp] = r3;\n\t"  \
  "r3=%5;\n\t" \
  "r2=%4;\n\t" \
  "r1=%3;\n\t" \
  "r0=%2;\n\t" \
  "p0=%1;\n\t" \
  "excpt 0;\n\t" \
  "%0=r0;\n\t" \
  "r3 = [sp++];\n\t" \
   : "=da" (__res) \
   : "d"  (syscall_nr), \
     "a"  ((long)(a1)), \
     "a"  ((long)(a2)), \
     "a"  ((long)(a3)), \
     "a"  ((long)(a4)) \
   : "CC", "R0","R1","R2","R3", "P0"); \
  __res; \
})

#define _syscall5(syscall_nr,a1,a2,a3,a4,a5) \
({ \
  long __res; \
  __asm__ __volatile__ ( \
  "[--sp] = r3;\n\t"  \
  "r3=%5;\n\t" \
  "r2=%4;\n\t" \
  "r1=%3;\n\t" \
  "r0=%2;\n\t" \
  "p0=%1;\n\t" \
  "excpt 0;\n\t" \
  "%0=r0;\n\t" \
  "r3 = [sp++];\n\t" \
   : "=da" (__res) \
   : "d"  (syscall_nr), \
     "a"  ((long)(a1)), \
     "a"  ((long)(a2)), \
     "a"  ((long)(a3)), \
     "a"  ((long)(a4)) \
   : "CC", "R0","R1","R2","R3", "P0"); \
  __res; \
})

#define _syscall6(syscall_nr,a1,a2,a3,a4,a5,a6) \
({ \
  long __res; \
  __asm__ __volatile__ ( \
  "[--sp] = r3;\n\t"  \
  "r3=%5;\n\t" \
  "r2=%4;\n\t" \
  "r1=%3;\n\t" \
  "r0=%2;\n\t" \
  "p0=%1;\n\t" \
  "excpt 0;\n\t" \
  "%0=r0;\n\t" \
  "r3 = [sp++];\n\t" \
   : "=da" (__res) \
   : "d"  (syscall_nr), \
     "a"  ((long)(a1)), \
     "a"  ((long)(a2)), \
     "a"  ((long)(a3)), \
     "a"  ((long)(a4)) \
   : "CC", "R0","R1","R2","R3", "P0"); \
  __res; \
})

#endif



