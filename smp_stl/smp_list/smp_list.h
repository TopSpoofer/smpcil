#ifndef SMP_LIST_H
#define SMP_LIST_H

#include <sys/types.h>


typedef int (smp_visit_list_elem) (void *);


typedef struct smp_lpart_s {
    void *elems;                    /* 元素真正的起始地址 */
    struct smp_lpart_s *next;
    uint elem_mnt;                  /* elems's amount， 元素的数量 */
}smp_lpart_t;


typedef struct smp_list_s {
    struct smp_lpart_s head;
    size_t elem_size;               /* 每个元素的size */
    struct smp_lpart_s *tail;
    void *mpool;                    /* 内存池 */
    uint part_elem_maxmnt;          /* 每个part可以保存的最大元素个数 */
}smp_list_t;


struct smp_list_s *smp_list_create(size_t elem_size, uint part_elem_maxmnt);
void *smp_list_push(struct smp_list_s *list);
SMP_STATUS smp_list_destory(struct smp_list_s *list);
void smp_list_traversal(struct smp_list_s *list, smp_visit_list_elem *visit_func);

#endif // SMP_LIST_H
