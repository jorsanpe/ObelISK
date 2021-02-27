/*
 * $FILE: sdram_init.h
 *
 * Blackfin SDRAM initialization
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

#ifndef _SDRAM_INIT_H_
#define _SDRAM_INIT_H_

#include <arch/processor.h>
#include <arch/timer.h>

/* Macros for EBIU_DDRCTL0 */
#define SET_tRC(x)      (((x)&0xF) << 26)
#define SET_tRAS(x)     (((x)&0xF) << 22)
#define SET_tRP(x)      (((x)&0xF) << 18)
#define SET_tRFC(x)     (((x)&0xF) << 14)
#define SET_tREFI(x)    ((x)&0x3FFF)

/* Macros for EBIU_DDRCTL1 */
#define SET_tWTR(x)     (((x)&0xF) << 28)
#define SET_tWR(x)      (((x)&0x3) << 8)
#define SET_tMRD(x)     (((x)&0xF) << 4)
#define SET_tRCD(x)     ((x)&0xF)

/* Values for EBIU_DDRCTL0 */
#define DDR_tRC         SET_tRC(8)
#define DDR_tRAS        SET_tRAS(6)
#define DDR_tRP         SET_tRP(2)
#define DDR_tRFC        SET_tRFC(10)
#define DDR_tREFI       SET_tREFI(0x1041)

/* Values for EBIU_DDRCTL1 */
#define DDR_tRCD        SET_tRCD(2)
#define DDR_tWTR        SET_tWTR(2)
#define DDR_tMRD        SET_tMRD(2)
#define DDR_tWR         SET_tWR(2)
#define DDR_SIZE        DEVSZ_512
#define DDR_WIDTH       DEVWD_16
#define DDR_CL          CL_2

/*
#define MIN_DDR_SCLK(x) (x*(CONFIG_SCLK/1000/1000)/1000 + 1)
#define MAX_DDR_SCLK(x) (x*(CONFIG_SCLK/1000/1000)/1000)
#define DDR_CLK_HZ(x)   (1000*1000*1000/x)

#define DDR_tRC         DDR_TRC(MIN_DDR_SCLK(60))
#define DDR_tRAS        DDR_TRAS(MIN_DDR_SCLK(42))
#define DDR_tRP         DDR_TRP(MIN_DDR_SCLK(15))
#define DDR_tRFC        DDR_TRFC(MIN_DDR_SCLK(72))
#define DDR_tREFI       DDR_TREFI(MAX_DDR_SCLK(7800))

#define DDR_tRCD        DDR_TRCD(MIN_DDR_SCLK(15))
#define DDR_tWTR        DDR_TWTR(1)
#define DDR_tMRD        DDR_TMRD(MIN_DDR_SCLK(12))
#define DDR_tWR         DDR_TWR(MIN_DDR_SCLK(15))
*/

#define mem_DDRCTL0     (DDR_tRP | DDR_tRAS | DDR_tRC | DDR_tRFC | DDR_tREFI)
#define mem_DDRCTL1     (DDR_DATWIDTH | DDRDEVWIDTH | EXTBANK_2 | DDR_SIZE | DDR_WIDTH | DDR_tWTR \
                        | DDR_tMRD | DDR_tWR | DDR_tRCD)
#define mem_DDRCTL2     (DDR_CL | BURSTLENGTH)


#endif   //_SDRAM_INIT_H_
