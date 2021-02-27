/*
 * $FILE: arch_types.h
 *
 * Types defined by the architecture
 *
 * $VERSION: 1.0
 *
 * Modified: Jordi Sánchez <jorsanp3@upvnet.upv.es>
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

#ifndef _ARCH_TYPES_H_
#define _ARCH_TYPES_H_

#ifndef __ASSEMBLY__

#ifndef NULL
#define NULL   0x00000000
#endif

typedef char                   int8_t;
typedef short                  int16_t;
typedef int                    int32_t;
typedef long long int          int64_t;
typedef unsigned char          uint8_t;
typedef unsigned short         uint16_t;
typedef unsigned long          uint32_t;
typedef unsigned long long int uint64_t;
typedef unsigned int           size_t;

typedef uint64_t hwtime_t;
typedef uint64_t duration_t;
typedef uint64_t time_t;


#endif //__ASSEMBLY__
#endif
