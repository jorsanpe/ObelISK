/*
 * $FILE: semaphore.c
 *
 * ITRON semaphores user side
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
 * acre_sem()
 *
 *    Create semaphore with automatid ID assignment
 *
 *    WCET: Constant
 */
ER acre_sem(T_CSEM *pk_csem)
{
   int res;
   res = _syscall1(-TFN_ACRE_SEM, pk_csem);
   return res;
}

/*
 * cre_sem()
 *
 *    Create semaphore
 *
 *    WCET: Constant
 */
ER cre_sem(ID semid, T_CSEM *pk_csem)
{
   int res;
   res = _syscall2(-TFN_CRE_SEM, semid, pk_csem);
   return res;
}

/*
 * del_sem()
 *
 *    Delete semaphore
 *
 *    WCET: Non Constant
 */
ER del_sem(ID semid)
{
   int res;
   res = _syscall1(-TFN_DEL_SEM, semid);
   return res;
}


/*
 * sig_sem()
 *
 *    Release semaphore
 *
 *    WCET: Constant
 */
ER sig_sem(ID semid)
{
   int res;
   res = _syscall1(-TFN_SIG_SEM, semid);
   return res;
}

/*
 * wai_sem()
 *
 *    Acquire semaphore
 *    TODO: Consider semaphore wait queue managed by priorities (TA_TPRI)
 *
 *    WCET: Constant
 */
ER wai_sem(ID semid)
{
   int res;
   res = _syscall1(-TFN_WAI_SEM, semid);
   return res;
}

/*
 * twai_sem()
 *
 *    Acquire semaphore with timeout
 *    TODO: Consider semaphore wait queue managed by priorities (TA_TPRI)
 *
 *    WCET: Constant
 */
ER twai_sem(ID semid, TMO tmout)
{
   int res;
   res = _syscall2(-TFN_TWAI_SEM, semid, tmout);
   return res;
}

