/*
 * $FILE: ints.c
 *
 * ObelISK interrupt handling
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
#include <processor.h>
#include <linkage.h>
#include <stddef.h>
#include <arch/ints.h>

// Declared on entry.S
extern void schedule_from_int();
extern void exception_entry();
extern void hardware_error();
extern void unhandled_interrupt();
extern void interrupt_vector_7();
extern void interrupt_vector_8();
extern void interrupt_vector_9();
extern void interrupt_vector_10();
extern void interrupt_vector_11();
extern void interrupt_vector_12();
extern void interrupt_vector_13();

__dnocache uint32_t interrupt_flags;
__dnocache uint32_t int_nest_count;
__dnocache uint32_t need_resched;
__dnocache void *interrupt_table[7];

/*
 * evt_ivg_install()
 *
 *    This is the low level function for blackfin architecture
 *    which writes to the MMRs
 */
__inocache void evt_ivg_install(uint32_t inhno, void *phdr)
{
   uint32_t *evt_ivg;

   // Get the address of the interrupt controller register
   evt_ivg = (uint32_t *)(EVT0 + (inhno << 2));

   // Install the isr
   *evt_ivg = (uint32_t)phdr;
}

/*
 * install_handler()
 */
void install_handler(uint32_t inhno, void *phdr)
{
   interrupt_table[inhno-7] = phdr;
}

/*
 * release_handler()
 */
void release_handler(uint32_t inhno)
{
   interrupt_table[inhno-7] = unhandled_interrupt;
}

/*
 * trap_c()
 *
 *    C Exception handler
 *
 *    TODO: Call the user handler for this exception
 */
int trap_c(cause)
{
   return 0;
}

/*
 * install_default_handlers()
 */
__init void install_default_handlers(void)
{
   int i;

   // Interrupt flags
   int_nest_count = 0;
   need_resched = 0;

   // Set default handlers
   evt_ivg_install(7, interrupt_vector_7);
   evt_ivg_install(8, interrupt_vector_8);
   evt_ivg_install(9, interrupt_vector_9);
   evt_ivg_install(10, interrupt_vector_10);
   evt_ivg_install(11, interrupt_vector_11);
   evt_ivg_install(12, interrupt_vector_12);
   evt_ivg_install(13, interrupt_vector_13);

   for (i=0; i<7; i++) {
      interrupt_table[i] = unhandled_interrupt;
   }

   bfin_write_EVT3(exception_entry);

   // Software interrupt
   evt_ivg_install(14, schedule_from_int);
   mask_interrupt(14);

   bfin_write_ILAT(-1);
}

/*
 * def_inh_sys()
 */
ER def_inh_sys(UINT inhno, T_DINH *pk_dinh)
{
   // Check invalid interrupt vectors
   if (inhno < 7 || inhno > 13) {
      return E_PAR;
   }

   // Check invalid pk_dinh pointer
   if ((uint32_t)pk_dinh & 0x3) {
      return E_PAR;
   }
   
   __hw_cli();

#ifdef CONFIG_SYSLOG_ACTIVE
      write_log(TFN_DEF_INH, inhno, E_OK);
#endif

   // If pk_dinh==NULL then release handler (see ITRON spec)
   if (!pk_dinh) {
      release_handler(inhno);
      __hw_sti();

      // Interrupt mask must be modified out of critical regions
      unmask_interrupt(inhno);
   } else {
      // By default the interrupt mask is set
      install_handler(inhno, pk_dinh->inthdr);
      __hw_sti();

      // Interrupt mask must be modified out of critical regions
      mask_interrupt(inhno);
   }

   return E_OK;
}

/*
 * dis_int_sys()
 */
/*ER dis_int_sys()
{

}*/

