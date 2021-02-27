/*
 * $FILE: task.c
 *
 * ITRON tasks user side
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
 * acre_tsk()
 *
 *    Task creation with automatic ID assignment
 *
 *    WCET: Constant
 */
ER acre_tsk(T_CTSK *pk_ctsk)
{
   int res;
   res = _syscall1(-TFN_ACRE_TSK, pk_ctsk);
   return res;
}

/*
 * act_tsk()
 *
 * WCET: Constant
 */
ER act_tsk(ID tskid)
{
   int res;
   res = _syscall1(-TFN_ACT_TSK, tskid);
   return res;
}

/*
 * can_act()
 *
 *    Cancel activation requests
 *
 *    WCET: Constant
 */
ER can_act(ID tskid)
{
   int res;
   res = _syscall1(-TFN_CAN_ACT, tskid);
   return res;
}

/*
 * chg_pri()
 *
 *    Change task priority
 *
 *    WCET: Constant
 */
ER chg_pri(ID tskid, PRI tskpri)
{
   int res;
   res = _syscall2(-TFN_CHG_PRI, tskid, tskpri);
   return res;
}

/*
 * cre_tsk()
 *
 *    Create task
 *
 *    WCET: Constant
 */
ER cre_tsk(ID tskid, T_CTSK *pk_ctsk)
{
   int res;
   res = _syscall2(-TFN_CRE_TSK, tskid, pk_ctsk);
   return res;
}

/*
 * del_tsk()
 *
 *    Delete task
 *
 *    WCET: Constant
 */
ER del_tsk(ID tskid)
{
   int res;
   res = _syscall1(-TFN_DEL_TSK, tskid);
   return res;
}

/*
 * ext_tsk
 *
 *    Finish the execution of the task
 *
 *    WCET: Constant
*/
void ext_tsk(void)
{
   _syscall0(-TFN_EXT_TSK);
}

/*
 * exd_tsk
 *
 *    Finish the execution of the task and
 *    release the ID used
 *
 *    WCET: Constant
*/
void exd_tsk(void)
{
   _syscall0(-TFN_EXD_TSK);
}

/*
 * get_pri()
 *
 *    Get task current priority
 *
 *    WCET: Constant
 */
ER get_pri(ID tskid, PRI *p_tskpri)
{
   int res;
   res = _syscall2(-TFN_GET_PRI, tskid, p_tskpri);
   return res;
}

ER rel_wai(ID tskid)
{
   int res;
   res = _syscall1(-TFN_REL_WAI, tskid);
   return res;
}

/*
 * slp_tsk()
 *
 *    Sleep invoking task. Task is put into WAITING state
 *
 *    WCET: Constant
 */
ER slp_tsk(void)
{
   int res;
   res = _syscall0(-TFN_SLP_TSK);
   return res;
}

/*
 * tslp_tsk()
 *
 *    Sleep invoking task for tmout ticks
 *
 *    WCET: Constant
 */
ER tslp_tsk(TMO tmout)
{
   int res;
   res = _syscall1(-TFN_TSLP_TSK, tmout);
   return res;
}

/*
 * wup_tsk()
 *
 *    Wake up task
 */
ER wup_tsk(ID tskid)
{
   return E_SYS;
}

/*
 * sus_tsk
 */
ER sus_tsk(ID tskid)
{
   return E_SYS;
}

/*
 * ter_tsk()
 *
 *    TODO: Finish the implementation of this function
 */
ER ter_tsk(ID tskid)
{
   return E_SYS;
}




