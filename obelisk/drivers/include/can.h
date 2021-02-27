/*
 * $FILE: can.h
 *
 * ObelISK CAN Device Driver Header
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
 */
#ifndef _CAN_H_
#define _CAN_H_

/**
 * This file may also be included by some user library.
 */

typedef struct can_message {
   /* CAN message identifier */
   uint32_t id;

   /* CAN data stream */
   uint8_t data[8];
} canmsg_t;

#endif //_CAN_H_
