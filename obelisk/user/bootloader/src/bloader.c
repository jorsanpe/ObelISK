/*
 * $FILE: bloader.c
 *
 * Boot loader main routine. Reads and loads the elf
 * application container
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

#include <libelf.h>

extern void halt_system();
int valid_elf_image(void *addr);
unsigned int load_elf_image(void *addr);

/*
 * bmemcpy()
 *
 *    Boot mem copy routine
 */
void *bmemcpy(void *dst, const void *src, unsigned int count)
{
  register char *d=dst;
  register const char *s=src;

  ++count;
  while (--count) {
    *d = *s;
    ++d; ++s;
  }
  return dst;
}

/*
 * bmemset()
 *
 *    Boot mem set routine
 */
void *bmemset(void *dst, int val, unsigned int count)
{
   register char *d=dst;
   register char v=val;

   ++count;
   while (--count) {
      *d = v;
      ++d;
   }
}

/*
 * valid_elf_image()
 *
 *    Determines if the elf image is valid
 */
int valid_elf_image(void *addr)
{
   Elf32_Ehdr *ehdr; /* Elf header structure pointer */

   ehdr = (Elf32_Ehdr *) addr;

   if (!IS_ELF(*ehdr)) {
      return 0;
   }

   if (ehdr->e_type != ET_EXEC) {
      return 0;
   }

   return 1;
}

/*
 * load_elf_image()
 *
 *    Simple elf loader. Assumes that the image is valid and returns
 *    the entry address
 */
unsigned int load_elf_image(void *addr)
{
   Elf32_Ehdr *ehdr; /* Elf header structure pointer     */
   Elf32_Shdr *shdr; /* Section header structure pointer */
   unsigned char *strtab = 0;
   unsigned char *image;
   int i;

   ehdr = (Elf32_Ehdr *) addr;

   /* Find the section header string table for output info */
   shdr = (Elf32_Shdr *) (addr + ehdr->e_shoff + (ehdr->e_shstrndx
         * sizeof(Elf32_Shdr)));

   if (shdr->sh_type == SHT_STRTAB)
      strtab = (unsigned char *) (addr + shdr->sh_offset);

   /* Load each appropriate section */
   for (i = 0; i < ehdr->e_shnum; ++i) {
      shdr = (Elf32_Shdr *) (addr + ehdr->e_shoff + (i
            * sizeof(Elf32_Shdr)));

      if (!(shdr->sh_flags & SHF_ALLOC) || shdr->sh_addr == 0
            || shdr->sh_size == 0) {
         continue;
      }

      if (shdr->sh_type == SHT_NOBITS) {
         bmemset((void *) shdr->sh_addr, 0, shdr->sh_size);
      } else {
         image = (unsigned char *) addr + shdr->sh_offset;
         bmemcpy((void *) shdr->sh_addr, (const void *) image,
               shdr->sh_size);
      }
   }

   return ehdr->e_entry;
}

/*
 * boot_main()
 *
 *    Main function of the bootloader. Assumes that the
 *    external RAM has been initialized
 */
void boot_main(void)
{
   extern flash_entry_t *appPoolPtr; // Application pool pointer on flash memory
   unsigned int app_start;

   // Do we come from a warm or a cold reset?
   // In case of critical reset, boot recovery partition
   // Else, boot normally


   // Check elf image
   if (!valid_elf_image(appElfPtr)) {
      halt_system();
   }

   // Load elf image into memory
   app_start = load_elf_image(appElfPtr);

   // Jump to application code
   ((void(*)(void)) app_start)();
}
