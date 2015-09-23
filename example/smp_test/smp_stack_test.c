#include "../smpcil.h"
#include "../smp_mpool/smp_mpool.h"
#include "../smp_linux/smp_string/smp_string.h"
#include "../smp_stl/smp_stack/smp_stack.h"
#include "smp_stack_test.h"



inline static SMP_STATUS smp_stack_elem_apption(void *stk_elem, void *data)
{
    *((int *)stk_elem) = *((int *)data);
    return SMP_OK;
}



SMP_STATUS smp_test_stack_init()
{
    smp_stk_t *stack = NULL;
    smp_stk_node_t *node = NULL;
    int i = 0;
    int len = 0;
    int *data = NULL;

    stack = smp_stack_init(sizeof(int), SMP_STACK_UNLIMITED, smp_stack_elem_apption);

    if (stack == NULL) {
        printf("init stack err\n");
        return SMP_FAILURE;
    }

    for (i = 0; i < 10; i++) {
        data = smp_stack_push(stack, &i);
    }
    printf("stack size %d\n", smp_stack_size(stack));
    len = smp_stack_length(stack);
    printf("stack length %d\n", len);

    for (i = 0; i < 8; i++) {
        node = smp_stack_pop(stack);
        printf("%d\n", *((int *) node->data));
        smp_stack_recover(stack, node);
    }

    for (i = 16; i < 24; i++) {
        data = smp_stack_push(stack, &i);
    }

    len = smp_stack_length(stack);

    for (i = 0; i < len; i++) {
        node = smp_stack_pop(stack);
        printf("%d\n", *((int *)node->data));
        smp_stack_recover(stack, node);
    }
    printf("len --- %d\n", len);
    smp_stack_destory(stack);

    return SMP_OK;
}


void smp_test_stack_main()
{
    smp_test_stack_init();

    printf("finished test\n");
}
