/*
 * $FILE: flash.h
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

#ifndef _FLASH_H_
#define _FLASH_H_

// CM-BF548 oscillator frequency
#define PROC_CLOCK_FREQ_IN          25          // [MHz]

// Flash configuration
#define FLASH_INTEL_STRATA                      // intel strata compatible flash
#define FLASH_START_ADDRESS         0x20000000
#define FLASH_NUM_SECTORS           256
#define FLASH_SECTOR_SIZE           0x8000
#define FLASH_NOF_BANKS             1
#define FLASH_BANK_SIZE             0x800000    // 8MB

#define FLASH_PROGRAM_TIMEOUT 		0x0ffffff
#define FLASH_ERASE_TIMEOUT			0x0ffffff
#define FLASH_UNLOCK_TIMEOUT			0x0ffffff

#define FLASH_DEBUG_LEVEL				0
#define FLASH_MAX_NOF_BANK_SELECT	4

#define FLASH_LAST_SECTOR				(FLASH_SECTOR_SIZE * (FLASH_NUM_SECTORS - 1))
#define FLASH_SIZE						(FLASH_SECTOR_SIZE * FLASH_NUM_SECTORS)

void flash_setup(void);
int flash_write_word(unsigned short *waddr, unsigned short data);
unsigned short flash_read_word(unsigned short *raddr);
int flash_erase_sector(unsigned short *eaddr);
int flash_write_buffer(void *dst, void *src, int size, int offset);

#endif //_FLASH_H_

