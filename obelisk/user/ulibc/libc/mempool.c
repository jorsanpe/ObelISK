/*
 * $FILE: mempool.c
 *
 * ITRON mempools, user side
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

#include <itron.h>
#include <syscalls.h>

/*
 * cre_mpf()
 *
 *    Create fixed-size memory pool
 *
 *    WCET: Non Constant
 */
ER cre_mpf(ID mpfid, T_CMPF *pk_cmpf)
{
   int res;
   res = _syscall2(-TFN_CRE_MPF, mpfid, pk_cmpf);
   return res;
}

/*
 * del_mpf()
 *
 *    Delete fixed-sized memory pool
 *
 *    WCET: Non Constant
 */
ER del_mpf(ID mpfid)
{
   int res;
   res = _syscall1(-TFN_DEL_MPF, mpfid);
   return res;
}

/*
 * get_mpf()
 *
 *    Acquire fixed-size memory pool block
 *
 *    WCET: Constant for each mpf
 */
ER get_mpf(ID mpfid, VP *p_blk)
{
   int res;
   res = _syscall2(-TFN_GET_MPF, mpfid, p_blk);
   return res;
}

/*
 * rel_mpf()
 *
 *    Release fixed-size memory pool block
 *
 *    WCET: Constant for each mpf
 */
ER rel_mpf(ID mpfid, VP blk)
{
   int res;
   res = _syscall2(-TFN_REL_MPF, mpfid, blk);
   return res;
}

/*
 * cre_mpl()
 *
 *    Create variable-sized memory pool block (TLSF)
 */
ER cre_mpl(ID mplid, T_CMPL *pk_cmpl)
{
   int res;
   res = _syscall2(-TFN_CRE_MPL, mplid, pk_cmpl);
   return res;
}

/*
 * get_mpl()
 *
 *    Acquire variable-sized memory pool block (TLSF)
 *
 *    WCET: Constant
 */
ER get_mpl(ID mplid, UINT blksz, VP *p_blk)
{
   int res;
   res = _syscall3(-TFN_GET_MPL, mplid, blksz, p_blk);
   return res;
}

/*
 * rel_mpl()
 *
 *    Release variable-sized memory pool block
 *
 *    WCET: Constant
 */
ER rel_mpl(ID mplid, VP blk)
{
   int res;
   res = _syscall2(-TFN_REL_MPL, mplid, blk);
   return res;
}
