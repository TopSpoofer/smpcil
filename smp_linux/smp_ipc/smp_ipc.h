#ifndef SMP_IPC_H
#define SMP_IPC_H

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/sem.h>


typedef struct smp_msg_s {
    long type;
    char msg[0];
}smp_msg_t;


/*  msg  */
inline SMP_VALUE smp_msgget(key_t key, int flg);
inline SMP_VALUE smp_msgqueue_create(int key);
inline SMP_STATUS smp_msgsnd(int msgid, const void *msgbuf, size_t msg_size, int msgflg);
inline SMP_STATUS smp_msgsnd_wait(int msgid, const struct smp_msg_s *msgbuf, size_t data_size);
inline SMP_STATUS smp_msgsnd_nowait(int msgid, const struct smp_msg_s *msgbuf, size_t data_size);
inline SMP_VALUE smp_msgrcv(int msgid, void *msgbuf, size_t msg_size, long msgtype, int msgflg);
inline SMP_VALUE smp_msgrcv_wait(int msgid, struct smp_msg_s *msgbuf, size_t rcv_size, long msgtype);
inline SMP_VALUE smp_msgrcv_nowait(int msgid, struct smp_msg_s *msgbuf, size_t rcv_size, long msgtype);
inline SMP_STATUS smp_msgctl(int msgid, int cmd, struct msqid_ds *buf);
inline SMP_VALUE smp_msgqueue_elem_amount(int msgid);
inline SMP_VALUE smp_msgqueue_usedsize(int msgid);
inline SMP_VALUE smp_msgqueue_maxsize(int msgid);
inline SMP_VALUE smp_msgqueue_msg_maxsize(int msgid);
inline SMP_STATUS smp_msgqueue_set_maxsize(int msgid, size_t size);
inline SMP_STATUS smp_msgqueue_remove(int msgid);


/* sem */

typedef union smp_semun_u {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
    void *__pad;
}smp_semun;

#define smp_semctl semctl
#define SMP_IPC_ONTWAIT IPC_NOWAIT
#define SMP_SEM_UNDO    SEM_UNDO


inline SMP_VALUE smp_semget(key_t key, int amount, int flg);
inline SMP_VALUE smp_sem_create(int key, int amount);
inline SMP_VALUE smp_sem_owner(int semid);
inline SMP_VALUE smp_sem_getval(int semid, int n);
inline SMP_STATUS smp_sem_getall(int semid, union smp_semun_u semun);
inline SMP_VALUE smp_sem_wait_increment(int semid);
inline SMP_VALUE smp_sem_wait_decrease(int semid);
inline SMP_STATUS smp_sem_setval(int semid, int n, int val);
inline SMP_STATUS smp_sem_setall(int semid, union smp_semun_u semun);
inline SMP_STATUS smp_semop(int semid, struct sembuf *semops, int semops_amt);
inline SMP_STATUS smp_sem_setop_get(struct sembuf *semops, int i, int n, int way);
inline SMP_STATUS smp_sem_setop_release(struct sembuf *semops, int i, int n, int way);
inline SMP_STATUS smp_sem_remove(int semid);


#endif // SMP_IPC_H
