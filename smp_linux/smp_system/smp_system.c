#include "../../smpcil.h"
#include "../../smp_mpool/smp_mpool.h"
#include <sys/prctl.h>
#include "./smp_system.h"


extern char **environ;

static char *smp_sys_argv_last = NULL;
static char *smp_sys_env_last = NULL;


/**
 *  将src copy n 个字节到dst中直到遇到src中的'\0'， 如果n为0,返回dst。
 *  返回在dst中偏移到的地址
 **/

u_char *smp_strcpyn(u_char *dst, const u_char *src, size_t n)
{
    if (smp_unlikely(n == 0)) return dst;

    while (n--) {
        *dst = *src;

        if (*dst == '\0') return dst;

        dst++;
        src++;
    }

    *dst = '\0';

    return dst;
}

SMP_VALUE smp_sys_getpid_byname(const char *name)
{
    if (smp_unlikely(name == NULL)) return SMP_FAILURE;
    else {
        FILE *fptr;
        char buf[128] = {'\0'};
        char *cmd = NULL;

        if ((cmd = smp_calloc(strlen(name) + 16)) == NULL) return SMP_FAILURE;

        pid_t pid = -1;
        sprintf(cmd, "pidof %s", name);

        if((fptr = popen(cmd, "r")) != NULL) {
            if(fgets(buf, 128, fptr) != NULL) {
                pid = atoi(buf);
            }
        }

        pclose(fptr);
        smp_free(cmd);

        return pid;
    }
}

static SMP_VALUE smp_cpy_environs()
{
    u_char *new_addr = NULL;
    size_t size = smp_sys_env_last - environ[0];

    if ((new_addr = smp_calloc(size)) == NULL) return SMP_FAILURE;
    else {	/* 复制环境变量到新的地址上 */
        u_char *current = new_addr;
        u_char *p = current;
        int i = 0;

        /* 复制进程的环境变量 */
        for (i = 0; environ[i]; i++) {
            current = smp_strcpyn(current, (u_char *) environ[i], strlen(environ[i]));
            environ[i] = (char *) p;
            current++;
            p = current;
        }
    }

    return SMP_OK;
}

static SMP_VALUE smp_init_set_proc_title(int argc, char **argv)
{
    int i = 0;

    smp_sys_argv_last = argv[0];

    /* 解决argv[i]被修改后指向的非进程启动时的连续内存, 来自nginx */
    for (i = 0; i < argc; i++) {
        if (smp_sys_argv_last == argv[i]) {
            smp_sys_argv_last = argv[i] + strlen(argv[i]) + 1;	/* 加上1是为了补上最后的'\0' */
        }
    }

    smp_sys_env_last = smp_sys_argv_last;

    for (i = 0; environ[i]; i++) {
        if (smp_sys_env_last == environ[i]) {
            smp_sys_env_last = environ[i] + strlen(environ[i]) + 1;
        }
    }

    smp_sys_env_last--;

    return SMP_OK;
}

static SMP_VALUE smp_reset_proc_titel(const u_char *title, size_t tlen, int argc, char **argv)
{
    u_char *params = NULL;

    if (argc <= 1) {	/* 没有参数时, 直接覆盖 */
        memset(argv[0], '\0', smp_sys_env_last - argv[0]);
        memcpy(argv[0], title, tlen);
        return SMP_OK;
    }else {
        u_char *current = NULL;
        u_char *p = NULL;
        size_t size = smp_sys_argv_last - argv[1];
        int i = 0;

        if ((params = malloc(size)) == NULL) return 0;
        current = params;
        p = current;

        for (i = 1; i < argc; i++) {	/* 备份程序参数 */
            current = smp_strcpyn(current, (u_char *) argv[i], strlen(argv[i]));
            argv[i] = (char *) p;
            current++;
            p = current;
        }

        memset(argv[0], '\0', smp_sys_env_last - argv[0]);	/* 一致性全部置 '\0' */
        current = smp_strcpyn((u_char *) argv[0], title, tlen);

        current++;	/* 加上1是为了补上最后的 '\0' */
        p = current;

        for (i = 1; i < argc; i++) {
            current = smp_strcpyn(current, (const u_char *) argv[i], strlen(argv[i]));
            argv[i] = (char *) p;
            current++;
            p = current;
        }
    }

    return SMP_OK;
}

SMP_VALUE smp_set_proc_titel(const char *title, int argc, char **argv)
{
    size_t tlen = 0;

    if (title == NULL) return SMP_FAILURE;

    tlen = strlen(title);
    if (tlen <= strlen(argv[0])) {
        memset(argv[0], '\0', strlen(argv[0]));
        memcpy(argv[0], title, tlen);
        return SMP_OK;
    }

    smp_init_set_proc_title(argc, argv);
    smp_cpy_environs();
    smp_reset_proc_titel((u_char *) title, tlen, argc, argv);

    return SMP_OK;
}
