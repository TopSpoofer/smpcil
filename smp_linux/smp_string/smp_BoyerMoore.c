#include "../../smpcil.h"
#include "../smp_linux.h"
#include "../../smp_mpool/smp_mpool.h"
#include "../smp_string/smp_string.h"

#define SMP_BM_MAX_CHAR 256
#define SMP_BM_SIZE 256


static void smp_bm_badchar(const char *substr, int substr_len, int BMbc[]);
static void smp_bm_suffix(const char *substr, int substr_len, int suff[]);
static void smp_bm_good_suffix(const char *substr, int substr_len, int BMgs[]);
static void __smp_BoyerMoore(const char *substr, int substr_len,
                        const char *text, int text_len, int sub_point[]);


static void smp_bm_badchar(const char *substr, int substr_len, int BMbc[])
{
    int i;

    for(i = 0; i < SMP_BM_MAX_CHAR; i++) BMbc[i] = substr_len;

    for(i = 0; i < substr_len - 1; i++) BMbc[(int) substr[i]] = substr_len - 1 - i;
}


static void smp_bm_suffix(const char *substr, int substr_len, int suff[])
{
   int persucceff = 0;
   int permiss = 0;
   int curr = 0;

   suff[substr_len - 1] = substr_len;
   permiss = substr_len - 1;

   for (curr = substr_len - 2; curr >= 0; --curr) {
      if (curr > permiss && suff[curr + substr_len - (persucceff + 1)] < curr - permiss)
         suff[curr] = suff[curr + substr_len - (persucceff + 1)];
      else {
         if (curr < permiss) permiss = curr;
         persucceff = curr;

         while (permiss >= 0 && substr[permiss] == substr[permiss + substr_len - (persucceff + 1)])
             permiss--;

         suff[curr] = persucceff - permiss;
      }
   }
}


static void smp_bm_good_suffix(const char *substr, int substr_len, int BMgs[])
{
    int i = 0;
    int j = 0;
    int suff[SMP_BM_SIZE];

    smp_bm_suffix(substr, substr_len, suff);

    for(i = 0; i < substr_len; i++) BMgs[i] = substr_len;

    for(i = substr_len - 1; i >= 0; i--)
    {
        if(suff[i] == i + 1)
        {
            for(; j < substr_len - (i + 1); j++)
            {
                if(BMgs[j] == substr_len) BMgs[j] = substr_len - (i + 1);
            }
        }
    }

    for(i = 0; i <= substr_len - 2; i++) {
        BMgs[substr_len - (suff[i] + 1)] = substr_len - (i + 1);
    }
}


static void __smp_BoyerMoore(const char *substr, int substr_len,
                        const char *text, int text_len, int sub_point[])
{
    int i = 0;
    int j = 0;
    int count = 0;
    int BMbc[SMP_BM_MAX_CHAR] = {0};
    int BMgs[SMP_BM_SIZE] = {0};

    smp_bm_badchar(substr, substr_len, BMbc);
    smp_bm_good_suffix(substr, substr_len, BMgs);

    while(j <= text_len - substr_len)
    {
        for(i = substr_len - 1; i >= 0 && substr[i] == text[i + j]; i--);
        if(i < 0)
        {
            ++count;
            sub_point[count] = j;
            j += BMgs[0];

            if (count >= sub_point[0]) return;
        }
        else
        {
            j += SMP_INT_MAX(BMbc[(int) text[i + j]] - substr_len + 1 + i, BMgs[i]);
        }
    }

    sub_point[0] = count;
}


int *smp_BoyerMoore(const char *substr, const char *text, uint limit)
{
    int *point = NULL;

    if (smp_unlikely(substr == NULL || text == NULL || limit == 0)) return NULL;

    if ((point = smp_calloc(sizeof(int) * (limit + 1))) == NULL) return NULL;
    point[0] = limit;

    __smp_BoyerMoore(substr, strlen(substr), text, strlen(text), point);

    return point;
}


SMP_BOOLEAN smp_bm_substr_exist(const char *text, const char *substr)
{
    int point[3] = {0};

    if (smp_unlikely(substr == NULL || text == NULL)) return SMP_FALSE;

    point[0] = 2;

    __smp_BoyerMoore(substr, strlen(substr), text, strlen(text), point);
    if (point[0] <= 0) return SMP_FALSE;

    return SMP_TRUE;
}


inline SMP_STATUS smp_bm_point_destory(int *point)
{
    if (point != NULL) smp_free(point);

    return SMP_OK;
}
