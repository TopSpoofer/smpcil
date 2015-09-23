#include "../smpcil.h"
#include "../smp_linux/smp_sig/smp_signal.h"
#include "../smp_linux/smp_error/smp_error.h"


static void smp_print_sigset(sigset_t set)
{
    int i = 0;
    for (i = 0; i < 1; i++) {
        printf("0x%8x\n", set.__val[i]);
        if ((i + 1) % 8 == 0) printf("\n");
    }
}

SMP_STATUS smp_test_sig_addset()
{
    sigset_t set;

    printf("after empty set :\n");
    smp_sig_setempty(&set);
    smp_print_sigset(set);

    printf("after add signo=2 :\n");
    smp_sig_add2set(2, &set);
    smp_print_sigset(set);

    printf("after add signo=10 :\n");
    smp_sig_add2set(10, &set);
    smp_print_sigset(set);

    printf("after fill all :\n");
    smp_sig_setfill(&set);
    smp_print_sigset(set);

    return SMP_OK;
}


void smp_test_sig_main()
{
    smp_test_sig_addset();
}
