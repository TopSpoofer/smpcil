#ifndef SMP_STRING_H
#define SMP_STRING_H

#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>


#define SMP_IS_SPACE(c)  (c == ' ' || c == '\t')
#define SMP_IS_CRLF(c)  (c == '\n' || c == '\r')


#define smp_str2int(number) atoi(number)
#define smp_str2long(number) atol(number)
#define smp_str2float(number) atof(number);

#define smp_chr2upper(chr) toupper(chr)
#define smp_chr2lower(chr) tolower(chr)


typedef struct smp_str_s {
    size_t len;
    u_char *data;
}smp_str_t;


typedef struct smp_split_s {
    uint amount;
    struct smp_str_s *strs;
}smp_split_t;


typedef struct smp_kv_s {
    struct smp_str_s key;
    struct smp_str_s value;
}smp_kv_t;




/* string */
SMP_VALUE smp_str_cmp(const struct smp_str_s *str1, const struct smp_str_s *str2);
SMP_VALUE smp_str_cmp_bychr(const struct smp_str_s *str, const u_char *chr);
struct smp_str_s *smp_string_create(void *data, size_t data_size);
struct smp_str_s *smp_str_create();
struct smp_str_s *smp_str_batch_create(uint amount);
struct smp_split_s *smp_split_create();

void *smp_str_set_data(struct smp_str_s *str, void *data, size_t data_size);
SMP_STATUS smp_str_destory_complete(struct smp_str_s *str);
SMP_STATUS smp_str_destory(struct smp_str_s *str);
SMP_STATUS smp_split_destory(struct smp_split_s *sp);

SMP_STATUS smp_str_chrprint(struct smp_str_s *str);
SMP_STATUS smp_str_hexprint(struct smp_str_s *str);
char *smp_str_trim(char * str);
char *smp_str_trimLF(char *str);
char *smp_str_trimCRLF(char *str);
char *smp_str_trim_complete(char *str);

/* substr */
SMP_VALUE smp_str_substr_exist(const char *text, const char *substr);
struct smp_split_s *smp_str_split(const char *text, const char *delim, uint limit);
char *smp_strtok(char *str, const char *delim);

/* key value */
SMP_BOOLEAN smp_kv_keyIsequal_bykeychr(const struct smp_kv_s *kv, const char *key);
SMP_BOOLEAN smp_kv_keyIsequal_bystrs(const struct smp_str_s *key1, const struct smp_str_s *key2);
SMP_BOOLEAN smp_kv_keyIsequal_bykeys(const struct smp_kv_s *kv1, const struct smp_kv_s *kv2);
struct smp_kv_s *smp_kv_create(void *pool);
SMP_STATUS smp_kv_build(struct smp_kv_s *kv, const char *key, const char *value);
struct smp_kv_s *smp_kv_init(void *pool, const char *key, const char *value);

/* BoyerMoore */
int *smp_BoyerMoore(const char *substr, const char *text, uint limit);
SMP_STATUS smp_bm_point_destory(int *point);
SMP_BOOLEAN smp_bm_substr_exist(const char *text, const char *substr);

#endif // SMP_STRING_H
