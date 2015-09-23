#include "../smpcil.h"
#include "../smp_stl/smp_array/smp_array.h"
#include "../smp_linux/smp_string/smp_string.h"
#include "smp_array_test.h"


SMP_STATUS smp_visit_array_elems(void *elem_)
{
    int *elem = (int *) elem_;

    printf("%d      %p\n", *elem, elem);

    return SMP_OK;
}


SMP_STATUS smp_test_array_init()
{
    smp_array_t *array = NULL;
    int i = 0;

    if ((array = smp_array_init(sizeof(int), 10)) == NULL) return SMP_FAILURE;

    for (i = 0; i < 5; i++) {
        int *elem = smp_array_push(array);
        *elem = i;
    }

    smp_array_traversal(array, smp_visit_array_elems);

    int *j = (int *) smp_array_elem_n(array, 2);
    printf("%d     %p\n", *j, j);

    printf("array size %d\n", smp_array_size(array));

    smp_array_destory(array, NULL);

    return SMP_OK;
}


void smp_test_array_main()
{
    smp_test_array_init();
    printf("finished test\n");
}
