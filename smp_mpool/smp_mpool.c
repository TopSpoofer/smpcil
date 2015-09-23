#include "../smpcil.h"
#include "../smp_linux/smp_error/smp_error.h"
#include "./smp_mpool.h"


void *smp_alloc(size_t size)
{
    void *p = NULL;
    p = malloc(size);
    if (p == NULL) smp_set_error(strerror(errno));

    return p;
}


void *smp_calloc(size_t size)
{
    void *p = NULL;

    p = smp_alloc(size);
    if (p != NULL)  smp_memzero(p, size);
    else smp_set_error(strerror(errno));

    return p;
}


void *smp_memalign(size_t alignment, size_t size)
{
    int err = 0;
    void *p = NULL;

    err = posix_memalign(&p, alignment, size);
    if (err) {
        smp_set_error(strerror(errno));
        p = NULL;
    }

    return p;
}


/**
 *
 *  alloc a mem in a pool.
 *
 * */

void *smp_mpalloc(struct smp_pool_s *pool, size_t size)
{
    u_char *m;
    struct smp_pool_s *p;

    if (size <= pool->max) {

        p = pool->current;

        do {
            m = smp_align_ptr(p->data.last, SMP_POOL_ALIGNMENT);
            if ((size_t) (p->data.end - m) >= size) {
                p->data.last = m + size;
                return m;
            }
            p = p->data.next;
        }while(p);

        return smp_mpalloc_block(pool, size);
    }

    return smp_mpalloc_bigpool(pool, size);
}


void *smp_mpalloc_block(struct smp_pool_s *pool, size_t size)
{
    u_char *m;
    size_t psize;
    struct smp_pool_s *p, *new_p, *current_p;

    psize = (size_t) (pool->data.end - (u_char*) pool);

    if ((m = smp_memalign(SMP_POOL_ALIGNMENT, psize)) == NULL) return NULL;

    new_p = (struct smp_pool_s *) m;

    new_p->data.end = m + psize;
    new_p->data.next = NULL;
    new_p->data.failure = 0;

    m += sizeof(struct smp_pool_data_s);
    m = smp_align_ptr(m, SMP_POOL_ALIGNMENT);   /* Align */
    new_p->data.last = m + size;

    current_p = pool->current;

    for (p = current_p; p->data.next; p = p->data.next) {
        /* if p->data.failure++ > 4, current pool space used out */
        if (p->data.failure++ > 4) current_p  = p->data.next;
    }

    p->data.next = new_p;
    pool->current = current_p ? current_p : new_p;

    return m;
}


void *smp_mpalloc_bigpool(struct smp_pool_s *pool, size_t size)
{
    void *p;
    int n = 0;
    struct smp_big_pool_s *big_p;

    if ((p = smp_calloc(size)) == NULL) return NULL;

    /* find a big pool struct for p */
    for (big_p = pool->big_pool; big_p; big_p = big_p->next) {
        if (big_p->alloc == NULL) {
            big_p->alloc = p;
            return p;
        }
        /* Avoid Access all list's node */
        if (n++ > 3) break; /* 4 is a empiric value */
    }

    big_p = smp_mpalloc(pool, sizeof(struct smp_big_pool_s));
    if (big_p == NULL) {
        smp_free(big_p);
        return NULL;
    }

    big_p->alloc = p;
    big_p->next = pool->big_pool;
    pool->big_pool = big_p;

    return p;
}


void *smp_mpool_create(size_t size)
{
    struct smp_pool_s *p;

    p = smp_memalign(SMP_POOL_ALIGNMENT, size);
    if (p == NULL) return NULL;

    p->data.last = (u_char *) p + sizeof(struct smp_pool_s);
    p->data.end = (u_char *) p + size;

    p->data.failure = 0;
    p->data.next = NULL;

    size = size - sizeof(struct smp_pool_s);
    /* if size > SMP_PAGE_SIZE, we alloc a block in big mpool */
    p->max = (size < SMP_PAGE_SIZE) ? size : (SMP_PAGE_SIZE);

    p->current = p;
    p->big_pool = NULL;

    return p;
}


/**
 *
 *  only free the big pool's alloc.
 *
 * */

SMP_STATUS smp_mpfree_bigpool(struct smp_pool_s *pool, void *p)
{
    struct smp_big_pool_s *big_p = NULL;

    for (big_p = pool->big_pool; big_p; big_p = big_p->next) {

        if (big_p->alloc == p) {
            smp_free(big_p->alloc);
            big_p->alloc = NULL;
            return SMP_OK;
        }
    }

    return SMP_OK;
}


static SMP_STATUS smp_bigpool_destory(struct smp_big_pool_s *bigpool)
{
    struct smp_big_pool_s *bigp_free = NULL;

    if (smp_unlikely(bigpool == NULL)) return SMP_OK;

    bigp_free = bigpool;

    while (bigp_free != NULL) {
        /* the big pool struct alloc by pool, we just free big pool's alloc */
        if (bigp_free->alloc != NULL) smp_free(bigp_free->alloc);
        bigp_free->alloc = NULL;
        bigp_free = bigp_free->next;
    }

    return SMP_OK;
}


SMP_STATUS smp_mpdestory(struct smp_pool_s *pool)
{
    struct smp_pool_s *data_pool = NULL;

    if (smp_unlikely(pool == NULL)) return SMP_OK;

    smp_bigpool_destory(pool->big_pool);

    data_pool = pool->data.next;
    while (data_pool != NULL) {
        pool->data.next = data_pool->data.next;
        data_pool->data.next = NULL;
        smp_free(data_pool);
        data_pool = pool->data.next;
    }

    smp_free(pool);

    return SMP_OK;
}
