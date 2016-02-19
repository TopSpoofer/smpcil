#ifndef SMP_RBT_H
#define SMP_RBT_H


typedef int (smp_rbt_keycmp) (void *, void *);
typedef int (smp_rbt_keyfree) (void *);
typedef int (smp_rbt_key_order) (void *);

typedef struct smp_rbt_node_s {
    struct smp_rbt_node_s *parent;
    struct smp_rbt_node_s *left;
    struct smp_rbt_node_s *right;
    void *key;
    void *carrier;                          /* load this rbt node' struct, ues to Combine other struct like queue */
    char color;
}smp_rbt_node_t, smp_rnode_t;


typedef struct smp_rbt_s {
    struct smp_rbt_node_s *root;
    smp_rbt_keycmp *key_cmp;
    smp_rbt_keyfree *free_key;
    struct smp_rbt_node_s nil;
    pthread_cond_t cond;                    /* process or thread block in here */
    pthread_mutex_t mutex;                  /* uesd in Multi-process or Multithreading env */
    unsigned int quotes;                    /* the amount of process or thread to quote this queue */
    unsigned long size;                     /* the rbt node's count */
}smp_rbt_t;



unsigned long smp_rbt_size(struct smp_rbt_s * rbt);
SMP_STATUS smp_rnode_insert(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd);
struct smp_rbt_node_s *smp_rnode_find(struct smp_rbt_s *rbt, void *key);
struct smp_rbt_node_s *smp_rnode_min(struct smp_rbt_s *rbt);
struct smp_rbt_node_s *smp_rnode_max(struct smp_rbt_s *rbt);
struct smp_rbt_node_s *smp_rnode_create(void *key);
struct smp_rbt_s *smp_rbt_create(smp_rbt_keycmp *key_cmp, void *free_key);
struct smp_rbt_node_s *smp_rnode_remove(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd);
struct smp_rbt_node_s *smp_rnode_find_remove(struct smp_rbt_s *rbt, void *key);
SMP_STATUS smp_rnode_destory(smp_rbt_node_t *node, smp_rbt_keyfree *free_key);
SMP_STATUS smp_rnode_delete(struct smp_rbt_s *rbt, struct smp_rbt_node_s *nd);
SMP_STATUS smp_rnode_delete_bykey(struct smp_rbt_s *rbt, void *key);
SMP_STATUS smp_rbt_destory(struct smp_rbt_s *rbt);


void smp_rbt_preorder(struct smp_rbt_s *rbt, smp_rbt_key_order *print);
void smp_rbt_inorder(struct smp_rbt_s *rbt, smp_rbt_key_order *print);
void smp_rbt_postorder(struct smp_rbt_s *rbt, smp_rbt_key_order *print);

#endif // SMP_RBT_H
