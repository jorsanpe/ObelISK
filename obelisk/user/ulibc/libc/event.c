/*
 * $FILE: event.c
 *
 * ITRON Events, user side
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
 * clr_flg()
 *
 *    Clear eventflag
 *
 *    WCET: Constant
 */
ER clr_flg(ID flgid, FLGPTN clrptn)
{
   int res;
   res = _syscall2(-TFN_CLR_FLG, flgid, clrptn);
   return res;
}

/*
 * cre_flg()
 *
 *    Create Eventflag
 *
 *    WCET: Constant
 */
ER cre_flg(ID flgid, T_CFLG *pk_cflg)
{
   int res;
   res = _syscall2(-TFN_CRE_FLG, flgid, pk_cflg);
   return res;
}

/*
 * del_flg()
 *
 *    Delete eventflag
 *
 *    WCET: Non Constant
 */
ER del_flg(ID flgid)
{
   int res;
   res = _syscall1(-TFN_DEL_FLG, flgid);
   return res;
}

/*
 * set_flg()
 *
 *    Set eventflag
 *
 *    WCET: Non Constant
 */
ER set_flg(ID flgid, FLGPTN setptn)
{
   int res;
   res = _syscall2(-TFN_SET_FLG, flgid, setptn);
   return res;
}

/*
 * wai_flg()
 *
 *    Wait for eventflag
 *    TODO: Consider eventflag wait queue managed by priorities (TA_TPRI)
 *
 *    WCET: Constant
 */
ER wai_flg(ID flgid, FLGPTN waiptn, UINT wfmode, FLGPTN *p_flgptn)
{
   int res;
   res = _syscall4(-TFN_WAI_FLG, flgid, waiptn, wfmode, p_flgptn);
   return res;
}

