#include "../../smpcil.h"
#include "../../smp_mpool/smp_mpool.h"
#include "../../smp_linux/smp_linux.h"
#include "../smp_stl.h"
#include "smp_link.h"


#define SMP_LINK_POOL_SIZE  (1024)



static struct smp_link_node_s *smp_lnode_create(struct smp_link_s *link);
static void __smp_link_init(struct smp_link_s *link, smp_lnode_data_free *free_func);
static SMP_STATUS __smp_link_insert(struct smp_link_s *link, struct smp_link_node_s *node);
static void smp_link_node_recover(struct smp_link_s *link, struct smp_link_node_s *node);
static struct smp_link_node_s *smp_link_rm_node(struct smp_link_s *link,
                                                struct smp_link_node_s *node);


static struct smp_link_node_s *smp_lnode_create(struct smp_link_s *link)
{
    struct smp_link_node_s *node = NULL;

    if (link->recover != NULL) {
        node = link->recover;
        link->recover = node->next;
        node->next = NULL;
        return node;
    }

    node = smp_mpalloc((smp_pool_t *) link->pool, sizeof(struct smp_link_node_s));

    if (node == NULL) return NULL;

    node->data = NULL;
    node->prev = NULL;
    node->next = NULL;

    return node;
}


static void __smp_link_init(struct smp_link_s *link, smp_lnode_data_free *free_func)
{
    if (smp_unlikely(link == NULL)) return;

    link->node = NULL;
    link->tail = NULL;
    link->quotes = 1;
    link->size = 0;
    link->data_free = free_func;

    pthread_cond_init(&link->cond, NULL);
    pthread_mutex_init(&link->mutex, NULL);
}


struct smp_link_s *smp_link_init(smp_lnode_data_free *free_func)
{
    struct smp_link_s *link = NULL;
    smp_pool_t *pool = NULL;

    if ((pool = smp_mpool_create(SMP_LINK_POOL_SIZE)) == NULL) return NULL;

    if ((link = smp_mpalloc(pool, sizeof(struct smp_link_s))) == NULL) {
        smp_mpdestory(pool);
        return NULL;
    }

//    link->appoint = appoint;
    link->pool = pool;
    __smp_link_init(link, free_func);

    return link;
}


static SMP_STATUS __smp_link_insert(struct smp_link_s *link, struct smp_link_node_s *node)
{
    if (link->node == NULL) {
        link->node = node;
        link->tail = node;
    }else {
        link->tail->next = node;
        node->prev = link->tail;
        link->tail = node;
    }

    link->size++;

    return SMP_OK;
}


struct smp_link_node_s *smp_link_insert(struct smp_link_s *link, void *data)
{
    struct smp_link_node_s *node = NULL;

    if (smp_unlikely(link == NULL || data == NULL)) return NULL;

    if ((node = smp_lnode_create(link)) == NULL) return NULL;

    __smp_link_insert(link, node);
    node->data = data;

    return node;
}


static void smp_link_node_recover(struct smp_link_s *link, struct smp_link_node_s *node)
{
    node->data = node->prev = node->next = NULL;

    if (link->recover == NULL) link->recover = node;
    else {
        node->next = link->recover;
        link->recover = node;
    }
}


static struct smp_link_node_s *smp_link_rm_node(struct smp_link_s *link,
                                                struct smp_link_node_s *node)
{
    if (node == link->node) {
        if (link->node->next == NULL) {
            link->node = link->tail = NULL;
        }else {
            link->node = link->node->next;
            link->node->prev = NULL;
        }
        node->prev = node->next = NULL;
        link->size--;
        return node;
    }

    if (node == link->tail) {
        if (link->tail == link->node) {
            link->tail = link->node = NULL;
        }else {
            link->tail = link->tail->prev;
            link->tail->next = NULL;
        }
        node->prev = node->next = NULL;
        link->size--;
        return node;
    }

    node->prev->next = node->next;
    node->next->prev = node->prev;
    node ->next = node->prev = NULL;
    link->size--;
    return node;
}


void *smp_link_node_rm_athead(struct smp_link_s *link)
{
    struct smp_link_node_s *node =NULL;
    void *data = NULL;

    if (smp_unlikely(link == NULL)) return NULL;

    node = link->node;
    if (node == NULL) return NULL;

    node = smp_link_rm_node(link, node);
    data = node->data;
    smp_link_node_recover(link, node);

    return data;
}


void *smp_link_node_rm_attail(struct smp_link_s *link)
{
    struct smp_link_node_s *node =NULL;
    void *data = NULL;

    if (smp_unlikely(link == NULL)) return NULL;

    node = link->tail;
    if (node == NULL) return NULL;

    node = smp_link_rm_node(link, node);
    data = node->data;
    smp_link_node_recover(link, node);

    return data;
}


void *smp_link_node_rm(struct smp_link_s *link, void *data, smp_link_data_cmp *cmp_func)
{
    struct smp_link_node_s *node = NULL;
    void *node_data = NULL;

    if (smp_unlikely(link == NULL || data == NULL)) return NULL;

    node = link->node;
    while (node != NULL) {
        if (cmp_func(node->data, data) == SMP_CMP_EQUAL) break;
        node = node->next;
    }

    if (node == NULL) return NULL;
    else node = smp_link_rm_node(link, node);

    node_data = node->data;
    smp_link_node_recover(link, node);

    return node_data;
}


inline size_t smp_link_size(struct smp_link_s *link)
{
    if (smp_unlikely(link == NULL)) return 0;
    return link->size;
}


inline SMP_VALUE smp_link_quote_inc(struct smp_link_s *link)
{
    if (smp_unlikely(link == NULL)) return 0;

    link->quotes++;

    return link->quotes;
}


inline SMP_VALUE smp_link_quote_dec(struct smp_link_s *link)
{
    if (smp_unlikely(link == NULL)) return 0;

    link->quotes--;

    return link->quotes;
}


SMP_STATUS smp_link_traversal(struct smp_link_s *link, smp_link_visit *visit)
{
    struct smp_link_node_s *node = NULL;

    if (smp_unlikely(link == NULL || visit == NULL)) return SMP_OK;

    node = link->node;
    while (node != NULL) {
        visit(node->data);
        node = node->next;
    }

    return SMP_OK;
}


inline SMP_STATUS smp_link_destory(struct smp_link_s *link)
{
    if (smp_unlikely(link == NULL)) return SMP_OK;

    smp_link_traversal(link, link->data_free);

    return smp_mpdestory((smp_pool_t *) link->pool);
}
