#include "../smpcil.h"
#include "../smp_linux/smp_ipc/smp_ipc.h"
#include "../smp_linux/smp_error/smp_error.h"

SMP_STATUS smp_test_sem_getset(int semid)
{
    int i = 0;
    unsigned short sem_arry[3];
    unsigned short sem_arry_read[3];
    smp_semun arg;
    struct sembuf buf[3];

    for (i = 0; i < 3; i++) sem_arry[i] = i + 1;
    arg.array = sem_arry;

    if (smp_sem_setall(semid, arg) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    arg.array = sem_arry_read;

    if (smp_sem_getall(semid, arg) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    for (i = 0; i < 3; i++) printf("semaphore  %d  value %d\n", i, sem_arry_read[i]);

    for (i = 0; i < 3; i++) {
        int ret = smp_sem_getval(semid, i);
        printf("semaphore  %d  value %d\n", i, ret);
    }

    smp_sem_setop_get(buf, 0, 1, IPC_NOWAIT);
    smp_semop(semid, buf, 1);

    printf("after get semop\n");

    for (i = 0; i < 3; i++) {
        int ret = smp_sem_getval(semid, i);
        printf("semaphore  %d  value %d\n", i, ret);
    }

    smp_sem_setop_release(buf, 0, 1, IPC_NOWAIT);
    smp_semop(semid, buf, 1);

    printf("after release semop\n");

    for (i = 0; i < 3; i++) {
        int ret = smp_sem_getval(semid, i);
        printf("semaphore  %d  value %d\n", i, ret);
    }

    return SMP_OK;
}

SMP_STATUS smp_test_semget()
{
    int semid = 12345677;

    semid = smp_sem_create(semid, 3);
    if (semid == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("semid is %d\n", semid);

    smp_test_sem_getset(semid);

    smp_sem_remove(semid);

    return SMP_OK;
}


void smp_test_sem_main()
{
    smp_test_semget();

    printf("finished test\n");
}
