/*
 * $FILE: task_example.c
 *
 * A simple task creation example.
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

#include <itron.h>
#include <kernel_inc/arch/delay.h>
#include <arch/processor.h>

int main(void)
{
   W i;
   UH val=0;

   wri16_mmr(PORTH_DIR_SET, PH5);

   while (1) {
      tslp_tsk(500);
      //udelay(500000);
      rea16_mmr(PORTH, &val);
      wri16_mmr(PORTH, val ^ PH5);
   }

   ext_tsk();
}
