/*
 * $FILE: syslog.c
 *
 * Kernel log
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

#include <itron.h>
#include <linkage.h>
#include <syslog.h>
#include <string.h>
#include <tlsf.h>

__kobject static log_entry_t system_log[CONFIG_SYSLOG_SIZE];
static int log_index;

/*
 * init_syslog()
 *
 *    TODO: Consider passing system log size as a parameter
 */
void __init init_syslog(void)
{
   memset(system_log, 0, sizeof(system_log));
   log_index = 0;
}

/*
 * write_log()
 */
void write_log(int16_t event, int16_t arg1, int16_t arg2)
{
   log_entry_t *curr_entry = &system_log[log_index];
   curr_entry->time  = get_system_time();
   curr_entry->event = event;
   curr_entry->arg1  = arg1;
   curr_entry->arg2  = arg2;

   log_index = (log_index + 1) % CONFIG_SYSLOG_SIZE;
}

