#include "../smpcil.h"
#include "./smp_sys_test.h"
#include "../smp_linux/smp_system/smp_system.h"


void smp_test_sys_name2pid()
{
    sleep(10);
    int pid = smp_sys_getpid_byname("smpcil");
    printf("pid %d\n", pid);
}

void smp_test_sys_reset_pname()
{
    sleep(5);
    int pid = smp_sys_getpid_byname("smpcil");
    printf("pid %d\n", pid);

    int ret = smp_sys_reset_pname("fork_smpcil_tets");
    printf("%d\n", ret);

    pid = smp_sys_getpid_byname("smpcil");
    printf("pid %d\n", pid);

    pid = smp_sys_getpid_byname("fork_smpcil_tets");
    printf("pid %d\n", pid);
    sleep(100);

}

void smp_test_sys_main()
{
//    smp_test_sys_name2pid();
    smp_test_sys_reset_pname();
}

