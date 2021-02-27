/*
 * $FILE: region.h
 *
 * Region manager
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

#ifndef _REGION_H_
#define _REGION_H_

#ifndef NULL
#define NULL 0
#endif

typedef struct _region {
    void *pstart;               // Pointer to the start of the assigned memory slot
    void *pfree;                // Pointer to the next free position to assign
    int size;                   // Size of the memory slot
    int used;                   // Bytes used
    int array_lock;             // 0: Not creating an array;  s: creating an array with element size s
} region_t;

// Function prototypes
region_t *new_region(void *pstart, int memsize);
void *ralloc(region_t *region, int size);
void *rcalloc(region_t *region, int size);
void rfree(region_t *region);
void *rnew_array(region_t *region, int itemsize, int *maxsize);
void rclose_array(region_t *region, int nitems);

#endif	//_REGION_H_



