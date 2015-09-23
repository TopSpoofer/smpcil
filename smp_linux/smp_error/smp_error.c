#include "../../smpcil.h"
#include "./smp_error.h"


#define SMP_ERROR_SIZE  (2048)

static char *smp_errno = NULL;


void smp_print_error()
{
    printf("error : %s\n", smp_errno);
}


const char *smp_get_error()
{
    return smp_errno;
}


void smp_set_error(const char *error)
{
    int error_len = 0;

    if (smp_errno == NULL) {
        smp_errno = malloc(SMP_ERROR_SIZE);
        if (smp_errno == NULL) return;
    }

    error_len = (error_len = strlen(error)) > SMP_ERROR_SIZE - 1 ? SMP_ERROR_SIZE - 1 : error_len;

    memset(smp_errno, '\0', SMP_ERROR_SIZE);
    memcpy(smp_errno, error, error_len);
}
