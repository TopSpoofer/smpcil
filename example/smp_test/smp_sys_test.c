#include "../smpcil.h"
#include "./smp_sys_test.h"
#include "../smp_linux/smp_system/smp_system.h"


extern char **environ;


void smp_test_sys_name2pid()
{
    sleep(10);
    int pid = smp_sys_getpid_byname("smpcil");
    printf("pid %d\n", pid);
}

void smp_test_sys_reset_pname(int argc, char *argv[])
{
    int pid = smp_sys_getpid_byname("smpcil");
    printf("pid %d\n", pid);
    smp_set_proc_titel("fork_smpcil_tetsgfdhdfhgfdhfghfghsdvsdvsvdsdvsdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddfgh--", argc, argv);

    pid = smp_sys_getpid_byname("fork_smpcil_tetsgfdhdfhgfdhfghfghsdvsdvsvdsdvsdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddfgh--");
    printf("pid %d\n", pid);
    sleep(10000);

    int i = 0;
    for (i = 0; environ[i]; i++) {
        printf("%s\n", environ[i]);
    }

    pid_t pid_f;
    if ((pid_f = fork()) == -1) return;
    else {
        if (pid_f == 0) { //child
            int i = 0;
            for (i = 0; environ[i]; i++) {
                printf("---%s---\n", environ[i]);
            }
        }else { //parent
            sleep(1);
            int i = 0;
            for (i = 0; environ[i]; i++) {
                printf("+++%s+++\n", environ[i]);
            }
        }
    }
}

void smp_test_sys_main(int argc, char *argv[])
{
//    smp_test_sys_name2pid();
    smp_test_sys_reset_pname(argc, argv);
}

