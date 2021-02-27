/*
 * $FILE: itron.h
 *
 * ITRON includes
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

#ifndef _ITRON_H_
#define _ITRON_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <autoconf.h>
/*
 *  Include the basic requirements
 */
#include <itronsys/types.h>
#include <itronsys/status.h>
#include <itronsys/eventflags.h>
#include <itronsys/fmempool.h>
#include <itronsys/intr.h>
#include <itronsys/mbox.h>
#include <itronsys/msgbuffer.h>
#include <itronsys/sysmgmt.h>
#include <itronsys/network.h>
#include <itronsys/port.h>
#include <itronsys/semaphore.h>
#include <itronsys/task.h>
#include <itronsys/time.h>
#include <itronsys/vmempool.h>
#include <itronsys/fncodes.h>

#endif //_ITRON_H_

