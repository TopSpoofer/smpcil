#ifndef SMP_LINK_H
#define SMP_LINK_H


typedef int (smp_lnode_data_free) (void *);
typedef int (smp_link_data_cmp) (void *link_node_data, void *data);
typedef int (smp_link_visit) (void *);


typedef struct smp_link_node_s {
    struct smp_link_node_s *next;           /* 后一个结点的指针 */
    struct smp_link_node_s *prev;           /* 前一个结点的指针 */
    void *data;                             /* 指向数据的指针 */
}smp_link_node_t;


typedef struct smp_link_s {
    size_t size;                            /* 队列的结点数量 */
    struct smp_link_node_s *node;           /* 指向队列的第一个结点 */
    struct smp_link_node_s *tail;           /* 指向队列的尾部 */
    smp_lnode_data_free *data_free;         /* 释放队列数据的函数指针 */
    struct smp_link_node_s *recover;        /* 回收的队列结点 */
    void *pool;                             /* 内存池 */
    pthread_cond_t cond;                    /* 条件变量，用于多线程环境 */
    pthread_mutex_t mutex;                  /* 锁，用于多线程环境 */
    uint quotes;                            /* 这条队列被引用的次数 */
}smp_link_t;




struct smp_link_s *smp_link_init(smp_lnode_data_free *free_func);
struct smp_link_node_s *smp_link_insert(struct smp_link_s *link, void *data);
void *smp_link_node_rm_athead(struct smp_link_s *link);
void *smp_link_node_rm_attail(struct smp_link_s *link);
void *smp_link_node_rm(struct smp_link_s *link, void *data, smp_link_data_cmp *cmp_func);
size_t smp_link_size(struct smp_link_s *link);
SMP_VALUE smp_link_quote_inc(struct smp_link_s *link);
SMP_VALUE smp_link_quote_dec(struct smp_link_s *link);
SMP_STATUS smp_link_destory(struct smp_link_s *link);
SMP_STATUS smp_link_traversal(struct smp_link_s *link, smp_link_visit *visit);


#endif // SMP_LINK_H
