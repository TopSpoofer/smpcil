#include "../smpcil.h"
#include "../smp_stl/smp_list/smp_list.h"
#include "../smp_linux/smp_string/smp_string.h"
#include "smp_list_test.h"


static int dump_list_part_elems(void *elem)
{
    printf("%d\n", *((int *) elem));
    return SMP_OK;
}


static void dump_list_parts(smp_list_t *list)
{
    smp_list_traversal(list, dump_list_part_elems);
    return;
}


SMP_STATUS smp_test_list_create()
{
    smp_list_t *list = NULL;
    int i = 0;

    list = smp_list_create(sizeof(int), 5);
    if (list == NULL) {
        printf("create list err\n");
        return SMP_FAILURE;
    }

    for (i = 0; i < 15; i++) {
        int *ptr = smp_list_push(list);
        *ptr = i + 1;
    }

    dump_list_parts(list);

    smp_list_destory(list);

    printf("finished tets\n");
    return SMP_OK;
}


void smp_test_list_main()
{
    smp_test_list_create();
}
