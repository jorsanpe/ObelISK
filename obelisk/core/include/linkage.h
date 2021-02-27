/*
 * $FILE: linkage.h
 *
 * Assembly macros
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

#ifndef _LINUX_LINKAGE_H
#define _LINUX_LINKAGE_H

#ifndef _KERNEL_
#error   Internal file. Do not include it in your sources.
#endif

#ifndef __ASSEMBLY__

#define __init __attribute__((section(".init.text")))
#define __kobject __attribute__((section(".kobjects")))
#define __device __attribute__((section(".devices")))
#define __cplbtab __attribute__((section(".cplbtab")))
#define __dnocache __attribute__((section(".dnocache")))
#define __inocache __attribute__((section(".inocache")))

#endif

#ifdef __ASSEMBLY__

#ifndef ENTRY
#define ENTRY(name) \
  .globl name; \
  .align 8; \
  name:
#endif

#ifndef WEAK
#define WEAK(name)      \
   .weak name;    \
   name:
#endif

#ifndef END
#define END(name) \
  .size name, .-name
#endif

/* If symbol 'name' is treated as a subroutine (gets called, and returns)
 * then please use ENDPROC to mark 'name' as STT_FUNC for the benefit of
 * static analysis tools such as stack depth analyzer.
 */
#ifndef ENDPROC
#define ENDPROC(name) \
  .type name, @function; \
  END(name)
#endif

#endif
#endif



