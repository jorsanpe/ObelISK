/*
 * $FILE: bf548.lds
 *
 * Linker Description Script for ObelISK based applications
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
 *
 */

#include <autoconf.h>

OUTPUT_FORMAT("elf32-bfin")
/*OUTPUT_ARCH(bfin)*/
ENTRY(__start)

MEMORY
{
   MEM_RAM_BANK0_KERNEL : ORIGIN = 0x00000000, LENGTH = 0x400000
   MEM_RAM_BANK0_USER   : ORIGIN = 0x00400000, LENGTH = 0xC00000
   MEM_RAM_BANK1        : ORIGIN = 0x01000000, LENGTH = 0x1000000
   MEM_RAM_BANK2        : ORIGIN = 0x02000000, LENGTH = 0x1000000
   MEM_RAM_BANK3        : ORIGIN = 0x03000000, LENGTH = 0x1000000
   MEM_L2               : ORIGIN = 0xFEB00000, LENGTH = 0x20000
   MEM_L1_DATA_A        : ORIGIN = 0xFF800000, LENGTH = 0x8000
   MEM_L1_DATA_B        : ORIGIN = 0xFF900000, LENGTH = 0x8000
   MEM_L1_CODE          : ORIGIN = 0xFFA00000, LENGTH = 0x12000
   MEM_L1_CODE_CACHE    : ORIGIN = 0xFFA10000, LENGTH = 0x4000
   MEM_L1_SCRATCH       : ORIGIN = 0xFFB00000, LENGTH = 0x1000
}

SECTIONS
{
   /*
    * KERNEL LINKS
    *
    *    Do not modify unless you know what you're doing
    */
   . = ORIGIN(MEM_L1_CODE);
   .obk_inocache : {
      _sl1kcode = .;

      obelisk.o (.init.text)
      obelisk.o (.irqtab)
      obelisk.o (.inocache)
      obelisk.o (l1.text)

      _el1kcode = ALIGN(0x1000);
   }
   _l1kcodesz = _el1kcode - _sl1kcode;

   . = ORIGIN(MEM_L1_DATA_A);
   .obk_dnocache : {
      _sl1kdata = .;

      _memtab = .;
      obelisk.o(.memtab)
      _memtabsz = . - _memtab;

      obelisk.o (.dnocache)
      obelisk.o (.extable)

      _el1kdata = ALIGN(0x1000);
   }
   _l1kdatasz = _el1kdata - _sl1kdata;

   /* We are allocating minimum 32 KB on L2 for kernel usage */
   . = ORIGIN(MEM_L2);
   .obk_objects : {
      _sl2kdata = .;

      __initcalls_start = .;
      obelisk.o(.initcalls0)
      obelisk.o(.initcalls1)
      obelisk.o(.initcalls2)
      __initcalls_end = .;

      obelisk.o (.kobjects)

      __device_pool = .;
      obelisk.o (.devices)
      __devices_end = .;

      obelisk.o (.syscalltab)
      obelisk.o (.tring)

      _el2kdata = ALIGN(0x1000);
      _el2kdata_align = ALIGN(0x8000);
   }
   /* Kernel Fast stack allocation on L2 */
   __kfastpool_base = _el2kdata;
   __kfastpool_size = _el2kdata_align - __kfastpool_base;

   _l2kdatasz = _el2kdata - _sl2kdata;


   . = ORIGIN(MEM_RAM_BANK0_KERNEL);
   .obk : {
      obelisk.o (.text)

      . = ALIGN(0x1000);

      obelisk.o (.data)
      obelisk.o (.rodata)

      . = ALIGN(0x1000);
   }

   .bss ALIGN(0x1000) : {
      _skbss = .;
      obelisk.o (.bss)
      *(.bss)
      _ekbss = ALIGN(0x1000);
   }

   __syspool_base = ALIGN(0x1000);
   __syspool_size = ORIGIN(MEM_RAM_BANK0_KERNEL) + LENGTH(MEM_RAM_BANK0_KERNEL) - __syspool_base;


   /*
    * USER LINKS
    *
    *    Link your code and data on marked places
    */
    . = _sl1kdata + _l1kdatasz;
   .l1_data_a : {
      _sl1udata = .;
      *(.l1_data_a)


      /* Link user L1_DATA_A here */


      _el1udata = ALIGN(0x1000);
   }
   _l1udatasz = ORIGIN(MEM_L1_DATA_A) + LENGTH(MEM_L1_DATA_A) - _sl1udata;
   /*ASSERT(ABSOLUTE(_el1udata)<0,"Test de Error");*/

   . = ORIGIN(MEM_L1_DATA_B);
   .l1_data_b : {
      *(.l1_data_b)


      /* Link user L1_DATA_B here */

   }

   . = _sl1kcode + _l1kcodesz;
   .l1_code : {
      _sl1ucode = .;
      *(.l1_code)

      /* Link user L1_CODE here */

      _el1ucode = ALIGN(0x1000);
   }
   _l1ucodesz = ORIGIN(MEM_L1_CODE) + LENGTH(MEM_L1_CODE) - _sl1ucode;


   . = _sl2kdata + _l2kdatasz;
   .l2_data : {
      _sl2udata = .;
      *(.l2_data)

      /* Link user L2 DATA here */

      _el2udata = ALIGN(0x1000);
   }
   _l2udatasz = ORIGIN(MEM_L2) + LENGTH(MEM_L2) - _el2udata;

   . = ORIGIN(MEM_RAM_BANK0_USER);
   .text : {
      *(.text)
   }

   .data ALIGN(0x1000) : {
      *(.data)
   }

   /*.bss ALIGN(0x1000) : {
      _subss = .;
      *(.bss)
      _eubss = .;
   }*/

   /* User External RAM Bank 0 Code and Data */
   .ram_bank0 : {
      *(.ram_bank0)
   }

   __userpool1_base = .;
   __userpool1_size = LENGTH(MEM_RAM_BANK0_USER) - (. - ORIGIN(MEM_RAM_BANK0_USER)) - 4;

   . = ORIGIN(MEM_RAM_BANK1);

   /* User External RAM Bank 1 Code and Data */
   .ram_bank1 : {
      *(.ram_bank1)
   }

   __userpool2_base = .;
   __userpool2_size = LENGTH(MEM_RAM_BANK1) - (. - ORIGIN(MEM_RAM_BANK1)) - 4;

   . = ORIGIN(MEM_RAM_BANK2);

   /* User External RAM Bank 2 Code and Data */
   .ram_bank2 : {
      *(.ram_bank2)
   }

   __userpool3_base = .;
   __userpool3_size = LENGTH(MEM_RAM_BANK2) - (. - ORIGIN(MEM_RAM_BANK2)) - 4;

   . = ORIGIN(MEM_RAM_BANK3);

   /* User External RAM Bank 3 Code and Data */
   .ram_bank3 : {
      *(.ram_bank3)
   }

   __userpool4_base = .;
   __userpool4_size = LENGTH(MEM_RAM_BANK3) - (. - ORIGIN(MEM_RAM_BANK3)) - 4;
}

