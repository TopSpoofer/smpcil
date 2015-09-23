#include "../smpcil.h"
#include "../smp_stl/smp_link/smp_link.h"
#include "../smp_linux/smp_string/smp_string.h"
#include "../smp_mpool/smp_mpool.h"
#include "../smp_linux/smp_linux.h"
#include "smp_link_test.h"


inline SMP_STATUS smp_link_data_free(void *data)
{
    if (data != NULL) {
        printf("free  %d\n", *((int *) data));
        smp_free(data);
    }

    return SMP_OK;
}


inline SMP_STATUS smp_link_data_print(void *data)
{
    if (data != NULL) printf("%d\n", *((int *) data));
    return SMP_OK;
}


inline SMP_STATUS smp_link_data_cmp_(void *link_data_, void *data_)
{
    int *link_data = link_data_;
    int *data =data_;

    if (*link_data == *data) return SMP_CMP_EQUAL;
    else return SMP_CMP_NOTEQUAL;
}

SMP_STATUS smp_test_link_init()
{
    smp_link_t *link = NULL;
    int i = 0;
    int *data = NULL;
    int *data2 = NULL;

    if ((link = smp_link_init(smp_link_data_free)) == NULL) {
        printf("maoolc link err\n");
        return SMP_FAILURE;
    }

    for (i = 0; i < 16; i++) {
        if ((data = malloc(sizeof(int))) == NULL) {
            printf("malloc data err\n");
            return SMP_FAILURE;
        }

        *data = i;

        smp_link_insert(link, data);
    }

    smp_link_traversal(link, smp_link_data_print);

    data = smp_link_node_rm_athead(link);
    printf("after rm node at head\n");
    smp_link_traversal(link, smp_link_data_print);
    printf("data -=--  %d\n", *((int *) data));

    printf("after rm node at head\n");
    data2 = smp_link_node_rm_athead(link);
    smp_link_traversal(link, smp_link_data_print);
    printf("data2 -=--  %d\n", *((int *) data2));

    printf("after insert 2 node\n");
    smp_link_insert(link, data2);
    smp_link_insert(link, data);
    smp_link_traversal(link, smp_link_data_print);

    printf("after rm node at tail\n");
    data = smp_link_node_rm_attail(link);
    smp_link_traversal(link, smp_link_data_print);
    printf("data tail ----- %d\n", *((int *) data));

    i = 10;
    printf("after rm node by find\n");
    data2 = smp_link_node_rm(link, &i, smp_link_data_cmp_);
    smp_link_traversal(link, smp_link_data_print);
    if (data2 != NULL) printf("data2   %d\n", *((int *) data2));

    smp_link_insert(link, data2);
    smp_link_insert(link, data);

    smp_link_destory(link);

    return SMP_OK;
}


void smp_test_link_main()
{
    smp_test_link_init();
    printf("finished test\n");
}
