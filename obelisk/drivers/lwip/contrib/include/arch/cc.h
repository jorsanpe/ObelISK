/*
 * $FILE: cc.h
 *
 * lwIP processor and compiler types
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

#ifndef _LWIP_CC_H_
#define _LWIP_CC_H_

#define BYTE_ORDER LITTLE_ENDIAN

#define U16_F  "hu"
#define S16_F  "hd"
#define X16_F  "hx"
#define U32_F  "u"
#define S32_F  "d"
#define X32_F  "x"
#define SZT_F  "uz"

#define SYS_ARCH_PROTECT(x)      __asm__ __volatile__("cli %0;\n\t": "=d" (x))
#define SYS_ARCH_UNPROTECT(x)    __asm__ __volatile__("sti %0;\n\t": : "d" (x))
#define SYS_ARCH_DECL_PROTECT(x) unsigned int x;

#include <lwip_log.h>

// Platform diagnostic messages
#define LWIP_DEBUG      // Enable debudding messages
#define LWIP_PLATFORM_DIAG(...)    //print_log(__VA_ARGS__) //print_log(x, sizeof(x))
#define LWIP_PLATFORM_ASSERT(...)  //print_log(__VA_ARGS__) //print_log(x, sizeof(x))

typedef char            s8_t;
typedef short           s16_t;
typedef int             s32_t;
typedef unsigned char   u8_t;
typedef unsigned short  u16_t;
typedef unsigned int    u32_t;
typedef unsigned long   mem_ptr_t;    // Due to pointer arithmetics, this is of integer type

#define PACK_STRUCT_FIELD(x)     x
#define PACK_STRUCT_STRUCT       __attribute__ ((__packed__))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/*#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct <structure_name> {
    PACK_STRUCT_FIELD(<type> <field>);
    PACK_STRUCT_FIELD(<type> <field>);
    <...>
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif*/


#endif   //_LWIP_CC_H_

