/*
 * $FILE: lwip_log.c
 *
 * lwIP log
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

#include <lwip_log.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/*
 * At this point, setting explicitly the section of the lwip log
 * is not necessary as it is stored where intended. Change this
 * if necessary, including the corresponding modifications on the
 * lds
 *
 * __attribute__((section(".lwiplog")))
 */
struct lwiplog_struct lwiplog[LWIPLOG_ENTRIES];
static int lwipidx;

//void print_log(const char *nmsg, int size)
void print_log(const char *format, ...)
{
   int n;

   /*va_list arg_ptr;
   va_start(arg_ptr, format);
   n = vsnprintf(lwiplog[lwipidx].msg, LWIPLOG_MSG_SIZE, format, arg_ptr);
   va_end(arg_ptr);*/

   strncpy(lwiplog[lwipidx].msg, format, LWIPLOG_MSG_SIZE);
   //strncpy(lwiplog[lwipidx].msg,nmsg, size);
   lwiplog[lwipidx].time = get_system_time();
   lwipidx = (lwipidx + 1) % LWIPLOG_ENTRIES;

   /*sprintf(lwiplog[lwipidx].msg, format);
   if (size < 256) {
      memcpy(lwiplog[lwipidx].msg, nmsg, size);
   }
   lwiplog[lwipidx].time = get_system_time();
   lwipidx = (lwipidx + 1) % LWIPLOG_ENTRIES;*/
}




