#ifndef _LOGGER_H_
#define _LOGGER_H_

#define CODE_TABLE_ENTRIES (sizeof(code_table)/sizeof(struct tab_entry))

struct tab_entry {
   int fncode;
   char event[128];
   char arg1[64];
   char arg2[64];
};

struct tab_entry code_table[] = {
{-0x01, "scheduler", "out", "in"},
{-0x02, "rq_insert", "task", ""},
{-0x03, "rq_remove", "task", "status"},
{-0x04, "rq_relocate", "task", "priority"},
{-0x05, "cre_tsk", "task", ""},
{-0x06, "del_tsk", "task", ""},
{-0x07, "act_tsk", "task", "activation count"},
{-0x08, "can_act", "task", "activation count"},
{-0x09, "start task", "", ""},
{-0x0a, "ext_tsk", "task id", ""},
{-0x0b, "exd_tsk", "task", ""},
{-0x0c, "ter_tsk", "", ""},
{-0x0d, "chg_pri", "task", "new priority"},
{-0x0e, "get_pri", "task", "priority"},
{-0x0f, "ref_tsk", "", ""},
{-0x10, "ref_tst", "", ""},
{-0x11, "slp_tsk", "task", ""},
{-0x12, "tslp_tsk", "task", "timeout"},
{-0x13, "wup_tsk", "task", ""},
{-0x14, "can_wup", "task", ""},
{-0x15, "rel_wai", "task", ""},
{-0x16, "sus_tsk", "task", ""},
{-0x17, "resume task", "task", ""},
{-0x18, "resume task with timeout", "task", "timeout"},
{-0x19, "delay task", "task", ""},
{-0x1d, "di", "", ""},
{-0x21, "cre_sem", "sem", ""},
{-0x22, "del_sem", "sem", ""},
{-0x23, "sig_sem", "sem", ""},
{-0x25, "wai_sem", "sem", ""},
{-0x26, "poll semaphore", "sem", ""},
{-0x27, "twai_sem", "sem", ""},
{-0x28, "ref_sem", "sem", ""},
{-0x29, "cre_flg", "flg", ""},
{-0x2a, "del_flg", "flg", ""},
{-0x2b, "set_flg", "flg", ""},
{-0x2c, "clr_flg", "flg", ""},
{-0x2d, "wai_flg", "flg", ""},
{-0x2e, "poll flag", "flg", ""},
{-0x2f, "twai_flg", "flg", ""},
{-0x30, "ref_flg", "flg", ""},
{-0x31, "create dataque", "dtq", ""},
{-0x35, "send to dataque", "dtq", ""},
{-0x39, "receive from dataque", "dtq", ""},
{-0x3d, "cre_mbx", "mbx", ""},
{-0x3e, "del_mbx", "mbx", ""},
{-0x3f, "snd_mbx", "mbx", ""},
{-0x41, "rcv_mbx", "mbx", ""},
{-0x42, "receive from mailbox with polling", "mbx", ""},
{-0x43, "trcv_mbx", "mbx", "tmo"},
{-0x44, "ref_mbx", "mbx", ""},
{-0x45, "cre_mpf", "mpf", ""},
{-0x46, "del_mpf", "mpf", ""},
{-0x47, "rel_mpf", "mpf", ""},
{-0x49, "get_mpf", "mpf", ""},
{-0x4a, "get block from fixed memory pool with polling", "mpf", ""},
{-0x4b, "tget_mpf", "mpf", ""},
{-0x4c, "ref_mpf", "", ""},
{-0x4d, "set_tim", "", ""},
{-0x4e, "get_tim", "", ""},
{-0x61, "ref_sys", "", ""},
{-0x65, "def_inh", "", ""},
{-0x69, "disable interrupt", "", ""},
{-0xa1, "cre_mpl", "", ""},
{-0xa2, "del_mpl", "", ""},
{-0xa3, "rel_mpl", "", ""},
{-0xa5, "get_mpl", "", ""},
{-0xa6, "get block from variable memory pool with polling", "", ""},
{-0xa7, "tget_mpl", "", ""},
{-0xa8, "ref_mpl", "", ""},
{-0xa9, "create alarm", "", ""},
{-0xad, "reference alarm", "", ""},
{-0xc1, "acre_tsk", "task", ""},
{-0xc2, "acre_sem", "sem", ""},
{-0xc3, "acre_flg", "", ""},
{-0xc5, "acre_mbx", "", ""},
{-0xc9, "acre_mpf", "", ""},
{-0xca, "acre_mpl", "", ""},
{-0xcd, "create interrupt service routine with autoid", "", ""},
{0xee, "int", "", ""},
};

#endif   //_LOGGER_H_
