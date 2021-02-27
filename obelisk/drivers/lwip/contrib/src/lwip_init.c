/*
 * $FILE: lwip_init.c
 *
 * lwIP initialization procedure
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

#include <autoconf.h>
#include <linkage.h>
#include <arch/sys_arch.h>
#include <lwip/ip.h>
#include <lwip/tcpip.h>
#include <lwip/netif.h>
#include <netif/etharp.h>
#include <lwip/mem.h>
#include <ethernetif.h>

#define DEFAULT_IP            0xC0A80202
#define DEFAULT_NETMASK       0xFFFFFF00
#define DEFAULT_GATEWAY       0xC0A80201

#define E_PARSEIP             0xFFFFFFFF

// Parsing macros
#define IS_NUMBER(X)          ((unsigned int)(X-'0') < 10u)
#define GET_NUMBER_VALUE(X)   (X-'0')
#define IS_HEX_LETTER_UP(X)   ((unsigned int)(X-'A') < 6u)
#define GET_HEX_LVAL_UP(X)    (X-'A'+10)
#define IS_HEX_LETTER_LO(X)   ((unsigned int)(X-'a') < 6u)
#define GET_HEX_LVAL_LO(X)    (X-'a'+10)


#ifndef CONFIG_USE_DHCP
/* (Manual) Addresses configured */
char mac_addr_string[] = CONFIG_MAC_ADDRESS; // Default: "EE:FF:C0:A8:01:01"
char ip_addr_string[] = CONFIG_IP_ADDRESS;
char netmask_string[] = CONFIG_NETMASK;
char gateway_string[] = CONFIG_GATEWAY;
#endif

sys_thread_t ethif_thread;

/*
 * parse_ip_address()
 *
 *    Parse IP address
 */
static inline uint32_t parse_ip_address(char *ip_string)
{
   int i, k;
   int tmpval, ipaddr;

   k = ipaddr = 0;

   for (i=0; i<4; ++i) {
      tmpval = 0;
      while (ip_string[k] != '.' && ip_string[k] != '\0') {
         if (IS_NUMBER(ip_string[k])) {
            tmpval = tmpval*10 + GET_NUMBER_VALUE(ip_string[k]);
         } else {
            return E_PARSEIP;
         }
         if (tmpval > 255) {
            return E_PARSEIP;
         }
         ++k;
      }
      ++k;
      ipaddr = ipaddr | (tmpval << (i*8));
   }

   return ipaddr;
}

/*
 * parse_hex_char()
 *
 *    Returns the integer value of an hexadecimal character
 */
static inline int parse_hex_char(char h)
{
   if (IS_NUMBER(h)) {
      return GET_NUMBER_VALUE(h);
   }
   if (IS_HEX_LETTER_UP(h)) {
      return GET_HEX_LVAL_UP(h);
   }
   if (IS_HEX_LETTER_LO(h)) {
      return GET_HEX_LVAL_LO(h);
   }
   return -1;
}

/*
 * parse_mac_address()
 *
 *    Parse the MAC address. Should this string contain any error,
 *    the system will set a default MAC. It is assumed that the
 *    size of the string is checked on the caller function
 */
static inline void parse_mac_address(char *mac_string, unsigned char *hwaddr)
{
   int k, i, val;

   // Assign MAC values while checking them
   k = 0;
   for (i = 0; i < 17; i += 3) {
      if ((val = parse_hex_char(mac_string[i])) == -1) {
         goto default_mac;
      }
      hwaddr[k] = val << 4;
      if ((val = parse_hex_char(mac_string[i+1])) == -1) {
         goto default_mac;
      }
      hwaddr[k] += val;
      k++;
   }

   return;

   // Set default MAC address
default_mac:
   hwaddr[0] = 0xEE;
   hwaddr[1] = 0xFF;
   hwaddr[2] = 0xC0;
   hwaddr[3] = 0xA8;
   hwaddr[4] = 0x01;
   hwaddr[5] = 0x01;

   return;
}

/*
 * init_lwip_stack()
 *
 *    lwIP stack initialization
 */
__init void init_lwip_stack()
{
   struct netif *netif;
   struct ip_addr ipaddr, netmask, gway;

   // Init lwip modules
   tcpip_init(0, 0);

#ifdef CONFIG_USE_DHCP

#else
   // Manual configuration. Parse addresses given by user
   if ((ipaddr.addr = parse_ip_address(ip_addr_string)) == E_PARSEIP) {
      ipaddr.addr = DEFAULT_IP;
   }
   if ((netmask.addr = parse_ip_address(netmask_string)) == E_PARSEIP) {
      netmask.addr = DEFAULT_NETMASK;
   }
   if ((gway.addr = parse_ip_address(gateway_string)) == E_PARSEIP) {
      gway.addr = DEFAULT_GATEWAY;
   }
#endif

   // Allocate memory for the network interface structure
   if ((netif=sys_malloc(sizeof(struct netif))) == NULL) {
      // Log bad lwIP initialization
      return;
   }

   // User defined MAC address
   parse_mac_address(mac_addr_string, &netif->hwaddr[0]);
   netif->hwaddr_len = 6;

   // Add the interface and set as default.
   netif_add(netif, &ipaddr, &netmask, &gway, NULL, ethernetif_init, tcpip_input);
   netif_set_default(netif);
   netif_set_up(netif);

   // start the ARP update thread
   etharp_timer_init();
}






