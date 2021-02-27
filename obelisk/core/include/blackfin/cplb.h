/*
 * File:         include/asm-blackfin/cplb.h
 * Based on:     include/asm-blackfin/mach-bf537/bf537.h
 * Author:       Robin Getz <rgetz@blackfin.uclinux.org>
 *
 * Created:      2000
 * Description:  Common CPLB definitions for CPLB init
 *
 * Modified:
 *               Copyright 2004-2007 Analog Devices Inc.
 *
 * Bugs:         Enter bugs at http://blackfin.uclinux.org/
 *
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
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _CPLB_H
#define _CPLB_H

#include <arch/anomaly.h>
#include <arch/processor.h>
#include <stddef.h>

#if ANOMALY_05000158
#define ANOMALY_05000158_WORKAROUND             0x200
#else
#define ANOMALY_05000158_WORKAROUND             0x0
#endif

#define MAX_CPLBS 16

#define SIZE_1K 0x00000400      /* 1K */
#define SIZE_4K 0x00001000      /* 4K */
#define SIZE_1M 0x00100000      /* 1M */
#define SIZE_4M 0x00400000      /* 4M */

#ifndef __ASSEMBLY__
static inline void disable_dcplb(void)
{
   unsigned long ctrl;
   ctrl = bfin_read_DMEM_CONTROL();
   ctrl &= ~ENDCPLB;
   /* CSYNC to ensure load store ordering */
   CSYNC();
   bfin_write_DMEM_CONTROL(ctrl);
   SSYNC();
}

static inline void enable_dcplb(void)
{
   unsigned long ctrl;
   ctrl = bfin_read_DMEM_CONTROL();
   ctrl |= ENDCPLB;
   /* CSYNC to ensure load store ordering */
   CSYNC();
   bfin_write_DMEM_CONTROL(ctrl);
   SSYNC();
}

static inline void disable_icplb(void)
{
   unsigned long ctrl;
   ctrl = bfin_read_IMEM_CONTROL();
   ctrl &= ~ENICPLB;
   /* CSYNC to ensure load store ordering */
   CSYNC();
   bfin_write_IMEM_CONTROL(ctrl);
   SSYNC();
}

static inline void enable_icplb(void)
{
   unsigned long ctrl;
   ctrl = bfin_read_IMEM_CONTROL();
   ctrl |= ENICPLB;
   /* CSYNC to ensure load store ordering */
   CSYNC();
   bfin_write_IMEM_CONTROL(ctrl);
   SSYNC();
}

struct cplb_entry {
   uint32_t addr, data;
};

#endif      /* __ASSEMBLY__ */
#endif      /* _CPLB_H */
