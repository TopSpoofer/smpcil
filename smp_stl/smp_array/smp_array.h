#ifndef SMP_ARRAY_H
#define SMP_ARRAY_H

#include <sys/types.h>

typedef struct smp_array_s {
    uint unit_elem_amt;             /* 每个arrays里可以容纳的元素的个数 */
    uint elem_size;                 /* 每个元素的大小 */
    uint index_amt;                 /* 索引数组的数量 */
    uint index_used;                /* 被使用的索引数组的数量 */
    uint curr_point;                /* 当前分配的地址 */
    u_char **index;                 /* 用来找到arrays索引 */
    void *pool;                     /* 这个数组的内存池 */
}smp_array_t;


typedef int (smp_visit_ary_elem) (void *);
typedef int (smp_destory_ary) (void *);

struct smp_array_s *smp_array_init(uint elem_size, uint unit_elem_amt);
void *smp_array_push(struct smp_array_s *array);
SMP_VALUE smp_array_size(struct smp_array_s *array);
void *smp_array_elem_n(struct smp_array_s *array, uint i);
SMP_STATUS smp_array_traversal(struct smp_array_s *array, smp_visit_ary_elem *trave);
SMP_STATUS smp_array_destory(struct smp_array_s *array,
                             smp_destory_ary *free_elems_func);


#endif // SMP_ARRAY_H
