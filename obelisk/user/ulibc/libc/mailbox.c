/*
 * $FILE: mailbox.c
 *
 * ITRON mailboxes, user side
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
 * acre_mbx()
 *
 *    Create mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ID acre_mbx(T_CMBX *pk_cmbx)
{
   int res;
   res = _syscall1(-TFN_ACRE_MBX, pk_cmbx);
   return res;
}

/*
 * cre_mbx()
 *
 *    Create mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ER cre_mbx(ID mbxid, T_CMBX *pk_cmbx)
{
   int res;
   res = _syscall2(-TFN_CRE_MBX, mbxid, pk_cmbx);
   return res;
}

/*
 * del_mbx()
 *
 *    Delete mailbox
 *
 *    TODO: Check any memory that has been allocated by the kernel
 *    to release it. Case of mailboxes handled by priorities
 *
 *    WCET: Non Constant
 */
ER del_mbx(ID mbxid)
{
   int res;
   res = _syscall1(-TFN_DEL_MBX, mbxid);
   return res;
}

/*
 * rcv_mbx()
 *
 *    Receive from mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ER rcv_mbx(ID mbxid, T_MSG **ppk_msg)
{
   int res;
   res = _syscall2(-TFN_RCV_MBX, mbxid, ppk_msg);
   return res;
}

/*
 * trcv_mbx()
 *
 *    Receive from mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ER trcv_mbx(ID mbxid, T_MSG **ppk_msg, TMO tmout)
{
   int res;
   res = _syscall3(-TFN_TRCV_MBX, mbxid, ppk_msg, tmout);
   return res;
}

/*
 * ref_mbx()
 *
 *    Reference mailbox state
 *
 *    WCET: Constant
 */
ER ref_mbx(ID mbxid, T_RMBX *pk_rmbx)
{
   int res;
   res = _syscall2(-TFN_REF_MBX, mbxid, pk_rmbx);
   return res;
}

/*
 * snd_mbx()
 *
 *    Send to mailbox
 *
 *    TODO: Consider mailbox queue managed by priorities
 *
 *    WCET: Constant
 */
ER snd_mbx(ID mbxid, T_MSG *pk_msg)
{
   int res;
   res = _syscall2(-TFN_SND_MBX, mbxid, pk_msg);
   return res;
}

