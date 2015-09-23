#include "../../smpcil.h"
#include "../../smp_linux/smp_error/smp_error.h"
#include "../../smp_mpool/smp_mpool.h"
#include "./smp_list.h"


#define SMP_LIST_POOL_SIZE  (4096)


static void smp_list_visit_part_elems(smp_lpart_t *part, smp_visit_list_elem *visit_func);
static void smp_list_visit_parts(struct smp_list_s *list, smp_visit_list_elem *visit_func);
static void __smp_list_init(struct smp_list_s *list, smp_pool_t *pool, size_t esize, uint maxmnt);



static void __smp_list_init(struct smp_list_s *list, smp_pool_t *pool, size_t esize, uint maxmnt)
{
    if (smp_unlikely(list == NULL)) return;

    list->elem_size = esize;
    list->part_elem_maxmnt = maxmnt;
    list->tail = &list->head;
    list->head.elem_mnt = 0;
    list->head.next = NULL;
    list->mpool = (void *) pool;
}


struct smp_list_s *smp_list_init(void *pool, size_t elem_size, uint part_elem_maxmnt)
{
    struct smp_list_s *list = NULL;

    if (smp_unlikely(pool == NULL|| elem_size == 0 || part_elem_maxmnt == 0)) {
        smp_set_error("init list err, pool can't be void");
        return NULL;
    }

    list = smp_mpalloc((smp_pool_t *) pool, sizeof(struct smp_list_s));
    if (list == NULL) {
        smp_set_error("Allocation list err int mem pool");
        return NULL;
    }

    list->head.elems = smp_mpalloc(pool, elem_size * part_elem_maxmnt);
    if (list->head.elems == NULL) {
        smp_set_error("Allocation list head elems err in mem pool");
        return NULL;
    }

    __smp_list_init(list, pool, elem_size, part_elem_maxmnt);

    return list;
}


struct smp_list_s *smp_list_create(size_t elem_size, uint part_elem_maxmnt)
{
    smp_pool_t *pool = NULL;

    if ((pool = smp_mpool_create(SMP_LIST_POOL_SIZE)) == NULL) return NULL;

    return smp_list_init(pool, elem_size, part_elem_maxmnt);
}


void *smp_list_push(struct smp_list_s *list)
{
    void *elem = NULL;
    struct smp_lpart_s *last = NULL;

    last = list->tail;

    if (last->elem_mnt == list->part_elem_maxmnt) {
        last = smp_mpalloc(list->mpool, sizeof(struct smp_lpart_s));
        if (last == NULL) return NULL;

        last->elems = smp_mpalloc(list->mpool, list->elem_size * list->part_elem_maxmnt);
        if (last->elems == NULL) return NULL;

        last->elem_mnt = 0;
        last->next = NULL;

        list->tail->next = last;
        list->tail = last;
    }

    elem = (char *) last->elems + list->elem_size * last->elem_mnt;
    last->elem_mnt++;

    return elem;
}


static void smp_list_visit_part_elems(smp_lpart_t *part, smp_visit_list_elem *visit_func)
{
    uint i = 0;
    int *ptr = NULL;

    if (smp_unlikely(part == NULL)) return;

    ptr = (int *) part->elems;

    for (i = 0; i < part->elem_mnt; i++) {
        visit_func(ptr + i);
    }
}


static void smp_list_visit_parts(struct smp_list_s *list, smp_visit_list_elem *visit_func)
{
    struct smp_lpart_s *part = NULL;

    if (smp_unlikely(list == NULL)) return;

    part = &list->head;

    while (part != NULL) {
        smp_list_visit_part_elems(part, visit_func);
        part = part->next;
    }
}


inline void smp_list_traversal(struct smp_list_s *list, smp_visit_list_elem *visit_func)
{
    if (smp_unlikely(list == NULL || visit_func == NULL)) return;

    smp_list_visit_parts(list, visit_func);
}


SMP_STATUS smp_list_destory(struct smp_list_s *list)
{
    if (smp_unlikely(list == NULL || list->mpool == NULL)) return SMP_OK;

    if (list->mpool != NULL) return smp_mpdestory(list->mpool);

    return SMP_OK;
}
