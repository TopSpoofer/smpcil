#include "../smpcil.h"
#include "../smp_stl/smp_queue/smp_queue.h"


typedef struct smp_qnode_data_s {
    int id;
    char name[10];
}smp_qnode_data_t;


SMP_STATUS smp_qnode_data_free(smp_qnode_data_t *data)
{
    if (data != NULL) {
        printf("data id %d    data name  --%s--  destory\n", data->id, data->name);
        free(data);
    }

    return SMP_OK;
}


SMP_STATUS smp_qnode_data_visit(smp_queue_t *queue, smp_qnode_t *node)
{
    smp_qnode_data_t *data;
    if (queue == NULL || node == NULL || node->data == NULL) return SMP_FAILURE;

    data = node->data;

    printf("data id %d    data name  --%s--\n", data->id, data->name);

    return SMP_OK;
}


static SMP_STATUS smp_test_initqnode_data(smp_qnode_data_t *data, int i)
{
    if (data == NULL) return SMP_FAILURE;

    data->id = i;
    memset(data->name, '\0', 10);

    memcpy(data->name, "qweq", 4);

    return SMP_OK;
}


static SMP_STATUS smp_test_queue_link(smp_queue_t *queue, smp_qnode_t *node)
{
    smp_qnode_t *node1 = NULL;
    printf("current node's data : \n");
    smp_qnode_data_visit(queue, node);
    printf("current node's prev node's data : \n");
    node1 = smp_qnode_prev(queue, node);
    smp_qnode_data_visit(queue, node1);

    printf("current node's next node's data : \n");
    node1 = smp_qnode_next(queue, node);
    smp_qnode_data_visit(queue, node1);

    printf("\n");
    return SMP_OK;
}

SMP_STATUS smp_test_qnode_create(smp_queue_t *queue)
{
    int i = 0;
    smp_qnode_t *node = NULL;
    smp_qnode_data_t *data = NULL;

    if (queue == NULL) return SMP_FAILURE;

    for (i = 0; i < 3; i++) {
        if ((data = malloc(sizeof(smp_qnode_data_t))) == NULL) {
            printf("malloc node data err i == %d\n", i);
            continue;
        }

        smp_test_initqnode_data(data, i);

        if ((node = smp_qnode_create((smp_qdata_free *) smp_qnode_data_free, data)) == NULL) {
            printf("create node err i == %d\n", i);
            continue;
        }

        smp_qenter(queue, node);
    }

    printf("removed node's data : \n");
    node = smp_qnode_remove(queue);
    smp_qnode_data_visit(queue, node);
    printf("\n");
    smp_qenter(queue, node);

    node = smp_cnt_qnode(&node->link);

    printf("\nqueue's nodes's data \n");
    return smp_queue_traversal(queue, (smp_queue_visit *) smp_qnode_data_visit);
}


SMP_STATUS smp_test_qcreate()
{
    smp_queue_t *queue = NULL;

    if ((queue = smp_queue_create((smp_qnode_free *) smp_qnode_destory)) == NULL) return SMP_FAILURE;

    smp_test_qnode_create(queue);

    printf("queue link :\n");
    pthread_mutex_lock(&queue->mutex);
    smp_queue_traversal(queue, (smp_queue_visit *) smp_test_queue_link);
    pthread_mutex_unlock(&queue->mutex);
    printf("\ndestory queue \n");
    smp_qdestory(queue);

    printf("finished test\n");

    return SMP_OK;
}


void smp_test_queue_main()
{
    smp_test_qcreate();
}
