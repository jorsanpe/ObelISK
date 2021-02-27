/*
 * $FILE: tbitmap.c
 *
 * Tree bitmaps
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

/*
#include <tbitmap.h>

uint32_t tbitmap_init(tbitmap_t *tbmap)
{
   uint32_t i;

   if (ONES(tbmap->nbits) > 0) {
      return -1;
   }

   if (array == NULL) {
      return -1;
   }
   tbmap->flbmap = setbitmap();
   for (i=0; i<tbmap->nbits>>5; i++) {
      tbmap->slbmap[i] = setbitmap();
   }

   return 0;
}

uint32_t tbitmap_ffs(tbitmap_t tbmap)
{
   int32_t n;

   if (n = _ffs(tbmap->flbmap) == -1) {
      return -1;
   }
   return (n+1) * _ffs(tbpam->slbmap[n]);
}

uint32_t tbitmap_ffc(tbitmap_t tbmap)
{
   int32_t n, i;


}

bitmap_t tbitmap_setbit(tbitmap_t tbmap, uint32_t bit)
{
   tbmap->slbmap[bit>>5] = clearbit(
}

bitmap_t tbitmap_clearbit(tbitmap_t tbmap, uint32_t bit)
{

}
*/
