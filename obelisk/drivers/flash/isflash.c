/*
 * $FILE: flash.c
 *
 * Intel Strata Flash Driver for CM-BF548
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

#include <flash.h>
#include <processor.h>

#define WRITE_FLASH(A, D)  (*(A) = (D)); SSYNC();

// Error codes
#define E_FLOK    0
#define E_FLTMO   -1
#define E_FLBAD   -2

/*
 * flash_open()
 *
 *    Open flash
 */
int32_t flash_open(int32_t omode)
{
   flash_setup();
}

/*
 * flash_setup()
 *
 *    Setup flash
 */
void flash_setup(void)
{

   bfin_write_EBIU_MODE(0x1); // Bank 0 async access mode
   bfin_write_EBIU_AMGCTL(0x3);

   // set port H MUX to configure PH8-PH13 as 1st Function (MUX = 00)
   // (bits 16-27 == 0) - Address signals A4-A9
   bfin_write_PORTH_FER(0x3F00);
   bfin_write_PORTH_MUX(0x0);
   bfin_write_PORTH_DIR_SET(0x3F00);

   // configure PI0-PI15 as A10-A21 respectively
   bfin_write_PORTI_FER(0xFFFF);
   bfin_write_PORTI_MUX(0x0);
   bfin_write_PORTI_DIR_SET(0xFFFF);
}

/*
 * flash_write_word()
 *
 *    Write a 16-bit word into the flash device
 *    @return:
 *       0x0080:  everything ok
 *       0xff00:  programming sequence timeout error
 *       0xee00:  value not programmed correctly
 */
int flash_write_word(unsigned short *waddr, unsigned short data)
{
   unsigned short status;
   unsigned long tmo;

   waddr += FLASH_START_ADDRESS;

   //WRITE_FLASH(waddr, 0x0050);
   WRITE_FLASH(waddr, 0x0040);
   WRITE_FLASH(waddr, data);

   tmo = 0;
   do {
      status = *waddr;
      SSYNC();
      ++tmo;
   } while (((status & 0x0080) == 0x00) && (tmo < FLASH_PROGRAM_TIMEOUT));

   status = E_FLOK;

   if (tmo >= FLASH_PROGRAM_TIMEOUT) {
      status = E_FLTMO;
   }

   WRITE_FLASH(waddr, 0xffff);

   if (status != 0xff00) {
      if (*waddr != data) {
         status = E_FLBAD;
      }
   }

   return status;
}

/*
 * flash_read_word()
 *
 *    Read a word from the flash memory
 */
unsigned short flash_read_word(unsigned short *raddr)
{
   unsigned short ret;

   raddr += FLASH_START_ADDRESS;
   ret = *raddr;
   SSYNC();
   return ret;
}

/*
 * flash_erase_sector()
 *
 *    Erase a sector from the flash
 */
int flash_erase_sector(unsigned short *eaddr)
{
   unsigned short status;
   unsigned long tmo;

   eaddr += FLASH_START_ADDRESS;

   //WRITE_FLASH(eaddr, 0x0050);
   WRITE_FLASH(eaddr, 0x0020);
   //WRITE_FLASH(eaddr, 0x00d0);

   tmo = 0;
   do {
      status = *eaddr;
      SSYNC();
      ++tmo;
   } while (((status & 0x0080) == 0x00) && (tmo < FLASH_PROGRAM_TIMEOUT));

   status = E_FLOK;

   if (tmo >= FLASH_ERASE_TIMEOUT) {
      status = E_FLTMO;
   }

   tmo = 0;
   do {
      *eaddr = 0xff;
      SSYNC();
      ++tmo;
   } while ((*eaddr != 0xffff) && (tmo < FLASH_PROGRAM_TIMEOUT));

   if (tmo >= FLASH_ERASE_TIMEOUT) {
      status = E_FLTMO;
   }

   return status;
}

/*
 * flash_write_buffer()
 *
 *    Burns the flash with a buffer
 */
int flash_write_buffer(void *dst, void *src, int size, int offset)
{
   unsigned short *p_dst = (unsigned short *)dst + offset;
   unsigned short *p_src = (unsigned short *)src;
   int i, sz, status;

   sz = size >> 1;
   for (i=0; i<sz; ++i) {
      // Write flash
      if ((status=flash_write_word(p_dst, *p_src)) != E_FLOK) {
         return status;
      }
      // Check writes
      if (flash_read_word(p_dst) != (*p_src)) {
         return E_FLBAD;
      }
      ++p_dst; ++p_src;
   }

   return E_FLOK;
}


