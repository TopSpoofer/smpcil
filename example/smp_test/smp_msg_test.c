#include "../smpcil.h"
#include "../smp_linux/smp_ipc/smp_ipc.h"
#include "../smp_linux/smp_error/smp_error.h"


SMP_STATUS smp_test_mag_create()
{
    int key = 1234567;
    int msgid = 0;
    pid_t pid = 0;
    smp_msg_t *msg = NULL;

    msgid = smp_msgqueue_create(key);
    if (msgid == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    if ((pid = fork()) == -1) return SMP_FAILURE;
    else {
        if (pid == 0) { //child
            msg = malloc(sizeof(smp_msg_t) + 1024);
            sleep(1000000);
            while (SMP_TRUE) {
                memset(msg, '\0', sizeof(smp_msg_t) + 1024);
                int ret = smp_msgrcv_wait(msgid, msg, 1024, 1);
                if (ret == SMP_FAILURE) {
                    smp_print_error();
                    continue;
                }

                printf("get msg, size is : %d,  data is \n%s\n", ret, msg->msg);
            }

        }else { //parent
            msg = malloc(sizeof(smp_msg_t) + 1024);

            while (SMP_TRUE) {
                memset(msg, '\0', sizeof(smp_msg_t) + 1024);
                msg->type = 1;
                printf("input msg to send: \n");
                scanf("%s", msg->msg);
                int ret = smp_msgsnd_wait(msgid, msg, strlen(msg->msg));
                if (ret == SMP_FAILURE) {
                    smp_print_error();
                    continue;
                }

                printf("send msg finished\n");
                usleep(100);

                printf("the queue curr elem amt %d\n", smp_msgqueue_elem_amount(msgid));
                printf("the queue usedsize %d\n", smp_msgqueue_usedsize(msgid));
                printf("the queue max msg size %d\n", smp_msgqueue_msg_maxsize(msgid));
                printf("the queue max size %d\n", smp_msgqueue_maxsize(msgid));
            }
        }
    }

    smp_msgqueue_remove(msgid);
    return SMP_FAILURE;
}


void smp_test_msg_main()
{
    smp_test_mag_create();
}
