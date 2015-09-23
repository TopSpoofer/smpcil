#include "../../smpcil.h"
#include "../../smp_linux/smp_error/smp_error.h"
#include "../../smp_mpool/smp_mpool.h"
#include "./smp_queue.h"



static inline void smp_qinit(struct smp_queue_s *queue, smp_qnode_free *free_node);
static inline void smp_qnode_init(struct smp_queue_node_s *qnode,
                                  smp_qdata_free *free_qdata, void *data);


inline SMP_VALUE smp_queue_quote_dec(struct smp_queue_s *queue)
{
    if (smp_unlikely(queue == NULL)) return 0;

    queue->quotes--;

    return queue->quotes;
}


inline SMP_VALUE smp_queue_quote_inc(struct smp_queue_s *queue)
{
    if (smp_unlikely(queue == NULL)) return 0;

    queue->quotes++;

    return queue->quotes;
}


inline SMP_VALUE smp_qlen(struct smp_queue_s *queue)
{
    if (smp_unlikely(queue == NULL)) return 0;
    return queue->qlen;
}


static inline void smp_qinit(struct smp_queue_s *queue, smp_qnode_free *free_node)
{
    if (smp_unlikely(queue == NULL)) return;

    queue->qlen = 0;
    queue->free_qnode = free_node;
    queue->quotes = smp_queue_quote_inc(queue);
    queue->elems = NULL;
    queue->tail = NULL;

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
}


struct smp_queue_s *smp_queue_create(smp_qnode_free *free_node)
{
    struct smp_queue_s *queue = NULL;

    if ((queue = smp_calloc(sizeof(struct smp_queue_s))) == NULL) {
        smp_set_error(strerror(errno));
        return NULL;
    }

    smp_qinit(queue, free_node);

    return queue;
}


static inline void smp_qnode_init(
        struct smp_queue_node_s *qnode,
        smp_qdata_free *free_qdata,
        void *data)
{
    if (smp_unlikely(qnode == NULL)) return;

    qnode->data = data;
    qnode->link.prev = NULL;
    qnode->link.next = NULL;
    qnode->free_qdata = free_qdata;
}


struct smp_queue_node_s *smp_qnode_create(smp_qdata_free *free_qdata, void *data)
{
    struct smp_queue_node_s *node = NULL;

    if ((node = smp_calloc(sizeof(struct smp_queue_node_s))) == NULL) {
        smp_set_error(strerror(errno));
        return NULL;
    }

    smp_qnode_init(node, free_qdata, data);

    return node;
}


/**
 *
 *  get the queue node by container.
 *
 * */

inline struct smp_queue_node_s *smp_cnt_qnode(smp_cnt_t *cnt)
{
    if ((cnt == NULL)) return NULL;

   return (struct smp_queue_node_s *) smp_container_data(cnt, smp_qnode_t, link);
}


inline SMP_BOOLEAN smp_qempty(struct smp_queue_s *queue)
{
    if ((queue == NULL)) return SMP_TRUE;

    if (queue->qlen > 0) return SMP_FALSE;
    return SMP_TRUE;
}


inline struct smp_queue_node_s *smp_qfront(struct smp_queue_s *queue)
{
    if ((queue == NULL)) return NULL;

    return smp_cnt_qnode(queue->elems);
}


inline struct smp_queue_node_s *smp_qrear(struct smp_queue_s *queue)
{
    if ((queue == NULL)) return NULL;

    return smp_cnt_qnode(queue->tail);
}


inline struct smp_queue_node_s *smp_qnode_prev(struct smp_queue_s *queue, struct smp_queue_node_s *node)
{
    if ((queue == NULL || node == NULL)) return NULL;

    if (node->link.prev == NULL) return smp_qrear(queue);
    else return smp_cnt_qnode(node->link.prev);
}


inline struct smp_queue_node_s *smp_qnode_next(struct smp_queue_s *queue, struct smp_queue_node_s *node)
{
    if ((queue == NULL || node == NULL)) return NULL;

    if (node->link.next == NULL) return smp_qfront(queue);
    else return smp_cnt_qnode(node->link.next);
}


inline SMP_STATUS smp_qenter(struct smp_queue_s *queue, struct smp_queue_node_s *node)
{
    if ((queue == NULL || node == NULL)) return SMP_FAILURE;

    if (queue->qlen == 0 && queue->elems == NULL) {
        queue->tail = queue->elems = &node->link;
        node->link.prev = node->link.next = NULL;
    }else {
        queue->tail->next = &node->link;
        node->link.prev = queue->tail;
        node->link.next = NULL;
        queue->tail = &node->link;
    }

    queue->qlen++;

    return SMP_OK;
}


struct smp_queue_node_s *smp_qinsert(struct smp_queue_s *queue, void *data, smp_qdata_free *free_qdata)
{
    struct smp_queue_node_s *node = NULL;

    if (smp_unlikely(queue == NULL || data == NULL)) return NULL;

    if ((node = smp_qnode_create(free_qdata, data)) == NULL) {
        smp_set_error("malloc queue node err\n");
        return NULL;
    }

    if (smp_qenter(queue, node) == SMP_OK) return node;
    else {
        smp_qnode_destory(node);
        return NULL;
    }
}


inline SMP_STATUS smp_qnode_destory(struct smp_queue_node_s *node)
{
    if ((node == NULL)) return SMP_OK;

    if (node->free_qdata != NULL) {
        if (smp_unlikely(node->data != NULL)) node->free_qdata(node->data);
    }

    smp_free(node);

    return SMP_OK;
}


inline struct smp_queue_node_s *smp_qnode_remove(struct smp_queue_s *queue)
{
    struct smp_container_s *node_cnt = NULL;

    if (smp_unlikely(queue == NULL || queue->elems == NULL || queue->qlen == 0)) return NULL;

    node_cnt = queue->elems;

    if (queue->tail == queue->elems) queue->elems = queue->tail = NULL;
    else {
       queue->elems = queue->elems->next;
       queue->elems->prev = NULL;
    }

    node_cnt->prev = node_cnt->next = NULL;
    queue->qlen--;

    return smp_cnt_qnode(node_cnt);
}


SMP_STATUS smp_qnode_delete(struct smp_queue_s *queue)
{
    struct smp_queue_node_s *node = NULL;

    if (smp_unlikely(queue == NULL || queue->elems == NULL || queue->qlen == 0)) return SMP_OK;

    if ((node = smp_qnode_remove(queue)) == NULL) return SMP_FAILURE;

    return smp_qnode_destory(node);
}


SMP_STATUS smp_qclean(struct smp_queue_s *queue)
{
    struct smp_queue_node_s *node = NULL;

    if (smp_unlikely(queue == NULL || queue->elems == NULL)) return SMP_OK;

    while ((node = smp_qnode_remove(queue)) != NULL) {
        if (smp_unlikely(queue->free_qnode == NULL)) break;
        queue->free_qnode(node);
    }

    queue->elems = queue->tail = NULL;
    queue->qlen = 0;

    return SMP_OK;
}


SMP_STATUS smp_qdestory(struct smp_queue_s *queue)
{
    if (smp_unlikely(queue == NULL)) return SMP_OK;

    if (smp_queue_quote_dec(queue) == 0) smp_qclean(queue);
    else {
        smp_set_error("other thread quoting this queue");
        return SMP_FAILURE;
    }

    smp_free(queue);

    return SMP_OK;
}


SMP_STATUS smp_queue_traversal(struct smp_queue_s *queue, smp_queue_visit *visit)
{
    struct smp_container_s *node_cnt = NULL;
    struct smp_queue_node_s *node = NULL;

    if (smp_unlikely(queue == NULL || queue->elems == NULL || queue->qlen == 0)) return SMP_OK;

    node_cnt = queue->elems;

    while (node_cnt != NULL) {
        node = smp_cnt_qnode(node_cnt);
        if (node != NULL) visit(queue, node);
        node_cnt = node_cnt->next;
    }

    return SMP_OK;
}


SMP_STATUS smp_qnode_traversal(struct smp_queue_s *queue, smp_qnode_visit *visit)
{
    struct smp_container_s *node_cnt = NULL;
    struct smp_queue_node_s *node = NULL;

    if (smp_unlikely(queue == NULL || queue->elems == NULL || queue->qlen == 0)) return SMP_OK;

    node_cnt = queue->elems;

    while (node_cnt != NULL) {
        node = smp_cnt_qnode(node_cnt);
        if (node != NULL) visit(node);
        node_cnt = node_cnt->next;
    }

    return SMP_OK;
}
