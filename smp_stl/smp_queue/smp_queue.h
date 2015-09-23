#ifndef SMP_QUEUE_H
#define SMP_QUEUE_H

#include <sys/types.h>
#include "../smp_stl.h"


typedef int (smp_qdata_free) (void *);
typedef int (smp_qnode_free) (void *);
typedef int (smp_queue_visit) (void *, void *);
typedef int (smp_qnode_visit) (void *);


typedef struct smp_queue_node_s {
    smp_cnt_t link;
    void *data;
    smp_qdata_free *free_qdata;
}smp_qnode_t;


typedef struct smp_queue_s {
    size_t qlen;
    struct smp_container_s *elems;
    struct smp_container_s *tail;
    smp_qnode_free *free_qnode;             /* used to free the node's data */
    pthread_cond_t cond;                    /* process or thread block in here */
    pthread_mutex_t mutex;                  /* uesd in Multi-process or Multithreading env */
    unsigned int quotes;                    /* the amount of process or thread to quote this queue */
}smp_qhead_t, smp_queue_t;


inline SMP_VALUE smp_queue_quote_dec(struct smp_queue_s *queue);
inline SMP_VALUE smp_queue_quote_inc(struct smp_queue_s *queue);
inline SMP_VALUE smp_qlen(struct smp_queue_s *queue);
struct smp_queue_s *smp_queue_create(smp_qnode_free *free_node);
struct smp_queue_node_s *smp_qnode_create(smp_qdata_free *free_qdata, void *data);
inline struct smp_queue_node_s *smp_cnt_qnode(smp_cnt_t *cnt);
inline SMP_BOOLEAN smp_qempty(struct smp_queue_s *queue);
inline struct smp_queue_node_s *smp_qfront(struct smp_queue_s *queue);
inline struct smp_queue_node_s *smp_qrear(struct smp_queue_s *queue);
inline struct smp_queue_node_s *smp_qnode_prev(struct smp_queue_s *queue, struct smp_queue_node_s *node);
inline struct smp_queue_node_s *smp_qnode_next(struct smp_queue_s *queue, struct smp_queue_node_s *node);
inline SMP_STATUS smp_qenter(struct smp_queue_s *queue, struct smp_queue_node_s *node);
struct smp_queue_node_s *smp_qinsert(struct smp_queue_s *queue, void *data, smp_qdata_free *free_qdata);
inline struct smp_queue_node_s *smp_qnode_remove(struct smp_queue_s *queue);
inline SMP_STATUS smp_qnode_destory(struct smp_queue_node_s *node);
SMP_STATUS smp_qnode_delete(struct smp_queue_s *queue);
SMP_STATUS smp_qclean(struct smp_queue_s *queue);
SMP_STATUS smp_qdestory(struct smp_queue_s *queue);
SMP_STATUS smp_queue_traversal(struct smp_queue_s *queue, smp_queue_visit *visit);
SMP_STATUS smp_qnode_traversal(struct smp_queue_s *queue, smp_qnode_visit *visit);

#endif // SMP_QUEUE_H

