/*
 * $FILE: logger.c
 *
 * ObelISK system log interpreter
 *
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
 */

#include <stdlib.h>
#include <logger.h>
#include <kernel_inc/autoconf.h>

typedef struct system_log_struct {
   unsigned int time;
   short event;
   short arg1, arg2, arg3;
} syslog_t;

typedef struct logger_entry {
   unsigned int thread;
   syslog_t lentry;
} logger_t;

int read_log(char *buffer)
{
   syslog_t *syslog;
   logger_t *log_table;
   int i, j, k, found;
   int n, thread, ctime;
   char *pbuf = buffer;

   if (!buffer) {
      return -1;
   }

   log_table = malloc(CONFIG_SYSLOG_SIZE*sizeof(logger_t));
   syslog = malloc(CONFIG_SYSLOG_SIZE*sizeof(syslog_t));

   k = read_system_log(syslog);
   if (k < 0) {
      return -1;
   }

   // Get the starting item
   for (i=0; i<(k-1); i++) {
      if (syslog[i+1].time < syslog[i].time) {
         break;
      }
   }

   //ctime = get_system_time();

   // Find first active thread
   n = i;
   for (j=0; j<k; ++j) {
      n = (n + 1) % k;
      if (syslog[n].event == -0x01) {
         thread = syslog[n].arg1;
         break;
      }
   }

   // Sort by time
   for (j=0; j<k; j++) {
      i = (i + 1) % k;
      log_table[j].lentry = syslog[i];
      log_table[j].thread = thread;
      if (syslog[i].event == -0x01) {
         thread = syslog[i].arg2;
      }
   }

   // Print log
   sprintf(buffer, "<obeliskLog systemTime=\"%d\">\n", ctime);

   for (i=0; i<k; i++) {
      for (j=0; j<CODE_TABLE_ENTRIES; ++j) {
         if (log_table[i].lentry.event == code_table[j].fncode) {

            sprintf(buffer, "<event id=\"%d\" description=\"%s\">\n", log_table[i].lentry.event, code_table[j].event);
            sprintf(buffer, "\t<time value=\"%d\"/>\n", log_table[i].lentry.time);
            sprintf(buffer, "\t<context thread=\"%d\"/>\n", log_table[i].thread);

            sprintf(buffer, "\t<arg desc=\"%s\" value=\"%d\"/>\n", code_table[j].arg1, log_table[i].lentry.arg1);

            if (strcmp(code_table[j].arg2, "") != 0) {
               sprintf(buffer, "\t<arg desc=\"%s\" value=\"%d\"/>\n", code_table[j].arg2, log_table[i].lentry.arg2);
            }
            sprintf(buffer, "</event>\n");
            break;
         }
      }
   }

   sprintf(buffer, "</obeliskLog>");

   free(log_table);
   free(syslog);

   return 0;
}



