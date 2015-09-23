#ifndef SMP_SIGNAL_H
#define SMP_SIGNAL_H

#include <signal.h>

typedef void (*smp_sa_handle)(int, siginfo_t *, void *);

inline SMP_STATUS smp_kill(pid_t pid, int sig);
inline SMP_STATUS smp_sig_send(pid_t pid, int sig);
inline SMP_STATUS smp_sig_send2group(int sig);
inline SMP_STATUS smp_sig_send2allproc(int sig);
inline SMP_STATUS smp_sig_send2PGID(pid_t pid, int sig);
inline SMP_STATUS smp_sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
SMP_STATUS smp_sig_install_def(int sig, smp_sa_handle handle);
SMP_STATUS smp_sig_install(int sig, smp_sa_handle handle, int flags);
inline SMP_BOOLEAN smp_sig_ismember(sigset_t *set, int sig);
SMP_STATUS smp_sig_add2set(int sig, sigset_t *set);
SMP_STATUS smp_sig_setfill(sigset_t *set);
SMP_STATUS smp_sig_setempty(sigset_t *set);
inline SMP_STATUS smp_sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
inline SMP_STATUS smp_sig_block(const sigset_t *set, sigset_t *oldset);
inline SMP_STATUS smp_sig_unblock(const sigset_t *set, sigset_t *oldset);
inline SMP_STATUS smp_sig_set_procmask(const sigset_t *set);
inline SMP_STATUS smp_sig_get_procmask(sigset_t *set);
SMP_STATUS smp_sig_mask_allset();

#endif // SMP_SIGNAL_H
