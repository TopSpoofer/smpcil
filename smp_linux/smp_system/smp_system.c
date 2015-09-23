#include "../../smpcil.h"
#include <sys/prctl.h>
#include "./smp_system.h"


SMP_VALUE smp_sys_getpid_byname(const char *name)
{
    if (smp_unlikely(name == NULL)) return SMP_FAILURE;
    else {
        FILE *fptr;
        char buf[128] = {'\0'};
        char cmd[128] = {'\0'};

        pid_t pid = -1;
        sprintf(cmd, "pidof %s", name);

        if((fptr = popen(cmd,"r")) != NULL) {
            if(fgets(buf, 128, fptr) != NULL) {
                pid = atoi(buf);
            }
        }

        pclose(fptr);
        return pid;
    }
}


SMP_VALUE smp_sys_reset_pname(const char *new_name)
{
//    if (smp_unlikely(new_name == NULL)) return SMP_FAILURE;
//    else {
//        int ret = prctl( PR_SET_NAME, new_name, 0, 0, 0);
//        if (ret < 0) return SMP_FAILURE;
//        else return SMP_TRUE;
//    }
    new_name = new_name;
    return -1;
}


