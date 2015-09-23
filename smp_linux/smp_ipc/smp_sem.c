#include "../../smpcil.h"
#include "../smp_error/smp_error.h"
#include "./smp_ipc.h"



inline SMP_VALUE smp_semget(key_t key, int amount, int flg)
{
    int ret = SMP_FAILURE;

    ret = semget(key, amount, flg);

    if (smp_unlikely(ret == -1)) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return ret;
}


/**
 *
 *  返回 semid 的拥有者的pid
 *
 * */

inline SMP_VALUE smp_sem_create(int key, int amount)
{
    return smp_semget((key_t) key, amount, IPC_CREAT | 0600);
}


inline SMP_VALUE smp_sem_owner(int semid)
{
    int ret = 0;

    if ((ret = smp_semctl(semid, 0, GETPID)) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return ret;
}


/**
 *
 *  获取信号量的值
 *
 * */

inline SMP_VALUE smp_sem_getval(int semid, int n)
{
    int ret = 0;

    if ((ret = smp_semctl(semid, n, GETVAL)) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return ret;
}


inline SMP_STATUS smp_sem_getall(int semid, union smp_semun_u semun)
{
    if (smp_semctl(semid, 0, GETALL, semun) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_VALUE smp_sem_wait_increment(int semid)
{
    int ret = 0;

    if ((ret = smp_semctl(semid, 0, GETNCNT)) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return ret;
}


inline SMP_VALUE smp_sem_wait_decrease(int semid)
{
    int ret = 0;

    if ((ret = smp_semctl(semid, 0, GETZCNT)) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return ret;
}


inline SMP_STATUS smp_sem_setval(int semid, int n, int val)
{
    union smp_semun_u semun;
    semun.val = val;

    if (smp_semctl(semid, n, SETVAL, semun) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_STATUS smp_sem_setall(int semid, union smp_semun_u semun)
{
    if (smp_semctl(semid, 0, SETALL, semun) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


/**
 *
 *  @semops 是对信号量操作的数组。
 *
 *  @semops_amt 是数组的元素的数量。
 *
 * */

inline SMP_STATUS smp_semop(int semid, struct sembuf *semops, int semops_amt)
{
    if (semop(semid, semops, semops_amt) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


/**
 *
 *  @i 对第i个信号量进行操作。
 *
 *  @n 信号量每次被get多少个，执行get后，信号量个数会减小n个。n > 0
 *
 *  @way 可以为SMP_IPC_ONTWAIT或者SMP_SEM_UNDO。z
 *
 *  SEM_UNDO, 在进程退出后，其对信号量的操作将被撤销。
 *
 * */

inline SMP_STATUS smp_sem_setop_get(struct sembuf *semops, int i, int n, int way)
{
    if (n < 0) n = -n;
    semops->sem_num = i;
    semops->sem_op = -n;
    semops->sem_flg = way;

    return SMP_OK;
}


/**
 *
 *  @n n < 0
 *
 * */

inline SMP_STATUS smp_sem_setop_release(struct sembuf *semops, int i, int n, int way)
{
    if (n > 0) n = -n;
    semops->sem_num = i;
    semops->sem_op = -n;
    semops->sem_flg = way;

    return SMP_OK;
}


inline SMP_STATUS smp_sem_remove(int semid)
{
    if (smp_semctl(semid, 0, IPC_RMID) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}
