FILE_OUT=core/include/asm-offsets.h
bfin-elf-gcc -S scripts/asm-offsets.c -I./core/include -D_KERNEL_ -o asm-offsets.s
cat <<EOF > $FILE_OUT
/*
 * AUTOMATICALLY GENERATED FILE. DO NOT MODIFY
 *
 * \$FILE: asm-offsets.h
 * 
 * This file contains the assembler offsets needed by
 * the assembler routines in order to access the task
 * structure fields
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

#ifndef _ASM_OFFSETS_H_
#define _ASM_OFFSETS_H_

EOF

cat asm-offsets.s | awk '($1 == "->") { print "#define " $2 " " $3}' >> $FILE_OUT
echo '#endif' >> $FILE_OUT
rm asm-offsets.s
