/*
 * $FILE: ethernetif.h
 *
 * Ethernet interface header
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

#ifndef _ETHERNETIF_H_
#define _ETHERNETIF_H_

// Network interface description
#define IFNAME0 'e'
#define IFNAME1 't'

// Maximum Transfer Unit
#define MTU 1518

extern err_t ethernetif_init(struct netif *netif);
extern void ethernetif_input(void);
extern void etharp_timer_init(void);

#endif //_ETHERNETIF_H_
