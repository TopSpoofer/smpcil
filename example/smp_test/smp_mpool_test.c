#include "../smpcil.h"
#include "../smp_mpool/smp_mpool.h"
#include "./smp_mpool_test.h"


SMP_STATUS smp_test_mpool_create()
{
    smp_pool_t *pool = NULL;
    char *p1 = NULL;
    char *p2 = NULL;
    char *p3 = NULL;

    if ((pool = smp_mpool_create(4098)) == NULL) return SMP_FAILURE;

    if ((p1 = smp_mpalloc(pool, 15)) == NULL) {
        printf("alloc p1 err\n");
        return SMP_FAILURE;
    }

    memset(p1, '\0', 15);
    memcpy(p1, "1234567890", strlen("1234567890"));
    printf("p1 addr %p,  p1 text: --%s--\n", p1, p1);

    if ((p2 = smp_mpalloc(pool, 15)) == NULL) {
        printf("alloc p2 err\n");
        return SMP_FAILURE;
    }

    memset(p2, '\0', 2000);
    memcpy(p2, "12345678901", strlen("12345678901"));
    printf("p2 addr %p,  p2 text: --%s--\n", p2, p2);

    if ((p3 = smp_mpalloc(pool, 4090)) == NULL) {
        printf("alloc p3 err\n");
        return SMP_FAILURE;
    }

    smp_mpdestory(pool);

    printf("finished tets\n");
    return SMP_OK;
}


void smp_test_mpool_main()
{
    smp_test_mpool_create();
}
