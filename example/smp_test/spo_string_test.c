#include "../smpcil.h"
#include "../smp_linux/smp_file/smp_file.h"
#include "../smp_mpool/smp_mpool.h"
#include "../smp_linux/smp_error/smp_error.h"
#include "../smp_linux/smp_linux.h"
#include "../smp_linux/smp_string/smp_string.h"


SMP_STATUS smp_test_str_create()
{
    smp_str_t *str = NULL;
    char *data = "    \t 112345678  \t \r\n\n";
    char *ch = NULL;
    char *hh = malloc(strlen(data) + 1);
    memset(hh, '\0', strlen(data));
    memcpy(hh, data, strlen(data));

    if ((str = smp_string_create(data, strlen(data))) == NULL) {
        printf("create str err\n");
        return SMP_FAILURE;
    }

//    printf("str char print : ");
//    smp_str_chrprint(str);
//    printf("\n");
//    printf("str hex print : ");
//    smp_str_hexprint(str);

//    ch = smp_str_trimLF(hh);
    ch = smp_str_trim_complete(hh);
    printf("--%s--\n", ch);

    smp_str_destory(str);

    return SMP_OK;
}

SMP_STATUS smp_test_str_spilt()
{
    char *pattern = ",";
    char *text = ",,123,,,456,,789,,";
    smp_split_t *split = NULL;
    uint i = 0;

    split = smp_str_split(text, pattern, 10);
    if (split == NULL) {
        printf("split str err\n");
        return SMP_FAILURE;
    }

    for (i = 0; i < split->amount; i++) {
        if (split->strs[i].data == NULL) {
            printf("the split i = %d, is NULL\n\n", i);
        }else {
            printf("split a substr and it's len is %d\n", (int) split->strs[i].len);
            smp_str_chrprint(&split->strs[i]);
            printf("\n");
        }
    }

    smp_split_destory(split);

    return SMP_OK;
}


SMP_STATUS smp_test_str_strtok()
{
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


SMP_STATUS smp_test_str_exist()
{
    char *substr = "asdf45d231f";
    char *text = "12fg1323asdf45d231fd213gd321dcv312fg6sdfgdggdfd9";

    if (smp_str_substr_exist(text, substr) == SMP_TRUE)
        printf("have a sub %s\n", substr);
    else printf("havn't a sub %s\n", substr);

    return SMP_OK;
}


SMP_STATUS smp_test_str2number()
{
    int num = smp_str2int("-12q3a");
    printf("%d\n", num);

    long num1 = smp_str2long("123444");
    printf("num1 %ld\n", num1);

    float num3 = smp_str2float("123.34");
    printf("num3 %0.2f\n", num3);

    char c = 'A';
    c = smp_chr2upper(c);
    printf("chr2upper  %c\n", c);

    c = smp_chr2lower(c);
    printf("chr2lower  %c\n", c);

    return SMP_OK;
}


SMP_STATUS smp_test_str_cmp()
{
    struct smp_str_s str1;
    struct smp_str_s str2;

    str1.data = (u_char *) "624";
    str1.len = strlen("623");

    str2.data = (u_char *) "5678";
    str2.len = strlen("5678");

    int ret = smp_str_cmp(&str1, &str2);
    if (ret == SMP_CMP_EQUAL) printf("equal\n");
    if (ret == SMP_CMP_LESSTHAN) printf("less than\n");
    if (ret == SMP_CMP_MORETHAN) printf("more than\n");
    if (ret == SMP_CMP_POINTLESS) printf("pointless\n");

    return SMP_OK;
}

SMP_STATUS smp_test_kv_create()
{
    smp_kv_t *kv1 = NULL;
    smp_kv_t *kv2 = NULL;
    smp_pool_t *pool = NULL;

    if ((pool = smp_mpool_create(1024)) == NULL) return SMP_FAILURE;

    if ((kv1 = smp_kv_init(pool, "lele", "1234")) == NULL) return SMP_FAILURE;
    if ((kv2 = smp_kv_init(pool, "fork", "bitch")) == NULL) return SMP_FAILURE;

    printf("key1 --%s--  value1 --%s-- \n", kv1->key.data, kv1->value.data);
    printf("key2 --%s--  value2 --%s-- \n", kv2->key.data, kv2->value.data);

//    int ret = smp_kv_keyIsequal_bykeys(kv1, kv2);
    int ret = smp_kv_keyIsequal_bykeychr(kv1, "lele");
    if (ret == SMP_CMP_EQUAL) printf("equal\n");
    if (ret == SMP_CMP_LESSTHAN) printf("less than\n");
    if (ret == SMP_CMP_MORETHAN) printf("more than\n");
    if (ret == SMP_CMP_POINTLESS) printf("pointless\n");
    if (ret == SMP_CMP_NOTEQUAL) printf("not equal\n");

    return SMP_OK;
}


void smp_test_str_main()
{
//    smp_test_str_create();
//    smp_test_str_spilt();
//    smp_test_str_strtok();
//    smp_test_str_exist();
//    smp_test_str2number();
//    smp_test_str_cmp();
    smp_test_kv_create();
    printf("finished test\n");
}


