#include "../../smpcil.h"
#include "../../smp_mpool/smp_mpool.h"
#include "../../smp_linux/smp_error/smp_error.h"
#include "smp_array.h"



#define SMP_ARRAY_POOL_SIZE     (2048)
#define SMP_ARRAY_INDEX_AMT     (32)
#define SMP_ARRAY_INDEX_INC     (4)


static SMP_STATUS smp_array_index_init(struct smp_array_s *array);
static SMP_STATUS smp_array_index_realloc(struct smp_array_s *array);
static inline SMP_STATUS smp_array_expand(struct smp_array_s *array);


static SMP_STATUS smp_array_index_init(struct smp_array_s *array)
{
    int i = 0;
    size_t size = sizeof(u_char *) * SMP_ARRAY_INDEX_AMT;

    if (smp_unlikely(array == NULL || array->pool == NULL)) return SMP_FAILURE;

    array->index = smp_mpalloc((smp_pool_t *) array->pool, size);
    if (array->index == NULL) return SMP_FAILURE;

    for (i = 0; i < SMP_ARRAY_INDEX_AMT; i++) {
        array->index[i] = NULL;
    }

    array->index_amt = SMP_ARRAY_INDEX_AMT;

    return SMP_OK;
}


struct smp_array_s *smp_array_init(uint elem_size, uint unit_elem_amt)
{
    struct smp_array_s *array = NULL;
    smp_pool_t *pool = NULL;
    void *start = NULL;
    uint size = elem_size * unit_elem_amt;

    if (smp_unlikely(size <= 0)) {
        smp_set_error("elem size can't be 0 or elem amt can't be 0");
        return NULL;
    }

    pool = smp_mpool_create(SMP_ARRAY_POOL_SIZE);
    if (pool == NULL) return NULL;

    array = smp_mpalloc(pool, sizeof(struct smp_array_s));
    if (array == NULL) goto smp_bad_init;

    array->pool = pool;

    if (smp_array_index_init(array) == SMP_FAILURE) goto smp_bad_init;
    if ((start = smp_mpalloc(pool, size)) == NULL) goto smp_bad_init;

    array->index[0] = start;
    array->index_used = 1;
    array->curr_point = 0;
    array->elem_size = elem_size;
    array->unit_elem_amt = unit_elem_amt;

    return array;

smp_bad_init:
    smp_mpdestory(pool);
    return NULL;
}


static SMP_STATUS smp_array_index_realloc(struct smp_array_s *array)
{
    size_t size = 0;
    void *index = NULL;

    if (smp_unlikely(array == NULL)) return SMP_FAILURE;

    size = sizeof(u_char *) * (array->index_used + SMP_ARRAY_INDEX_INC);

    index = smp_mpalloc((smp_pool_t *) array->pool, size);
    if (index == NULL) return SMP_FAILURE;

    memcpy(index, array->index, array->index_used * sizeof(u_char *));

    array->index = index;
    array->index_amt += SMP_ARRAY_INDEX_INC;

    return SMP_OK;
}


static inline SMP_STATUS smp_array_expand(struct smp_array_s *array)
{
    if (array->curr_point >= array->unit_elem_amt) {

        if (array->index_used == array->index_amt) {
            if (smp_array_index_realloc(array) == SMP_FAILURE)
                return SMP_FAILURE;
        }

        size_t size = array->elem_size * array->unit_elem_amt;

        void *arr = smp_mpalloc((smp_pool_t *) array->pool, size);
        if (arr == NULL) return SMP_FAILURE;

        array->index_used++;
        array->index[array->index_used - 1] = arr;
        array->curr_point = 0;
    }

    return SMP_OK;
}


void *smp_array_push(struct smp_array_s *array)
{
    u_char *start = NULL;

    if (smp_unlikely(array == NULL || array->index == NULL)) return NULL;

    if (smp_array_expand(array) == SMP_FAILURE) return NULL;

    start = array->index[array->index_used - 1] + array->curr_point * array->elem_size;

    array->curr_point++;

    return (void *)start;
}


SMP_VALUE smp_array_size(struct smp_array_s *array)
{
    if (smp_unlikely(array == NULL)) return 0;

    return (array->curr_point + (array->index_used - 1) * array->unit_elem_amt);
}


void *smp_array_elem_n(struct smp_array_s *array, uint i)
{
    i++;
    uint n = i % array->unit_elem_amt - 1;
    uint idx = i / array->unit_elem_amt;

    if (smp_unlikely(array == NULL || array->index == NULL)) return NULL;

    if (idx >= array->index_used) {
        smp_set_error("array Overflow");
        return NULL;
    }

    if (array->index[idx] != NULL) {
        if (n >= array->curr_point) {
            smp_set_error("array Overflow");
            return NULL;
        }
        return (void *) (array->index[idx] + array->elem_size * n);
    }

    return NULL;
}


SMP_STATUS smp_array_traversal(struct smp_array_s *array, smp_visit_ary_elem *trave)
{
    uint idx = 0;
    uint i = 0;
    u_char *curr_point = NULL;
    uint tmp = 0;

    if (smp_unlikely(array == NULL)) return SMP_OK;

    for (idx = 0; idx < array->index_used; idx++) {
        if (idx == array->index_used - 1) tmp = array->curr_point;
        else tmp = array->unit_elem_amt;

        for (i = 0; i < tmp; i++) {
            curr_point = array->index[idx] + array->elem_size * i;
            trave(curr_point);
        }
    }

    return SMP_OK;
}


SMP_STATUS smp_array_destory(struct smp_array_s *array,
                             smp_destory_ary *free_elems_func)
{
    if (smp_unlikely(array == NULL)) return SMP_OK;

    if (free_elems_func != NULL) {
        smp_array_traversal(array, free_elems_func);
    }

    if (array->pool != NULL)
        return smp_mpdestory((smp_pool_t *) array->pool);

    return SMP_OK;
}

