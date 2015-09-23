#include <stdio.h>
#include <smpcil/smpcil.h>
#include <smpcil/smp_linux/smp_linux.h>
#include <smpcil/smp_mpool/smp_mpool.h>
#include <smpcil/smp_linux/smp_string/smp_string.h>


int main() {
	printf("%s\n", "start");
	char *pattern = ",";
    char *text = ",,123,,,456,,789,,";
    char *p = NULL;

    if ((text = smp_calloc(strlen(",,123,,,456,,789,,"))) == NULL) {
        printf("malloc text err\n");
        return SMP_FAILURE;
    }

    memcpy(text, ",123,456,,789,,", strlen(",123,456,,789,,"));

    if ((p = smp_strtok(text, pattern)) == NULL) {
        printf("tok err\n");
        return SMP_OK;
    }
    printf("%s\n", p);

    if ((p = smp_strtok(NULL, pattern)) == NULL) {
        printf("tok err\n");
        return SMP_OK;
    }
    printf("%s\n", p);

    if ((p = smp_strtok(NULL, pattern)) == NULL) {
        printf("tok err\n");
        return SMP_OK;
    }
    printf("%s\n", p);

    return SMP_OK;
}