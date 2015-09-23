#include "../smpcil.h"
#include "../smp_stl/smp_tree/smp_rbt.h"


int c = 0;

SMP_STATUS smp_test_rbt_cmp(void *data1_, void *data2_)
{
    int *data1 = data1_;
    int *data2 = data2_;

    if (*data1 > *data2) return 1;
    else {
        if (*data1 < *data2) return -1;
        else return 0;
    }
}


SMP_STATUS smp_test_rdata_free(void *data)
{
    if (data != NULL) {
        printf("free data  %d\n", *((int *) data));
        free(data);
        c++;
    }
    return SMP_OK;
}


SMP_STATUS smp_test_rbt_order(smp_rbt_t *rbt)
{
    smp_rnode_t *node = NULL;

    printf("the rbt size : %ld\n", rbt->size);
    printf("\n------- test order -----\n");
    smp_rbt_preorder(rbt);
    smp_rbt_inorder(rbt);;
    smp_rbt_postorder(rbt);

    printf("\n------- test min and max -----\n");
    node = smp_rnode_min(rbt);
    printf("the min node data in this rbt %d\n", *((int *) node->key));

    node = smp_rnode_max(rbt);
    printf("the max node data in this rbt %d\n", *((int *) node->key));

    return SMP_OK;
}


SMP_STATUS smp_test_rbt_find_remove(smp_rbt_t *rbt)
{
    smp_rnode_t *node = NULL;
    smp_rnode_t *node1 = NULL;
    int a[] = { 10, 40, 30, 60, 90, 70, 20, 50, 80 };
    int i = 70;

    printf("\n------- test find and remove -----\n");

    if (rbt == NULL) return SMP_FAILURE;

    for (i = 0; i < 9; i++) {
        node = smp_rnode_find(rbt, &a[i]);
        if (node == NULL) printf("can find %d\n", a[i]);
        else printf("the found node's key is %d\n", *((int *) node->key));
    }

    node = smp_rnode_find_remove(rbt, &a[3]);
    if (node != NULL) printf("the find and remove node key is : %d\n", *((int *) node->key));
    else printf("can find and remove this node\n");

    printf("after remove , rbt size is %ld\n", rbt->size);
    smp_rnode_destory(node, rbt->free_key);

    printf("\n----- after remove node ---- \n");
    for (i = 0; i < 9; i++) {
        node1 = smp_rnode_find(rbt, &a[i]);
        if (node1 == NULL) printf("can find %d\n", a[i]);
        else printf("the found node's key is %d\n", *((int *) node1->key));
    }

    smp_rnode_delete(rbt, node1);
    printf("after delete node , rbt size is %ld\n", rbt->size);
    printf("\n----- after delete node ---- \n");
    for (i = 0; i < 9; i++) {
        node1 = smp_rnode_find(rbt, &a[i]);
        if (node1 == NULL) printf("can find %d\n", a[i]);
        else printf("the found node's key is %d\n", *((int *) node1->key));
    }

    smp_rnode_delete_bykey(rbt, &a[7]);
    printf("after delete node , rbt size is %ld\n", rbt->size);
    printf("\n----- after delete node ---- \n");
    for (i = 0; i < 9; i++) {
        node1 = smp_rnode_find(rbt, &a[i]);
        if (node1 == NULL) printf("can find %d\n", a[i]);
        else printf("the found node's key is %d\n", *((int *) node1->key));
    }

    return SMP_OK;
}


SMP_STATUS smp_test_rbt_create()
{
    int i = 0;
    int *data = NULL;
    smp_rbt_t *rbt = NULL;
    smp_rnode_t *node = NULL;

    int ilen = 16;

    if ((rbt = smp_rbt_create(smp_test_rbt_cmp, smp_test_rdata_free)) == NULL) {
        printf("create rbt err\n");
        return SMP_FAILURE;
    }

    for (i = 0; i < ilen; i++) {
        if ((data = malloc(sizeof(int))) == NULL) {
            printf("malloc data err\n");
            return SMP_FAILURE;
        }

        *data = i;

        if ((node = smp_rnode_create(data)) == NULL) {
            printf("malloc node err\n");
            return SMP_FAILURE;
        }

        smp_rnode_insert(rbt, node);
    }

    smp_rbt_destory(rbt);
    printf("free node count %d\n", c);

//    smp_test_rbt_order(rbt);
//    smp_test_rbt_find_remove(rbt);

    return SMP_OK;
}


void smp_test_rbt_main()
{
    smp_test_rbt_create();
    printf("finished test\n");
}
