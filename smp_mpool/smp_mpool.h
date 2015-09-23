#ifndef SMP_MPOOL_H
#define SMP_MPOOL_H


#include <malloc.h>
#include <sys/types.h>

#define SMP_ALLOC_IN_POOL   (1)         /* the mem is alloc in pool */


#define SMP_POOL_ALIGNMENT sizeof(unsigned long)
#define SMP_PAGE_SIZE ((size_t) getpagesize())


#define smp_align(d, a) (((d) + (a - 1)) & ~(a - 1))
#define smp_align_ptr(p, a) (u_char *)\
    (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))


#define smp_memzero(buf,size) (void)memset(buf, 0, size)
#define smp_memset(buf, c, size) (void)memset(buf, c, size)
#define smp_free free


typedef struct smp_pool_data_s {
    u_char *last;
    u_char *end;
    struct smp_pool_s *next;
    unsigned int failure;
}smp_pool_data_t;


typedef struct smp_big_pool_s {
    struct smp_big_pool_s *next;
    void *alloc;
}smp_big_pool_t;


typedef struct smp_pool_s {
    smp_pool_data_t data;
    size_t max;
    struct smp_pool_s *current;
    struct smp_big_pool_s *big_pool;
}smp_pool_t;



void *smp_alloc(size_t size);
void *smp_calloc(size_t size);
void *smp_memalign(size_t alignment, size_t size);


void *smp_mpalloc(struct smp_pool_s *pool, size_t size);
void *smp_mpalloc_block(struct smp_pool_s *pool, size_t size);
void *smp_mpalloc_bigpool(struct smp_pool_s *pool, size_t size);
void *smp_mpool_create(size_t size);
SMP_STATUS smp_mpfree_bigpool(struct smp_pool_s *pool, void *p);
SMP_STATUS smp_mpdestory(struct smp_pool_s *pool);

#endif // SMP_MPOOL_H
