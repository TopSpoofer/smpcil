#ifndef SMPSLT_H
#define SMPSLT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stddef.h>
#include <pthread.h>
#include <netinet/in.h>


#define SMP_STATUS int
#define SMP_VALUE int
#define SMP_BOOLEAN int


#define SMP_OK          (0)
#define SMP_FAILURE     (-1)

#define SMP_TRUE        (1)
#define SMP_FALSE       (0)

#define SMP_ERR         (-2)
#define SMP_WARM        (-3)

#define SMP_DEBUG       (1)

# define smp_likely(x)	__builtin_expect(!!(x), 1)
# define smp_unlikely(x)	__builtin_expect(!!(x), 0)


#endif // SMPSLT_H
