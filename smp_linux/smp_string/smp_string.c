#include "../../smpcil.h"
#include "../../smp_mpool/smp_mpool.h"
#include "../smp_error/smp_error.h"
#include "../smp_linux.h"
#include "smp_string.h"


#define SMP_HEX_PRINTF  16
#define SMP_CHAR_PRINTF 0


SMP_VALUE smp_str_cmp(const struct smp_str_s *str1, const struct smp_str_s *str2)
{
    if (smp_unlikely(str1 == NULL || str2 == NULL)) return SMP_CMP_POINTLESS;
    if (smp_unlikely(str1->data == NULL || str2->data == NULL)) return SMP_CMP_POINTLESS;

    if (str1->len > str2->len) return SMP_CMP_MORETHAN;
    else {
        int ret = 0;
        if (str1->len < str2->len) return SMP_CMP_LESSTHAN;
        ret = memcmp(str1->data, str2->data, str1->len);
        if (ret == 0) return SMP_CMP_EQUAL;
        else if (ret > 0) return SMP_CMP_MORETHAN;
        return SMP_CMP_LESSTHAN;
    }
}


SMP_VALUE smp_str_cmp_bychr(const struct smp_str_s *str, const u_char *chr)
{
    size_t chr_len = strlen((const char *) chr);

    if (smp_unlikely(str == NULL || chr == NULL || str->data == NULL))
        return SMP_CMP_POINTLESS;

    if (str->len > chr_len) return SMP_CMP_MORETHAN;
    if (str->len < chr_len) return SMP_CMP_LESSTHAN;

    int ret = memcmp(str->data, chr, chr_len);
    if (ret == 0) return SMP_CMP_EQUAL;
    else {
        if (ret > 0) return SMP_CMP_MORETHAN;
        return SMP_CMP_LESSTHAN;
    }
}


struct smp_str_s *smp_string_create(void *data, size_t data_size)
{
    struct smp_str_s *str = NULL;

    if ((str = smp_calloc(sizeof(struct smp_str_s))) == NULL) {
        smp_set_error("malloc str err\n");
        return NULL;
    }

    str->data = data;
    str->len = data_size;

    return str;
}


struct smp_str_s *smp_str_create()
{
    struct smp_str_s *str = NULL;

    if ((str = smp_calloc(sizeof(struct smp_str_s))) == NULL) {
        smp_set_error("malloc str err\n");
        return NULL;
    }

    str->data = NULL;
    str->len = 0;

    return str;
}


struct smp_str_s *smp_str_batch_create(uint amount)
{
    struct smp_str_s *strs = NULL;
    uint i = 0;

    if (smp_unlikely(amount == 0)) return NULL;

    if ((strs = smp_calloc(sizeof(struct smp_str_s) * amount)) == NULL) {
        smp_set_error(strerror(errno));
        return NULL;
    }

    for (i = 0; i < amount; i++) {
        strs[i].data = NULL;
        strs[i].len = 0;
    }

    return strs;
}


struct smp_split_s *smp_split_create()
{
    struct smp_split_s *sp = NULL;

    if ((sp = smp_calloc(sizeof(struct smp_split_s))) == NULL) {
        smp_set_error(strerror(errno));
        return NULL;
    }

    sp->amount = 0;
    sp->strs = NULL;

    return sp;
}


void *smp_str_set_data(struct smp_str_s *str, void *data, size_t data_size)
{
    void *old_data = NULL;

    if (smp_unlikely(str == NULL)) return NULL;

    old_data = str->data;
    str->data = data;
    str->len = data_size;

    return old_data;
}


SMP_STATUS smp_str_destory_complete(struct smp_str_s *str)
{
    if (smp_unlikely(str == NULL)) return SMP_OK;

    if (smp_unlikely(str->data != NULL)) smp_free(str->data);
    str->data = NULL;
    str->len = 0;

    smp_free(str);

    return SMP_OK;
}


SMP_STATUS smp_str_destory(struct smp_str_s *str)
{
    if (smp_unlikely(str == NULL)) return SMP_OK;

    str->data = NULL;
    str->len = 0;

    smp_free(str);

    return SMP_OK;
}


SMP_STATUS smp_split_destory(struct smp_split_s *sp)
{
    if (smp_unlikely(sp == NULL)) return SMP_OK;

    sp->amount = 0;
    if (sp->strs != NULL) smp_str_destory(sp->strs);
    sp->strs = NULL;

    smp_free(sp);

    return SMP_OK;
}


static SMP_STATUS smp_str_print(struct smp_str_s *str, char print_type)
{
    size_t i = 0;
    char *type = NULL;

    switch (print_type) {
        case SMP_HEX_PRINTF: type = "%02x "; break;
        case SMP_CHAR_PRINTF: type = "%c"; break;
        default: type = "%c"; break;
    }

    for (i = 0; i < str->len; i++) printf(type, *(str->data + i));

    printf("\n");

    return SMP_OK;
}


inline SMP_STATUS smp_str_chrprint(struct smp_str_s *str)
{
    if (smp_unlikely(str == NULL || str->len == 0)) return SMP_OK;
    if (smp_unlikely(str->data == NULL)) return SMP_FAILURE;

    return smp_str_print(str, SMP_CHAR_PRINTF);
}


inline SMP_STATUS smp_str_hexprint(struct smp_str_s *str)
{
    if (smp_unlikely(str == NULL || str->len == 0)) return SMP_OK;
    if (smp_unlikely(str->data == NULL)) return SMP_FAILURE;

    return smp_str_print(str, SMP_HEX_PRINTF);
}


char *smp_str_trim(char *str)
{
    char *start = NULL;
    char *end = NULL;

    if(smp_unlikely(str == NULL)) return NULL;

    while(SMP_IS_SPACE(*str)) ++str;

    for(start = str; *str != '\0'; str++) {
        if (SMP_IS_SPACE(*str)) {
            end = str++;
            while(SMP_IS_SPACE(*str)) ++str;
            if(*str != '\0') end = NULL;
        }
    }

    if(end != NULL) *end = '\0';

    return start;
}


char *smp_str_trimLF(char *str)
{
    if (smp_unlikely(str == NULL)) return NULL;

    char *start = NULL;
    char *end = NULL;

    if(smp_unlikely(str == NULL)) return NULL;

    while(SMP_IS_CRLF(*str)) ++str; /* rm the space in start */

    for(start = str; *str != '\0'; str++) {
        if (SMP_IS_CRLF(*str)) {
            end = str++;
            while(SMP_IS_CRLF(*str)) ++str;
            if(*str != '\0') end = NULL;
        }
    }

    if(end != NULL) *end = '\0';

    return start;
}


inline char *smp_str_trimCRLF(char *str)
{
    return smp_str_trimLF(str);
}


inline char *smp_str_trim_complete(char *str)
{
    str = smp_str_trimCRLF(str);
    return smp_str_trim(str);
}


inline SMP_VALUE smp_str_substr_exist(const char *text, const char *substr)
{
    return smp_bm_substr_exist(text, substr);
}


static void __smp_str_split(const char *text, const char *delim,
                            int *points, struct smp_split_s *split)
{
    int i = 0, tmp = 0, curr_point = 0;
    int delim_len = strlen(delim);
    int text_len = strlen(text);

    if (smp_unlikely(points == NULL || split == NULL)) return;

    if (points[0] == 0) {   /* no match the delim */
        split->amount = 1;
        split->strs[0].data = (u_char *) text;
        split->strs[0].len = text_len;
    }else {
        for (i = 1; i <= points[0]; i++) {
            if (i == 1 || points[0] == 1) { /* in first substr */
                if (points[i] == 0) split->strs[curr_point].data = NULL;
                else split->strs[curr_point].data = (u_char *) text;
                split->strs[curr_point].len = points[i];
            }else { /* substr is null, be tow delim */
                if ((tmp = (points[i] - (points[i - 1] + delim_len))) <= 0){
                    split->strs[curr_point].data = NULL;
                    split->strs[curr_point].len = 0;
                }else {
                    split->strs[curr_point].data = (u_char *) text + points[i - 1] + delim_len;
                    split->strs[curr_point].len = tmp;
                }
            }

            curr_point++;
        }
        /* get the last substr */
        split->strs[curr_point].data = (u_char *) text + points[i - 1] + delim_len;
        split->strs[curr_point].len = text_len - (split->strs[curr_point].data - (u_char *) text);
        if (split->strs[curr_point].len == 0) split->strs[curr_point].data = NULL;
        split->amount = points[0] + 1;
    }
}


struct smp_split_s *smp_str_split(const char *text, const char *delim, uint limit)
{
    struct smp_split_s *split = NULL;
    int *points = NULL;

    if (smp_unlikely(text == NULL || delim == NULL || limit < 1)) return NULL;

    if ((split = smp_split_create()) == NULL) return NULL;
    if ((split->strs = smp_str_batch_create(limit)) == NULL) goto smp_bad_split;

    points = smp_BoyerMoore(delim, text, limit - 1);
    if (points == NULL) goto smp_bad_split;

    __smp_str_split(text, delim, points, split);

    if (points != NULL) smp_bm_point_destory(points);
    return split;

smp_bad_split:

    if (split != NULL) smp_split_destory(split);
    if (points != NULL) smp_bm_point_destory(points);
    return NULL;
}


char *smp_strtok(char *str, const char *delim)
{
    static char *p = NULL;
    static char *start = NULL;
    size_t i = 0;
    size_t len = 0;
    size_t del_len = 0;

    if (str != NULL) start = p = str;   /* identical str */

    start = p;
    len = strlen(p);

    if (delim == NULL) return p;
    del_len = strlen(delim);

    for (i = 0; i < len; i++) {
        if (memcmp(p, delim, del_len) == 0) {
            *p = '\0';
            p += del_len;
            return start;
        }
        p++;
    }

    return start;
}


/* ---------------------------- kv --------------------------- */


inline SMP_BOOLEAN smp_kv_keyIsequal_bykeychr(const struct smp_kv_s *kv, const char *key)
{
    if (smp_unlikely(kv == NULL || key == NULL || kv->key.data == NULL))
        return SMP_CMP_POINTLESS;

    if (strlen(key) == kv->key.len) {
        if (memcmp(kv->key.data, key, kv->key.len) == 0) return SMP_CMP_EQUAL;
    }

    return SMP_CMP_NOTEQUAL;
}


inline SMP_BOOLEAN smp_kv_keyIsequal_bystrs(const struct smp_str_s *key1, const struct smp_str_s *key2)
{
    if (smp_str_cmp(key1, key2) == SMP_CMP_EQUAL) return SMP_CMP_EQUAL;
    return SMP_CMP_NOTEQUAL;
}


inline SMP_BOOLEAN smp_kv_keyIsequal_bykeys(const struct smp_kv_s *kv1, const struct smp_kv_s *kv2)
{
    if (smp_unlikely(&kv1->key == NULL || &kv2->key == NULL)) return SMP_CMP_POINTLESS;
    return smp_kv_keyIsequal_bystrs(&kv1->key, &kv2->key);
}


struct smp_kv_s *smp_kv_create(void *pool)
{
    struct smp_kv_s *kv = NULL;

    if (smp_unlikely(pool == NULL)) return NULL;

    if ((kv = smp_mpalloc((smp_pool_t *) pool, sizeof(struct smp_kv_s))) == NULL) return NULL;

    kv->key.data = NULL;
    kv->key.len = 0;
    kv->value.data = NULL;
    kv->value.len = 0;

    return kv;
}


inline SMP_STATUS smp_kv_build(struct smp_kv_s *kv, const char *key, const char *value)
{
    if (smp_unlikely(kv == NULL || key == NULL)) return SMP_FAILURE;

    kv->key.data = (u_char *) key;
    kv->key.len = strlen(key);

    kv->value.data = (u_char *) value;
    if (value != NULL) kv->value.len = strlen(value);
    else kv->value.len = 0;

    return SMP_OK;
}


struct smp_kv_s *smp_kv_init(void *pool, const char *key, const char *value)
{
    struct smp_kv_s *kv = NULL;

    if (smp_unlikely(pool == NULL || key == NULL)) return NULL;

    if ((kv = smp_mpalloc((smp_pool_t *) pool, sizeof(struct smp_kv_s))) == NULL) return NULL;

    smp_kv_build(kv, key, value);

    return kv;
}

