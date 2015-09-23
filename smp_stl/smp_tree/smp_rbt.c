#include "../../smpcil.h"
#include "../../smp_linux/smp_error/smp_error.h"
#include "../smp_queue/smp_queue.h"
#include "../../smp_mpool/smp_mpool.h"
#include "./smp_rbt.h"


#define SMP_RBT_RED     (1)
#define SMP_RBT_BLACK   (0)


static void smp_left_rotate(struct smp_rbt_s *rbt, struct smp_rbt_node_s *node);
static void smp_right_rotate(struct smp_rbt_s *rbt, struct smp_rbt_node_s *node);
static void smp_insert_fixup(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd);
static struct smp_rbt_node_s *smp_rbt_successor(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd);
static void smp_delete_fixup(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd);

static SMP_STATUS smp_rnode_enter_queue(smp_queue_t *queue_busy,
                                        smp_queue_t *queue_idle,
                                        struct smp_rbt_node_s *rnode);
static SMP_STATUS smp_do_rbt_clean(struct smp_rbt_s *rbt,
                                   smp_queue_t *queue_busy, smp_queue_t *queue_idle);
static SMP_STATUS smp_rbt_clean(struct smp_rbt_s *rbt);
static SMP_STATUS smp_rbt_free(struct smp_rbt_s *rbt);



/* - -- - - - - -- - - - - - red black tree - - - -- - - - - -- -  */


unsigned long smp_rbt_size(struct smp_rbt_s * rbt)
{
    if (smp_unlikely(rbt == NULL)) return SMP_OK;

    return rbt->size;
}


static void smp_left_rotate(struct smp_rbt_s *rbt, struct smp_rbt_node_s *node)
{
    struct smp_rbt_node_s *tmp = node->right;

    node->right = tmp->left;

    if (tmp->left != &rbt->nil) tmp->left->parent = node;

    tmp->parent = node->parent;

    if (node->parent == &rbt->nil)
        rbt->root = tmp;
    else if (node == node->parent->left)
        node->parent->left = tmp;
    else
        node->parent->right = tmp;

    tmp->left = node;
    node->parent = tmp;
}


static void smp_right_rotate(struct smp_rbt_s *rbt, struct smp_rbt_node_s *node)
{
    struct smp_rbt_node_s *tmp = node->left;

    node->left = tmp->right;

    if (tmp->right != &rbt->nil) tmp->right->parent = node;

    tmp->parent = node->parent;

    if (node->parent == &rbt->nil)
        rbt->root = tmp;
    else if (node == node->parent->left)
        node->parent->left = tmp;
    else
        node->parent->right = tmp;

    tmp->right = node;
    node->parent = tmp;
}


static void smp_insert_fixup(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd)
{
    struct smp_rbt_node_s *tmp;

    while (nd->parent->color == SMP_RBT_RED) {
        if (nd->parent == nd->parent->parent->left) {
            tmp = nd->parent->parent->right;

            if (tmp->color == SMP_RBT_RED) {
                nd->parent->color = tmp->color = SMP_RBT_BLACK;
                nd->parent->parent->color = SMP_RBT_RED;
                nd = nd->parent->parent;
            } else {

                if (nd == nd->parent->right) {
                    nd = nd->parent;
                    smp_left_rotate(rbt, nd);
                }

                nd->parent->color = SMP_RBT_BLACK;
                nd->parent->parent->color = SMP_RBT_RED;
                smp_right_rotate(rbt, nd->parent->parent);
            }
        } else {
            tmp = nd->parent->parent->left;

            if (tmp->color == SMP_RBT_RED) {
                nd->parent->color = tmp->color = SMP_RBT_BLACK;
                nd->parent->parent->color = SMP_RBT_RED;
                nd = nd->parent->parent;
            } else {

                if (nd == nd->parent->left) {
                    nd = nd->parent;
                    smp_right_rotate(rbt, nd);
                }

                nd->parent->color = SMP_RBT_BLACK;
                nd->parent->parent->color = SMP_RBT_RED;
                smp_left_rotate(rbt, nd->parent->parent);
            }
        }
    }

    rbt->root->color = SMP_RBT_BLACK;
}


static void smp_delete_fixup(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd)
{
    struct smp_rbt_node_s *tmp = &rbt->nil;

    while (nd != rbt->root && nd->color == SMP_RBT_BLACK)
        if (nd == nd->parent->left) {
            tmp = nd->parent->right;

            if (tmp->color == SMP_RBT_RED) {
                tmp->color = SMP_RBT_BLACK;
                nd->parent->color = SMP_RBT_RED;
                smp_left_rotate(rbt, nd->parent);
                tmp = nd->parent->right;
            }

            if (tmp->left->color == SMP_RBT_BLACK && tmp->right->color == SMP_RBT_BLACK) {
                tmp->color = SMP_RBT_RED;
                nd = nd->parent;
            } else {

                if (tmp->right->color == SMP_RBT_BLACK) {
                    tmp->left->color = SMP_RBT_BLACK;
                    tmp->color = SMP_RBT_RED;
                    smp_right_rotate(rbt, tmp);
                    tmp = nd->parent->right;
                }

                tmp->color = nd->parent->color;
                nd->parent->color = SMP_RBT_BLACK;
                tmp->right->color = SMP_RBT_BLACK;
                smp_left_rotate(rbt, nd->parent);
                nd = rbt->root; /* end while */
            }
        } else {
            tmp = nd->parent->left;

            if (tmp->color == SMP_RBT_RED) {
                tmp->color = SMP_RBT_BLACK;
                nd->parent->color = SMP_RBT_RED;
                smp_right_rotate(rbt, nd->parent);
                tmp = nd->parent->left;
            }

            if (tmp->right->color == SMP_RBT_BLACK && tmp->left->color == SMP_RBT_BLACK) {
                tmp->color = SMP_RBT_RED;
                nd = nd->parent;
            } else {

                if (tmp->left->color == SMP_RBT_BLACK) {
                    tmp->right->color = SMP_RBT_BLACK;
                    tmp->color = SMP_RBT_RED;
                    smp_left_rotate(rbt, tmp);
                    tmp = nd->parent->left;
                }

                tmp->color = nd->parent->color;
                nd->parent->color = SMP_RBT_BLACK;
                tmp->left->color = SMP_RBT_BLACK;
                smp_right_rotate(rbt, nd->parent);
                nd = rbt->root; /* end while */
            }
        }

    nd->color = SMP_RBT_BLACK;
}



/* - - - -- - -- - -- - -  insert and find and create rbt node  -- - - - - - -- - -- */


SMP_STATUS smp_rnode_insert(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd)
{
    struct smp_rbt_node_s *tmp = &rbt->nil, *itor = rbt->root;

    if (smp_unlikely(nd == NULL)) return SMP_FAILURE;

    while (itor != &rbt->nil) {
        tmp = itor;
        if ((rbt->key_cmp) (itor->key, nd->key) > 0) itor = itor->left;
        else itor = itor->right;
    }

    nd->parent = tmp;

    if (tmp == &rbt->nil) rbt->root = nd;
    else {
        if ((rbt->key_cmp) (tmp->key, nd->key) > 0) tmp->left = nd;
        else tmp->right = nd;
    }

    nd->left = nd->right = &rbt->nil;
    nd->color = SMP_RBT_RED;
    smp_insert_fixup(rbt, nd);
    rbt->size++;

    return SMP_OK;
}


/**
 *
 *  find_node and delete_node are not safe
 *
 *  delete node may return NULL.
 *
 * */

struct smp_rbt_node_s *smp_rnode_find(struct smp_rbt_s *rbt, void *key)
{
    struct smp_rbt_node_s *nd = &rbt->nil;
    int ret = 0;
    nd = rbt->root;

    while (nd != &rbt->nil) {
        ret = (rbt->key_cmp) (nd->key, key);
        if (ret > 0) {
            nd = nd->left;
            continue;
        }

        if (ret < 0) {
            nd = nd->right;
            continue;
        }

        if (ret == 0) return nd;
    }

    return NULL;
}


struct smp_rbt_node_s *smp_rnode_min(struct smp_rbt_s *rbt)
{
    struct smp_rbt_node_s *tmp;

    tmp = rbt->root;

    if (tmp == &rbt->nil) return NULL;

    while (tmp->left != &rbt->nil) {
        tmp = tmp->left;
    }

    if (tmp == &rbt->nil) return NULL;

    return tmp;
}


struct smp_rbt_node_s *smp_rnode_max(struct smp_rbt_s *rbt)
{
    struct smp_rbt_node_s *tmp;

    tmp = rbt->root;

    if (tmp == &rbt->nil) return NULL;

    while (tmp->right != &rbt->nil) {
        tmp = tmp->right;
    }

    if (tmp == &rbt->nil) return NULL;

    return tmp;
}


struct smp_rbt_node_s *smp_rnode_create(void *key)
{
    struct smp_rbt_node_s *node = NULL;

    if ((node = smp_calloc(sizeof(struct smp_rbt_node_s))) == NULL) return NULL;

    node->carrier   = NULL;
    node->color     = SMP_RBT_BLACK;
    node->key       = key;
    node->left      = NULL;
    node->right     = NULL;
    node->parent    = NULL;

    return node;
}


struct smp_rbt_s *smp_rbt_create(smp_rbt_keycmp *key_cmp, void *free_key)
{
    struct smp_rbt_s *rbt = smp_calloc(sizeof(struct smp_rbt_s));
    if (rbt == NULL) return NULL;

    rbt->key_cmp    = key_cmp;
    rbt->size       = 0;
    rbt->free_key   = free_key;
    rbt->nil.parent = &(rbt->nil);
    rbt->nil.left   = &(rbt->nil);
    rbt->nil.right  = &(rbt->nil);
    rbt->nil.color  = SMP_RBT_BLACK;
    rbt->nil.key    = NULL;
    rbt->root       = &rbt->nil;

    pthread_mutex_init(&rbt->mutex, NULL);
    pthread_cond_init(&rbt->cond, NULL);

    return rbt;
}


/* - - - -- - -- - - - -- - -  remove and destory rbt node -- - - - - - - -- - -- */


static struct smp_rbt_node_s *smp_rbt_successor(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd)
{
    struct smp_rbt_node_s *min = &rbt->nil;

    if (nd->right != &rbt->nil) {
        min = nd->right;

        while (min->left != &rbt->nil)
            min = min->left;

        return min;
    }

    min = nd->parent;

    while ((min != &rbt->nil) && (nd == min->right)) {
        nd = min;
        min = min->parent;
    }

    return min;
}


/**
 *
 *  free node, return val
 *
 * */

struct smp_rbt_node_s *smp_rnode_remove(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd)
{
    void *key = NULL;
    struct smp_rbt_node_s *tmp, *itor;

    if (smp_unlikely(nd == NULL || rbt == NULL)) return NULL;

    key = nd->key;

    if (nd->left == &rbt->nil || nd->right == &rbt->nil) tmp = nd;
    else tmp = smp_rbt_successor(rbt, nd);

    if (tmp->left != &rbt->nil) itor = tmp->left;
    else itor = tmp->right;

    itor->parent = tmp->parent;

    if (tmp->parent == &rbt->nil)
        rbt->root = itor;
    else {
        if (tmp == tmp->parent->left) tmp->parent->left = itor;
        else tmp->parent->right = itor;
    }

    if (tmp != itor) nd->key = tmp->key;
    if (tmp->color == SMP_RBT_BLACK) smp_delete_fixup(rbt, itor);

    rbt->size--;
    tmp->key = key;

    tmp->left = tmp->right = tmp->parent = NULL;

    return tmp;
}


struct smp_rbt_node_s *smp_rnode_find_remove(struct smp_rbt_s *rbt, void *key)
{
    struct smp_rbt_node_s *node = NULL;

    if (smp_unlikely(rbt == NULL || key == NULL)) return NULL;

    if ((node = smp_rnode_find(rbt, key)) == NULL) return NULL;

    return smp_rnode_remove(rbt, node);
}


SMP_STATUS smp_rnode_destory(smp_rbt_node_t *node, smp_rbt_keyfree *free_key)
{
    if (smp_unlikely(node == NULL)) return SMP_OK;

    if (smp_likely(node->key != NULL)) free_key(node->key);

    smp_free(node);

    return SMP_OK;
}


SMP_STATUS smp_rnode_delete(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd)
{
    if ((nd = smp_rnode_remove(rbt, nd)) == NULL) return SMP_OK;

    smp_rnode_destory(nd, rbt->free_key);

    return SMP_OK;
}


SMP_STATUS smp_rnode_delete_bykey(struct smp_rbt_s *rbt, void *key)
{
    struct smp_rbt_node_s *node = NULL;

    if ((node = smp_rnode_find_remove(rbt, key)) == NULL) return SMP_OK;

    smp_rnode_destory(node, rbt->free_key);

    return SMP_OK;
}


/* - - - -- - -- -- - - - -- - -  destory rbt  -- - - - -- - - - -- - - - -- - -- */


static SMP_STATUS smp_rnode_enter_queue(smp_queue_t *queue_busy,
                                        smp_queue_t *queue_idle,
                                        struct smp_rbt_node_s *rnode)
{
    smp_qnode_t *qnode_i = NULL;

    if ((qnode_i = smp_qnode_remove(queue_idle)) == NULL) {
        qnode_i = smp_qnode_create(NULL, (void *) rnode);
    }else {
        qnode_i->data = rnode;
    }

    return smp_qenter(queue_busy, qnode_i);
}


static SMP_STATUS smp_do_rbt_clean(struct smp_rbt_s *rbt, smp_queue_t *queue_busy, smp_queue_t *queue_idle)
{
    struct smp_rbt_node_s *rnode = NULL;
    smp_qnode_t *qnode = NULL;

    smp_qenter(queue_busy, smp_qnode_create(NULL, (void *) rbt->root));
    qnode = smp_qnode_remove(queue_busy);

    while (qnode != NULL) {
        rnode = (struct smp_rbt_node_s *) qnode->data;
        rbt->size--;
        if (rnode->left != &rbt->nil) {
            smp_rnode_enter_queue(queue_busy, queue_idle, rnode->left);
        }

        if (rnode->right != &rbt->nil) {
           smp_rnode_enter_queue(queue_busy, queue_idle, rnode->right);
        }

        rnode->left = rnode->right = rnode->carrier = rnode->parent = NULL;
        smp_rnode_destory(rnode, rbt->free_key);

        qnode->data = NULL;
        smp_qenter(queue_idle, qnode);

        qnode = smp_qnode_remove(queue_busy);
    }

    return SMP_OK;
}


static SMP_STATUS smp_rbt_clean(struct smp_rbt_s *rbt)
{
    smp_queue_t *queue_busy = NULL;
    smp_queue_t *queue_idle = NULL;

    if (smp_unlikely(rbt == NULL)) return SMP_OK;
    if (smp_unlikely(rbt->root == &rbt->nil)) return SMP_OK;

    if ((queue_busy = smp_queue_create((smp_qnode_free *) smp_qnode_destory)) == NULL) goto smp_rbt_bad_clean;
    if ((queue_idle = smp_queue_create((smp_qnode_free *) smp_qnode_destory)) == NULL) goto smp_rbt_bad_clean;

    if (smp_do_rbt_clean(rbt, queue_busy, queue_idle) == SMP_FAILURE) goto smp_rbt_bad_clean;
    else {
        smp_qdestory(queue_busy);
        smp_qdestory(queue_idle);
        return SMP_OK;
    }

smp_rbt_bad_clean:

    if (queue_busy != NULL) smp_qdestory(queue_busy);
    if (queue_idle != NULL) smp_qdestory(queue_idle);

    return SMP_FAILURE;
}


static SMP_STATUS smp_rbt_free(struct smp_rbt_s *rbt)
{
    if (smp_rbt_size(rbt) > 0) return SMP_FAILURE;

    smp_free(rbt);

    return SMP_OK;
}


SMP_STATUS smp_rbt_destory(struct smp_rbt_s *rbt)
{
    if (smp_unlikely(rbt == NULL)) return SMP_OK;

    /* find all node and free_key it */
    smp_rbt_clean(rbt);
    smp_rbt_free(rbt);

    return SMP_OK;
}


/*
 * 前序遍历"红黑树"
*/
static void smp_preorder(struct smp_rbt_s *rbt, struct smp_rbt_node_s *tree)
{
    if(tree != NULL && tree != &rbt->nil)
    {
        printf("%d ", *((int *) tree->key));
        smp_preorder(rbt, tree->left);
        smp_preorder(rbt, tree->right);
    }
}


void smp_rbt_preorder(struct smp_rbt_s *rbt)
{
    printf("the preorder \n");

    if (rbt != NULL)
        smp_preorder(rbt, rbt->root);

    printf("\n");
}


/*
 * 中序遍历"红黑树"
 */
static void smp_inorder(struct smp_rbt_s *rbt, struct smp_rbt_node_s *tree)
{
    if(tree != NULL && tree != &rbt->nil)
    {
        smp_inorder(rbt, tree->left);
        printf("%d ", *((int*) tree->key));
        smp_inorder(rbt, tree->right);
    }
}


void smp_rbt_inorder(struct smp_rbt_s *rbt)
{
    printf("the inorder \n");

    if (rbt)
        smp_inorder(rbt, rbt->root);

    printf("\n");
}


/*
 * 后序遍历"红黑树"
 */
static void smp_postorder(struct smp_rbt_s *rbt, struct smp_rbt_node_s *tree)
{
    if(tree != NULL && tree != &rbt->nil)
    {
        smp_postorder(rbt, tree->left);
        smp_postorder(rbt, tree->right);
        printf("%d ", *((int*) tree->key));
    }
}


void smp_rbt_postorder(struct smp_rbt_s *rbt)
{
    printf("the postorder \n");

    if (rbt)
        smp_postorder(rbt, rbt->root);

    printf("\n");
}
