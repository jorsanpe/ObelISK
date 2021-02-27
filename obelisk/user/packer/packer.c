/*
 * $FILE: packer.c
 *
 * Pack several files to produce an output image to burn on
 * Blackfin flash memory
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libelf.h"

#define max(a, b) (a > b ? a : b)
#define ROUND4(X) (((X) + 0x3) & ~0x3)

static inline int get_file_size(FILE *stream)
{
   int size;

   fseek(stream, 0L, SEEK_END);
   size = ftell(stream);
   fseek(stream, 0L, SEEK_SET);

   return size;
}

static int add_file(unsigned char *image, const char *name, int type)
{
   FILE *file;
   int size, i;
   char *digest;
   flash_entry_t *fl_entry;

   // Read binary file to memory
   file = fopen(name, "rb");
   size = get_file_size(file);
   digest = (unsigned char *)calloc(1, size);
   fread(digest, 1, size, file);
   fclose(file);

   // Check ELF image
   if (!valid_elf_image(digest)) {
      free(digest);
      printf("Error: Invalid ELF file: %s\n", name);
      exit(1);
   }

   // Allocate header
   fl_entry = (flash_entry_t *)calloc(1, sizeof(flash_entry_t));

   // Fill header
   //strcpy(fl_entry->name, name);
   memcpy(fl_entry->name, name, strlen(name));
   fl_entry->magic = FL_ENTRY_MAGIC;
   fl_entry->size = ROUND4(size);
   fl_entry->type = type;
   memcpy(image, fl_entry, sizeof(flash_entry_t));

   // Copy file
   memcpy(image + sizeof(flash_entry_t), digest, size);

   free(fl_entry);
   free(digest);

   return fl_entry->size + sizeof(flash_entry_t);
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

   /*if (ehdr->e_type != ET_EXEC) {
      return 0;
   }*/

   return 1;
}

/*
 * get_elf_image()
 *
 *    Copy the elf raw data
 */
int copy_elf_sections(unsigned char *out, unsigned char *addr)
{
   Elf32_Ehdr *ehdr; /* Elf header structure pointer     */
   Elf32_Shdr *shdr; /* Section header structure pointer */
   unsigned char *strtab = 0;
   unsigned char *image;
   unsigned char *otmp = out;
   int i;

   ehdr = (Elf32_Ehdr *) addr;

   // Find the section header string table for output info
   shdr = (Elf32_Shdr *) (addr + ehdr->e_shoff + (ehdr->e_shstrndx
         * sizeof(Elf32_Shdr)));

   if (shdr->sh_type == SHT_STRTAB)
      strtab = (unsigned char *) (addr + shdr->sh_offset);

   // Load each appropriate section
   for (i = 0; i < ehdr->e_shnum; ++i) {
      shdr = (Elf32_Shdr *) (addr + ehdr->e_shoff + (i * sizeof(Elf32_Shdr)));

      if (!(shdr->sh_flags & SHF_ALLOC) || shdr->sh_addr == 0 || shdr->sh_size == 0) {
         continue;
      }

      if (shdr->sh_type == SHT_NOBITS) {
         memset((void *) otmp, 0, shdr->sh_size);
         otmp += shdr->sh_size;
      } else {
         image = (unsigned char *) addr + shdr->sh_offset;
         memcpy((void *) otmp, (const void *) image, shdr->sh_size);
         otmp += shdr->sh_size;
      }
   }

   return (int)(otmp - out);
}

int main(int argc, char *argv[])
{
   FILE *file;
   int size, totsize, i;
   char *image, *digest;

   // Check parameters
   if (argc < 4) {
      printf("Usage: packer fboot fmain [flibs] fout\n");
      exit(1);
   }

   // Allocate the size of the flash memory
   image = (unsigned char *)calloc(1, 8000000);

   // Bootloader file
   file = fopen(argv[1], "rb");
   size = get_file_size(file);
   digest = (unsigned char *)calloc(1, size);
   fread(digest, 1, size, file);
   fclose(file);

   if (!valid_elf_image(digest)) {
      printf("Error: Invalid bootloader ELF file\n");
      exit(1);
   }

   // If the image was valid, copy the .text section of the boot loader
   totsize = copy_elf_sections(image, digest);

   // Bootloader pack finished
   free(digest);

   // Add the main application
   size = add_file(image+totsize, argv[2], FL_ENTRY_APP);
   totsize += size;

   // Add the libraries
   for (i=3; i<argc-1; ++i) {
      size = add_file(image+totsize, argv[i], FL_ENTRY_LIB);
      totsize += size;
   }

   // EOF character
   *((int *)(image+totsize)) = FL_ENTRY_EOF;
   totsize += 4;

   // Write
   file = fopen(argv[i], "wb");
   fwrite(image, 1, totsize, file);
   fclose(file);

   free(image);

   printf("Flash image created\n");

   return 0;
}
