/*
 * $FILE: init.h
 *
 * ObelISK initialization
 *
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
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

#ifndef _INIT_H_
#define _INIT_H_

#include <linkage.h>

typedef int (*initcall_t)(void);

#define __define_initcall(fn, lev) \
      __attribute__((__section__(".initcalls"#lev))) static initcall_t __initcall_##fn = fn

#define core_initcall(fn) \
		__define_initcall(fn, 0)

#define device_initcall(fn) \
      __define_initcall(fn, 2)

//#define logdev_initcall(fn) \
		__define_initcall(fn, 1)

//#define physdev_initcall(fn) \
		__define_initcall(fn, 2)

#endif //_INIT_H_

