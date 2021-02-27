/*
 * $FILE: stdarg.c
 *
 * Standard buffered input/output
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
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
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define A_GETC(fn) (++consumed,(fn)->getch((fn)->data))
#define A_PUTC(c,fn) (--consumed,(fn)->putch((c),(fn)->data))

struct str_data {
   unsigned char *str;
};

static int sgetc(struct str_data* sd)
{
   register unsigned int ret = *(sd->str++);
   return (ret) ? (int) ret : -1;
}

static int sputc(int c, struct str_data* sd)
{
   return (*(--sd->str) == c) ? c : -1;
}

#define A_GETC(fn)   (++consumed,(fn)->getch((fn)->data))
#define A_PUTC(c,fn) (--consumed,(fn)->putch((c),(fn)->data))

static int copystring(char* buf, int maxlen, const char* s)
{
   int i;
   for (i = 0; i < 3 && i < maxlen; ++i)
      buf[i] = s[i];
   if (i < maxlen) {
      buf[i] = 0;
      ++i;
   }
   return i;
}

static int __ltostr(char *s, unsigned int size, unsigned long i, unsigned int base, int UpCase)
{
   char *tmp;
   unsigned int j = 0;

   s[--size] = 0;

   tmp = s + size;

   if ((base == 0) || (base > 36))
      base = 10;

   j = 0;
   if (!i) {
      *(--tmp) = '0';
      j = 1;
   }

   while ((tmp > s) && (i)) {
      tmp--;
      if ((*tmp = i % base + '0') > '9')
         *tmp += (UpCase ? 'A' : 'a') - '9' - 1;
      i = i / base;
      j++;
   }
   memmove(s, tmp, j + 1);

   return j;
}

static int __lltostr(char *s, int size, unsigned long long i, int base, char UpCase)
{
   char *tmp;
   unsigned int j = 0;

   s[--size] = 0;

   tmp = s + size;

   if ((base == 0) || (base > 36))
      base = 10;

   j = 0;
   if (!i) {
      *(--tmp) = '0';
      j = 1;
   }

   while ((tmp > s) && (i)) {
      tmp--;
      if ((*tmp = i % base + '0') > '9')
         *tmp += (UpCase ? 'A' : 'a') - '9' - 1;
      i = i / base;
      j++;
   }
   memmove(s, tmp, j + 1);

   return j;
}

static inline unsigned int skip_to(const char *format)
{
   unsigned int nr;
   for (nr = 0; format[nr] && (format[nr] != '%'); ++nr);
   return nr;
}

#define A_WRITE(fn,buf,sz) ((fn)->put((void*)(buf),(sz),(fn)->data))

static const char pad_line[2][16] = { "                ", "0000000000000000", };
static inline int write_pad(struct arg_printf* fn, int len, int padwith)
{
   int nr = 0;
   for (; len > 15; len -= 16, nr += 16) {
      A_WRITE(fn,pad_line[(padwith=='0')?1:0],16);
   }
   if (len > 0) {
      A_WRITE(fn,pad_line[(padwith=='0')?1:0],(unsigned int)len);
      nr += len;
   }
   return nr;
}

// Printing floats inside the kernel is not allowed
int __v_printf(struct arg_printf *fn, const char *format, va_list arg_ptr)
{
   int len = 0;

   while (*format) {
      unsigned int sz = skip_to(format);
      if (sz) {
         A_WRITE(fn,format,sz);
         len += sz;
         format += sz;
      }
      if (*format == '%') {
         char buf[128];
         union {
            char*s;
         } u_str;
#define s u_str.s

         int retval;
         unsigned char ch, padwith = ' ';

         char flag_in_sign = 0;
         char flag_upcase = 0;
         char flag_hash = 0;
         char flag_left = 0;
         char flag_space = 0;
         char flag_sign = 0;
         char flag_dot = 0;
         signed char flag_long = 0;

         unsigned int base;
         unsigned int width = 0, preci = 0;

         long number = 0;
         long long llnumber = 0;

         ++format;
         inn_printf: switch (ch = *format++) {
            case 0:
               return -1;
               break;

               /* FLAGS */
            case '#':
               flag_hash = -1;
            case 'z':
               goto inn_printf;

            case 'h':
               --flag_long;
               goto inn_printf;
            case 'q': /* BSD ... */
            case 'L':
               ++flag_long; /* fall through */
            case 'l':
               ++flag_long;
               goto inn_printf;

            case '-':
               flag_left = 1;
               goto inn_printf;

            case ' ':
               flag_space = 1;
               goto inn_printf;

            case '+':
               flag_sign = 1;
               goto inn_printf;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               if (flag_dot)
                  return -1;
               width = strtoul(format - 1, (char**) &s, 10);
               if (ch == '0' && !flag_left)
                  padwith = '0';
               format = s;
               goto inn_printf;

            case '*':
               width = va_arg(arg_ptr,int);
               goto inn_printf;

            case '.':
               flag_dot = 1;
               if (*format == '*') {
                  int tmp = va_arg(arg_ptr,int);
                  preci = tmp < 0 ? 0 : tmp;
                  ++format;
               } else {
                  long int tmp = strtol(format, (char**) &s, 10);
                  preci = tmp < 0 ? 0 : tmp;
                  format = s;
               }
               goto inn_printf;

               /* print a char or % */
            case 'c':
               ch = (char) va_arg(arg_ptr,int);
            case '%':
               A_WRITE(fn,&ch,1);
               ++len;
               break;

               /* print a string */
            case 's':
               s = va_arg(arg_ptr,char *);
               if (!s)
                  s = "(null)";

               sz = strlen(s);
               if (flag_dot && sz > preci)
                  sz = preci;
               preci = 0;
               flag_dot ^= flag_dot;
               padwith = ' ';

               print_out: {
                  char *sign = s;
                  int todo = 0;
                  int vs;

                  if (!(width || preci)) {
                     A_WRITE(fn,s,sz);
                     len += sz;
                     break;
                  }

                  if (flag_in_sign)
                     todo = 1;
                  if (flag_hash > 0)
                     todo = flag_hash;
                  if (todo) {
                     s += todo;
                     sz -= todo;
                     width -= todo;
                  }

                  if (!flag_left) {
                     if (flag_dot) {
                        vs = preci > sz ? preci : sz;
                        len += write_pad(fn, (signed int) width - (signed int) vs, ' ');
                        if (todo) {
                           A_WRITE(fn,sign,todo);
                           len += todo;
                        }
                        len += write_pad(fn, (signed int) preci - (signed int) sz, '0');
                     } else {
                        if (todo && padwith == '0') {
                           A_WRITE(fn,sign,todo);
                           len += todo;
                           todo = 0;
                        }
                        len += write_pad(fn, (signed int) width
                              - (signed int) sz, padwith);
                        if (todo) {
                           A_WRITE(fn,sign,todo);
                           len += todo;
                        }
                     }
                     A_WRITE(fn,s,sz);
                     len += sz;
                  } else if (flag_left) {
                     if (todo) {
                        A_WRITE(fn,sign,todo);
                        len += todo;
                     }
                     len += write_pad(fn, (signed int) preci - (signed int) sz,
                           '0');
                     A_WRITE(fn,s,sz);
                     len += sz;
                     vs = preci > sz ? preci : sz;
                     len += write_pad(fn, (signed int) width - (signed int) vs,
                           ' ');
                  } else {
                     A_WRITE(fn,s,sz);
                     len += sz;
                  }
                  break;
               }

               /* print an integer value */
            case 'b':
               base = 2;
               sz = 0;
               goto num_printf;
            case 'p':
               flag_hash = 2;
               flag_long = 1;
               ch = 'x';
            case 'X':
               flag_upcase = (ch == 'X');
            case 'x':
               base = 16;
               sz = 0;
               if (flag_hash) {
                  buf[1] = '0';
                  buf[2] = ch;
                  flag_hash = 2;
                  sz = 2;
               }
               if (preci > width)
                  width = preci;
               goto num_printf;
            case 'd':
            case 'i':
               flag_in_sign = 1;
            case 'u':
               base = 10;
               sz = 0;
               goto num_printf;
            case 'o':
               base = 8;
               sz = 0;
               if (flag_hash) {
                  buf[1] = '0';
                  flag_hash = 1;
                  ++sz;
               }

               num_printf: s = buf + 1;

               if (flag_long > 0) {
                  if (flag_long > 1)
                     llnumber = va_arg(arg_ptr, long long);
                  else
                     number = va_arg(arg_ptr, long);
               } else
                  number = va_arg(arg_ptr, int);

               if (flag_in_sign) {
                  if ((flag_long > 1) && (llnumber < 0)) {
                     llnumber = -llnumber;
                     flag_in_sign = 2;
                  } else if (number < 0) {
                     number = -number;
                     flag_in_sign = 2;
                  }
               }
               if (flag_long < 0)
                  number &= 0xffff;
               if (flag_long < -1)
                  number &= 0xff;
               if (flag_long > 1)
                  retval = __lltostr(s + sz, sizeof(buf) - 5, (unsigned long long) llnumber, base, flag_upcase);
               else
                  retval = __ltostr(s + sz, sizeof(buf) - 5, (unsigned long) number, base, flag_upcase);

               /* When 0 is printed with an explicit precision 0, the output is empty. */
               if (flag_dot && retval == 1 && s[sz] == '0') {
                  if (preci == 0 || flag_hash > 0) {
                     sz = 0;
                  }
                  flag_hash = 0;
               } else
                  sz += retval;

               if (flag_in_sign == 2) {
                  *(--s) = '-';
                  ++sz;
               } else if ((flag_in_sign) && (flag_sign || flag_space)) {
                  *(--s) = (flag_sign) ? '+' : ' ';
                  ++sz;
               } else
                  flag_in_sign = 0;

               goto print_out;

#ifdef WANT_FLOATING_POINT_IN_PRINTF
               /* print a floating point value */
               case 'f':
               case 'g':
               {
                  int g=(ch=='g');
                  double d=va_arg(arg_ptr,double);
                  s=buf+1;
                  if (width==0) width=1;
                  if (!flag_dot) preci=6;
                  if (flag_sign || d < +0.0) flag_in_sign=1;

                  sz=__dtostr(d,s,sizeof(buf)-1,width,preci);

                  if (flag_dot) {
                     char *tmp;
                     if ((tmp=strchr(s,'.'))) {
                        if (preci || flag_hash) ++tmp;
                        while (preci>0 && *++tmp) --preci;
                        *tmp=0;
                     } else if (flag_hash) {
                        s[sz]='.';
                        s[++sz]='\0';
                     }
                  }

                  if (g) {
                     char *tmp,*tmp1; /* boy, is _this_ ugly! */
                     if ((tmp=strchr(s,'.'))) {
                        tmp1=strchr(tmp,'e');
                        while (*tmp) ++tmp;
                        if (tmp1) tmp=tmp1;
                        while (*--tmp=='0');
                        if (*tmp!='.') ++tmp;
                        *tmp=0;
                        if (tmp1) strcpy(tmp,tmp1);
                     }
                  }

                  if ((flag_sign || flag_space) && d>=0) {
                     *(--s)=(flag_sign)?'+':' ';
                     ++sz;
                  }

                  sz=strlen(s);
                  flag_dot=0;
                  flag_hash=0;
                  goto print_out;
               }
#endif

            default:
               break;
         }
      }
   }
   return len;
}

/*
int vprintf(const char *format, va_list ap)
{
   struct arg_printf _ap = { 0, (int(*)(void*, size_t, void*)) __stdio_outs };
   return __v_printf(&_ap, format, ap);
}*/
