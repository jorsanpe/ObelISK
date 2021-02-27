/*
 * $FILE: libelf.c
 *
 * ELF library for the ObelISK kernel
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

/*
#include <libelf.h>

int lookup_symbol(ElfDesc *ed, const char *sym, ADDR *addr)
{
   char *name;
   Elf32_Sym *symtab = ed->symtab;
   size_t symtab_sz = ed->symtab_sz;
   char *strtab = ed->strtab;

   int i;
   if (symtab && strtab) {
      for (i = 0; i < symtab_sz; i++) {
         name = &strtab[symtab[i].st_name];
         if (strcmp(name, sym) == 0) {
            *addr = symtab[i].st_value;
            return 1;
         }
      }
   }

   return 0; // not found
}*/




