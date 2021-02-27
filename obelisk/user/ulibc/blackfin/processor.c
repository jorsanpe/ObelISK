/*
 * $FILE: processor.c
 *
 * Blackfin processor management, user level
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

#include <itron.h>
#include <syscalls.h>
#include <arch/processor.h>
#include <kernel_inc/arch/arch_types.h>

uint32_t rea8_mmr(uint32_t mmr, uint8_t *p_val)
{
   int res;
   res = _syscall2(-TFN_REA8_MMR, mmr, p_val);
   return res;
}

uint32_t wri8_mmr(uint32_t mmr, uint8_t val)
{
   int res;
   res = _syscall2(-TFN_WRI8_MMR, mmr, val);
   return res;
}

uint32_t rea16_mmr(uint32_t mmr, uint16_t *p_val)
{
   int res;
   res = _syscall2(-TFN_REA16_MMR, mmr, p_val);
   return res;
}

uint32_t wri16_mmr(uint32_t mmr, uint16_t val)
{
   int res;
   res = _syscall2(-TFN_WRI16_MMR, mmr, val);
   return res;
}

uint32_t rea32_mmr(uint32_t mmr, uint32_t *p_val)
{
   int res;
   res = _syscall2(-TFN_REA32_MMR, mmr, p_val);
   return res;
}

uint32_t wri32_mmr(uint32_t mmr, uint32_t val)
{
   int res;
   res = _syscall2(-TFN_WRI32_MMR, mmr, val);
   return res;
}






