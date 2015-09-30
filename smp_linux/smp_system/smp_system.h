#ifndef SMP_SYSTEM_H
#define SMP_SYSTEM_H
SMP_VALUE smp_sys_getpid_byname(const char *name);
SMP_VALUE smp_set_proc_titel(const char *title, int argc, char **argv);
#endif // SMP_SYSTEM_H
