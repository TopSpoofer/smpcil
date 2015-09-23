#include "../../smpcil.h"
#include "../smp_error/smp_error.h"
#include "./smp_signal.h"


/**
 *
 *  向pid进程发送sig信号
 *
 * */

inline SMP_STATUS smp_kill(pid_t pid, int sig)
{
    if (kill(pid, sig) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


/**
 *
 *  向pid进程发送sig信号
 *
 * */

inline SMP_STATUS smp_sig_send(pid_t pid, int sig)
{
    if (pid <= 0) {
        smp_set_error("pid must > 0");
        return SMP_FAILURE;
    }

    return smp_kill(pid, sig);
}


/**
 *
 *  向进程组发送sig信号
 *
 * */

inline SMP_STATUS smp_sig_send2group(int sig)
{
    return smp_kill(0, sig);
}


/**
 *
 *  向系统所有进程发送sig信号
 *
 * */

inline SMP_STATUS smp_sig_send2allproc(int sig)
{
    return smp_kill(-1, sig);
}


/**
 *
 *  将信号sig发送给进程组号(PGID)为pid绝对值的所有进程
 *
 * */

inline SMP_STATUS smp_sig_send2PGID(pid_t pid, int sig)
{
    if (pid > 0) pid = -pid;
    return smp_kill(pid, sig);
}


/**
 *
 *  安装信号
 *
 * */

inline SMP_STATUS smp_sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{
    if (sigaction(sig, act, oact) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


/**
 *
 *  安装一个信号sig， 当捕获到sig信号的时候，执行handle函数。
 *  默认安装是，flags为0
 *
 * */

SMP_STATUS smp_sig_install_def(int sig, smp_sa_handle handle)
{
    struct sigaction act, oact;

    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handle;

    return smp_sigaction(sig, &act, &oact);
}


SMP_STATUS smp_sig_install(int sig, smp_sa_handle handle, int flags)
{
    struct sigaction act, oact;

    act.sa_flags = flags;
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handle;

    return smp_sigaction(sig, &act, &oact);
}


inline SMP_BOOLEAN smp_sig_ismember(sigset_t *set, int sig)
{
    if (smp_unlikely(sig < 0 || set == NULL)) {
        smp_set_error("source Unavailable, set is null");
        return SMP_FALSE;
    }

    if (sigismember(set, sig) == 1) return SMP_TRUE;

    return SMP_FALSE;
}


/**
 *
 *  将信号sig从set中移除
 *
 * */

SMP_STATUS smp_sig_remove_from_set(int sig, sigset_t *set)
{
    if (smp_unlikely(sig < 0 || set == NULL)) {
        smp_set_error("source Unavailable, set is null");
        return SMP_FAILURE;
    }

    if (smp_sig_ismember(set, sig) == SMP_TRUE) {
        if (sigdelset(set, sig) == -1) {
            smp_set_error(strerror(errno));
            return SMP_FAILURE;
        }

        return  SMP_OK;
    }

    return SMP_OK;
}


/**
 *
 *  加入一个信号sig到信号集中
 *
 * */

SMP_STATUS smp_sig_add2set(int sig, sigset_t *set)
{
    if (smp_unlikely(sig < 0 || set == NULL)) {
        smp_set_error("source Unavailable, set is null");
        return SMP_FAILURE;
    }

    if (sigaddset(set, sig) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


/**
 *
 *  将set信号集初始化，然后把所有的信号加入到此信号集中
 *
 * */

SMP_STATUS smp_sig_setfill(sigset_t *set)
{
    if (smp_unlikely(set == NULL)) {
        smp_set_error("source Unavailable, set is null");
        return SMP_FAILURE;
    }

    if (sigfillset(set) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


/**
 *
 *  清空信号集set
 *
 * */

SMP_STATUS smp_sig_setempty(sigset_t *set)
{
    if (smp_unlikely(set == NULL)) {
        smp_set_error("source Unavailable, set is null");
        return SMP_FAILURE;
    }

    if (sigemptyset(set) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


/**
 *
 *  对 sigprocmask的封装
 *
 * */

inline SMP_STATUS smp_sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    if (sigprocmask(how, set, oldset) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


/**
 *
 *  将set信号集添加到当前进程屏蔽的信号集中。
 *  将旧的进程信号集保存到oldset中
 *
 * */

inline SMP_STATUS smp_sig_block(const sigset_t *set, sigset_t *oldset)
{
    return smp_sigprocmask(SIG_BLOCK, set, oldset);
}


/**
 *
 *  将信号集set从进程当前信号集中删除。
 *  将旧的进程信号集保存到oldset中
 *
 * */

inline SMP_STATUS smp_sig_unblock(const sigset_t *set, sigset_t *oldset)
{
    return smp_sigprocmask(SIG_UNBLOCK, set, oldset);
}


/**
 *
 *  将当前进程的信号集设置为set。
 *  不管以前设置了什么，当前都为set。
 *
 * */

inline SMP_STATUS smp_sig_set_procmask(const sigset_t *set)
{
    return smp_sigprocmask(SIG_SETMASK, set, NULL);
}


/**
 *
 *  获取当前进程的信号集，保存到set中
 *
 * */

inline SMP_STATUS smp_sig_get_procmask(sigset_t *set)
{
    return smp_sigprocmask(SIG_SETMASK, NULL, set);
}


/**
 *
 *  进程将屏蔽所有信号，除了系统规定不能被屏蔽的。
 *
 * */

SMP_STATUS smp_sig_mask_allset()
{
    sigset_t set;

    memset(&set, '\0', sizeof(sigset_t));

    if (smp_sig_setfill(&set) == SMP_FAILURE) return SMP_FAILURE;

    return smp_sig_set_procmask(&set);
}
