#include "../../smpcil.h"
#include "../smp_error/smp_error.h"
#include "./smp_ipc.h"


#define SMP_MSG_DEF_MASK 0600


inline SMP_VALUE smp_msgget(key_t key, int flg)
{
    int ret = SMP_FAILURE;

    ret = msgget(key, flg);

    if (smp_unlikely(ret == -1)) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return ret;
}


inline SMP_VALUE smp_msgqueue_create(int key)
{
    return smp_msgget((key_t) key, IPC_CREAT | SMP_MSG_DEF_MASK);
}


inline SMP_STATUS smp_msgsnd(int msgid, const void *msgbuf, size_t msg_size, int msgflg)
{
    int ret = 0;

    ret = msgsnd(msgid, msgbuf, msg_size, msgflg);
    if (ret == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_STATUS smp_msgsnd_wait(int msgid, const struct smp_msg_s *msgbuf, size_t data_size)
{
    if (smp_unlikely(msgbuf == NULL)) return SMP_FAILURE;

    return smp_msgsnd(msgid, msgbuf, data_size, 0);
}


inline SMP_STATUS smp_msgsnd_nowait(int msgid, const struct smp_msg_s *msgbuf, size_t data_size)
{
    if (smp_unlikely(msgbuf == NULL)) return SMP_FAILURE;

    return smp_msgsnd(msgid, msgbuf, data_size, IPC_NOWAIT);
}


inline SMP_VALUE smp_msgrcv(int msgid, void *msgbuf, size_t msg_size, long msgtype, int msgflg)
{
    int ret = 0;

    ret = msgrcv(msgid, msgbuf, msg_size, msgtype, msgflg);

    if (ret == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return ret;
}


inline SMP_VALUE smp_msgrcv_wait(int msgid, struct smp_msg_s *msgbuf, size_t rcv_size, long msgtype)
{
    if (smp_unlikely(msgbuf == NULL)) return SMP_FAILURE;

    return smp_msgrcv(msgid, msgbuf, rcv_size, msgtype, 0);
}


inline SMP_VALUE smp_msgrcv_nowait(int msgid, struct smp_msg_s *msgbuf, size_t rcv_size, long msgtype)
{
    if (smp_unlikely(msgbuf == NULL)) return SMP_FAILURE;

    return smp_msgrcv(msgid, msgbuf, rcv_size, msgtype, IPC_NOWAIT);
}


inline SMP_STATUS smp_msgctl(int msgid, int cmd, struct msqid_ds *buf)
{
    int ret = 0;

    ret = msgctl(msgid, cmd, buf);
    if (ret == SMP_FAILURE) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_VALUE smp_msgqueue_elem_amount(int msgid)
{
    struct msqid_ds msg_info;

    if (smp_msgctl(msgid, IPC_STAT, &msg_info) == SMP_FAILURE) return SMP_FAILURE;

    return msg_info.msg_qnum;
}


inline SMP_VALUE smp_msgqueue_usedsize(int msgid)
{
    struct msqid_ds msg_info;

    if (smp_msgctl(msgid, IPC_STAT, &msg_info) == SMP_FAILURE) return SMP_FAILURE;

    return msg_info.__msg_cbytes;
}


inline SMP_VALUE smp_msgqueue_maxsize(int msgid)
{
    struct msginfo info;

    if (smp_msgctl(msgid, MSG_INFO, (struct msqid_ds *)&info) == SMP_FAILURE)
        return SMP_FAILURE;

    return info.msgmnb;
}


inline SMP_VALUE smp_msgqueue_msg_maxsize(int msgid)
{
    struct msginfo info;

    if (smp_msgctl(msgid, MSG_INFO, (struct msqid_ds *) &info) == SMP_FAILURE)
        return SMP_FAILURE;

    return info.msgmax;
}


inline SMP_STATUS smp_msgqueue_set_maxsize(int msgid, size_t size)
{
    struct msqid_ds msg_info;

    memset(&msg_info, '\0', sizeof(struct msqid_ds));

    msg_info.msg_qbytes = size;

    if (smp_msgctl(msgid, IPC_SET, &msg_info) == SMP_FAILURE) return SMP_FAILURE;

    return SMP_OK;
}


inline SMP_STATUS smp_msgqueue_remove(int msgid)
{
    return smp_msgctl(msgid, IPC_RMID, NULL);
}
