/*
 * $FILE: region.c
 *
 * Region Manager
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

#include <region.h>
#include <string.h>

/*
 * new_region(
 */
region_t *new_region(void *pstart, int memsize)
{
   region_t *region;

   region = (region_t *)pstart;                 // Region structure
   region->pstart = pstart + sizeof(region_t);  // Memory start
   region->pfree = region->pstart;              // Free memory pointer
   region->size = memsize - sizeof(region_t);   // Total allocatable memory
   region->used = 0;                            // Total used memory
   region->array_lock = 0;                      // Array generation flag

   return region;
}

/*
 * ralloc()
 *
 *    Allocate a buffer from the regio
 */
void *ralloc(region_t *region, int size)
{
   int flags;
   int size_rounded;
   void *pmem;

   // An array is being generated
   if (region->array_lock != 0)
      return NULL;

   // Round size allocated to a 4 byte boundary
   size_rounded = (size + 0x3) & ~0x3;

   // Check if there's enough memory
   if (region->size < (region->used + size_rounded)) {
      return NULL;
   }

   pmem = region->pfree;
   region->pfree += size_rounded;
   region->used += size_rounded;

   return pmem;
}

/*
 * rcalloc()
 *
 *    Allocate and initialize a buffer from the regio
 */
void *rcalloc(region_t *region, int size)
{
   void *pbuf;
   
   if ((pbuf = ralloc(region, size)) != NULL) {
      memset(pbuf, 0, size);
      return pbuf;
   }
   return NULL;
}

/*
 * rfree()
 *
 *    Deallocate all the memory in the regio
 */
void rfree(region_t *region)
{
   region->pfree = region->pstart;
   region->used = 0;
}

/*
 * rnew_array()
 *
 *    Begins the construction of an array. We don't know a priori
 *    the size of the arra
 */
void *rnew_array(region_t *region, int itemsize, int *maxsize)
{
   int selem_round;

   // Another array is being generated
   if (region->array_lock != 0) {
      return NULL;
   }
   
   region->array_lock = itemsize;
   
   // Return maximum size when required
   if (maxsize != NULL) {
      *maxsize = region->size - region->used; // Maximum size that the array can grow
   }

   return region->pfree;
}

/*
 * rclose_array(
 */
void rclose_array(region_t *region, int nitems)
{
   // Update values
   region->used += region->array_lock * nitems;
   region->pfree += region->array_lock * nitems;

   // Round pointer to a 4-byte boundary
   region->pfree = (void *)(((unsigned long)region->pfree + 0x3) & ~0x3);

   // Unlock region
   region->array_lock = 0;
}
