#include "../smpcil.h"
#include "../smp_linux/smp_file/smp_file.h"
#include "../smp_linux/smp_error/smp_error.h"


struct people_s {
    char name[15];
    int age;
    char sex;
};


static void smp_test_get_buffer_type(FILE *fp)
{
    if (smp_sio_if_nobuffer(fp) == SMP_TRUE) {
        printf("is no buffer\n");
    }else {
        if (smp_sio_is_fullbuffer(fp) == SMP_TRUE) {
            printf("is full buffer\n");
        }else {
            if (smp_sio_is_linebuffer(fp) == SMP_TRUE) {
                printf("is line buffer\n");
            }else printf("error buffer type\n");
        }
    }
}


static SMP_STATUS smp_test_sio_open(const char *file)
{
    FILE *fp = NULL;
    char *buf = malloc(BUFSIZ);

    if (buf == NULL || file == NULL) return SMP_FAILURE;

    if ((fp = smp_fopen(file, SMP_SIO_RDWD_CRT)) == NULL) {
        smp_print_error();
        return SMP_FAILURE;
    }

    smp_test_get_buffer_type(fp);

    if (smp_sio_setvbuffer(fp, buf, SMP_SIO_LINE_BUFFER, BUFSIZ) == SMP_FAILURE) {
        smp_print_error();
        free(buf);
        return SMP_FAILURE;
    }

    smp_test_get_buffer_type(fp);

    smp_fclose(fp);
    free(buf);
    printf("finished test\n");
    return SMP_OK;
}


static SMP_STATUS smp_test_setbuffer(const char *file)
{
    FILE *fp = NULL;
    char *buf = malloc(BUFSIZ);

    if (buf == NULL || file == NULL) return SMP_FAILURE;

    if ((fp = smp_fopen(file, SMP_SIO_RDWD_CRT)) == NULL) {
        smp_print_error();
        free(buf);
        return SMP_FAILURE;
    }

    smp_test_get_buffer_type(fp);

    if (smp_sio_setbuffer(fp, buf) == SMP_FAILURE) {
        smp_print_error();
        free(buf);
        return SMP_FAILURE;
    }

    smp_test_get_buffer_type(fp);

    free(buf);
    smp_fclose(fp);
    printf("finished tets\n");
    return SMP_OK;
}


static void smp_init_people(struct people_s *peoples)
{
    if (peoples == NULL) return;

    memset(peoples, '\0', sizeof(struct people_s) * 3);

    peoples[0].age = 1;
    peoples[0].sex = 'w';
    memcpy(&peoples[0], "lele", 4);

    peoples[1].age = 10;
    peoples[1].sex = 'm';
    memcpy(&peoples[1], "fork", 4);

    peoples[2].age = 15;
    peoples[2].sex = 'm';
    memcpy(&peoples[2], "dog", 3);
}


static SMP_STATUS smp_test_sio_write(const char *file)
{
    FILE *fp = NULL;

    struct people_s *peoples = malloc(sizeof(struct people_s) * 3);

    if (peoples == NULL || file == NULL) return SMP_FAILURE;

    if ((fp = smp_fopen(file, SMP_SIO_RDWD_CRT)) == NULL) {
        smp_print_error();
        return SMP_FAILURE;
    }

    smp_init_people(peoples);

    if (smp_sio_write(fp, (char *) peoples, sizeof(struct people_s), 3) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    smp_fclose(fp);
    free(peoples);
    printf("finished write test\n");

    return SMP_OK;
}


static SMP_STATUS smp_test_sio_read(const char *file)
{
    FILE *fp = NULL;
    int i = 0;

    struct people_s *peoples = malloc(sizeof(struct people_s) * 3);

    if (peoples == NULL || file == NULL) return SMP_FAILURE;

    if ((fp = smp_fopen(file, SMP_SIO_RDONLY)) == NULL) {
        smp_print_error();
        return SMP_FAILURE;
    }

    memset(peoples, '\0', sizeof(struct people_s) * 3);

    if (smp_sio_read(fp, peoples, sizeof(struct people_s), 3) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    for (i = 0; i < 3; i++) {
        printf("name %s,  age %d,  sex %c\n", peoples[i].name, peoples[i].age, peoples[i].sex);
    }

    smp_fclose(fp);
    free(peoples);
    printf("finished read test\n");

    return SMP_OK;
}


static void  smp_test_seeked_print(FILE *fp, const char *info)
{
    int i = 0;
    struct people_s *peoples = malloc(sizeof(struct people_s) * 3);
    if (peoples == NULL) return;

    memset(peoples, '\0', sizeof(struct people_s) * 3);

    if (smp_sio_read(fp, peoples, sizeof(struct people_s), 1) == SMP_FAILURE) {
        smp_print_error();
        return;
    }

    for (i = 0; i < 1; i++) {
        printf("%s  : name %s,  age %d,  sex %c\n", info, peoples[i].name, peoples[i].age, peoples[i].sex);
    }

    free(peoples);
}


SMP_STATUS smp_test_sio_seek(const char *file)
{
    FILE *fp = NULL;

    if ((fp = smp_fopen(file, SMP_SIO_RDONLY)) == NULL) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("current point %ld\n", smp_sio_seek_current(fp));
    printf("sizeof people_s %d\n", (int) sizeof(struct people_s));
    smp_test_seeked_print(fp, "first read");
    printf("after read current point %ld\n", smp_sio_seek_current(fp));

    smp_sio_seek_current_offset(fp, sizeof(struct people_s));
    smp_test_seeked_print(fp, "one people_s size of current point");

    smp_sio_seek_begin_offset(fp, 2 * sizeof(struct people_s));
    smp_test_seeked_print(fp, "seek to begin of 2 people_s size");

    smp_fclose(fp);
    printf("finished seek test\n");
    return SMP_OK;
}


SMP_STATUS smp_test_sio_sprinf()
{
    int i = 1314;
    float f = 1313.13124;
    char buf[128] = {'\0'};

    smp_sio_int2string(buf, i);
    printf("%s\n", buf);

    memset(buf, '\0', 128);
    smp_sio_float2string(buf, f);
    printf("%s\n", buf);

    memset(buf, '\0', 128);
    smp_sio_strcat(buf, "gggdfv", "sjsbv");
    printf("%s\n", buf);

    return SMP_OK;
}


void smp_test_sio_main()
{
    //smp_test_sio_open("/home/lele/test_buffer");
    //smp_test_setbuffer("/home/lele/test_buffer");
    //smp_test_sio_write("/home/lele/test_buffer");
    //smp_test_sio_read("/home/lele/test_buffer");
    //smp_test_sio_seek("/home/lele/test_buffer");

    smp_test_sio_sprinf();
}
