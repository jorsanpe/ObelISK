/*
 * $FILE: processor.c
 *
 * Blackfin processor management
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
#include <processor.h>
#include <types.h>

uint32_t rea8_mmr_sys(uint32_t mmr, uint8_t *p_val)
{
   if (!mmr) {
      return E_PAR;
   }

   __hw_cli();
   *p_val = bfin_read8(mmr);
   __hw_sti();

   return E_OK;
}

uint32_t wri8_mmr_sys(uint32_t mmr, uint8_t val)
{
   if (!mmr) {
      return E_PAR;
   }

   __hw_cli();
   bfin_write8(mmr, val);
   __hw_sti();

   return E_OK;
}

uint32_t rea16_mmr_sys(uint32_t mmr, uint16_t *p_val)
{
   if (!mmr) {
      return E_PAR;
   }

   __hw_cli();
   *p_val = bfin_read16(mmr);
   __hw_sti();

   return E_OK;
}

uint32_t wri16_mmr_sys(uint32_t mmr, uint16_t val)
{
   if (!mmr) {
      return E_PAR;
   }

   __hw_cli();
   bfin_write16(mmr, val);
   __hw_sti();

   return E_OK;
}

uint32_t rea32_mmr_sys(uint32_t mmr, uint32_t *p_val)
{
   if (!mmr) {
      return E_PAR;
   }

   __hw_cli();
   *p_val = bfin_read32(mmr);
   __hw_sti();

   return E_OK;
}

uint32_t wri32_mmr_sys(uint32_t mmr, uint32_t val)
{
   if (!mmr) {
      return E_PAR;
   }

   __hw_cli();
   bfin_write32(mmr, val);
   __hw_sti();

   return E_OK;
}



