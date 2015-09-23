#include "../smpcil.h"
#include "../smp_linux/smp_file/smp_file.h"
#include "./smp_file_test.h"
#include "../smp_linux/smp_error/smp_error.h"
#include <time.h>
#include<stdio.h>

SMP_STATUS smp_test_create_file(const char *file)
{
    int fd = 0;
    ssize_t ret = 0;
    char *write_data = "2abcfdbshvbhdbshb1";

    if (file == NULL) return SMP_FAILURE;

    fd = smp_create_file(file, SMP_FILE_DEF_MODE);
    if (fd == SMP_FAILURE) return SMP_FAILURE;

    ret = smp_write(fd, write_data, strlen(write_data));
    if (ret == SMP_FAILURE) {
        perror("write err\n");
        return SMP_FAILURE;
    }

    printf("write size %ld\n", ret);

    return SMP_OK;
}


SMP_STATUS smp_test_read_file(const char *file)
{
    int ret = 0;
    int size = 2048;
    char *buf = malloc(size);

    if (file == NULL || buf == NULL) return SMP_FAILURE;

    ret = smp_read_file(file, buf, size);
    if (ret == SMP_FILE_READ_FAIL) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("%s\n", buf);

    printf("read size %d\n", ret);

    return SMP_OK;
}


SMP_STATUS smp_test_write_file(const char *file)
{
    int ret = 0;
    char *buf = "xvhsdbvhsdhsbhbxhsdhfsdhfvshdbvhsdbvhsbhvbsj";

    if (file == NULL) return SMP_FAILURE;

    ret = smp_write_file(file, buf, strlen(buf));

    if (ret == SMP_FILE_WRITE_FAIL) {
        perror("write file err\n");
        return ret;
    }

    printf("write size %d\n", ret);

    return SMP_OK;
}


SMP_STATUS smp_test_apend_file(const char *file)
{
    int ret = 0;
    char *buf = "~~~xvhsdbvhsdhsbhbxhsdhfsdhfvshdbvhsdbvhsbhvbsj~";

    if (file == NULL) return SMP_FAILURE;

    ret = smp_file_write_apend(file, buf, strlen(buf));
    if (ret == SMP_FAILURE) {
        perror("write file apend err\n");
        return SMP_FAILURE;
    }

    printf("apend file %d\n", ret);

    return SMP_OK;
}


SMP_STATUS smp_test_file_size(const char *file)
{
    long file_size = 0;
    int fd = 0;

    if ((file_size = smp_file_size(file)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("file size == %ld\n", file_size);

    if ((fd = smp_open(file, SMP_FILE_RDONLY, SMP_FILE_DEF_MODE)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("seek size %ld\n", smp_file_size_from_seek(fd));

    return SMP_OK;
}


SMP_STATUS smp_test_file_modify_time(const char *file)
{
    if (smp_file_set_modify_time(file, 1233333) == SMP_FAILURE) {
        printf("set modify err\n");
        return SMP_FAILURE;
    }

    return SMP_OK;
}


SMP_STATUS smp_test_file_access_time(const char *file)
{
    if (smp_file_set_access_time(file, 123) == SMP_FAILURE) {
        printf("set modify err\n");
        return SMP_FAILURE;
    }

    printf("%ld\n", smp_file_access_time(file));

    return SMP_OK;
}


SMP_STATUS smp_test_file_stream2_dsp(const char *file)
{
    int fd = 0;
    FILE *fp = NULL;

    if ((fp = fopen(file, "r")) == NULL) {
        perror("err:\n");
        return SMP_FAILURE;
    }

    if ((fd = smp_file_stream2descriptor(fp)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("the fd is  %d\n", fd);

    return SMP_OK;
}


SMP_STATUS smp_test_file_dsp2fstream(const char *file)
{
    int fd = 0;
    FILE *fp = NULL;
    char buf[1024] = {'\0'};
    char *s = NULL;

    if ((fd = smp_open(file, SMP_FILE_RDWD, SMP_FILE_DEF_MODE)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    if ((fp = smp_file_descriptor2stream(fd, NULL)) == NULL) {
        smp_print_error();
        return SMP_FAILURE;
    }

    s = fgets(buf, 1023, fp);
    if (s == NULL) {
        perror("err:\n");
        return SMP_FAILURE;
    }

    printf("the line data %s\n", s);

    return SMP_OK;
}


SMP_STATUS smp_test_file_cp_descriptor(const char *file)
{
    int fd = 0;
    int cp_fd = 0;
    char *ptr = "helloworld";
    int w_size = 0;

    if ((fd = smp_open(file, SMP_FILE_WRONLY, 0644)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    if ((cp_fd = smp_file_cp_descriptor(fd)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    if ((w_size = smp_write(cp_fd, ptr, strlen(ptr))) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("cp_fd %d  write size %d\n", cp_fd, w_size);


    if ((w_size = smp_write(fd, ptr, strlen(ptr))) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("fd %d  write size %d\n", fd, w_size);

    return SMP_OK;
}


SMP_STATUS smp_file_test_seek(const char *file)
{
    int fd = 0;
    char *buf = NULL;

    if ((fd = smp_open(file, SMP_FILE_RDWD, SMP_FILE_DEF_MODE)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    if ((buf = malloc(1024)) == NULL) {
        perror("err:\n");
        return SMP_FAILURE;
    }

    memset(buf, '\0', 1024);

    if (smp_read(fd, buf, 10) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("buf : %s\n", buf);

    memset(buf, '\0', 1024);
    smp_file_seek_begin(fd);
    if (smp_read(fd, buf, 10) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("back begin buf : %s\n", buf);
    memset(buf, '\0', 1024);
    smp_file_seek_begin_offset(fd, 2);

    if (smp_read(fd, buf, 10) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("back begin 2 byte buf : %s\n", buf);
    memset(buf, '\0', 1024);

    smp_file_seek_end(fd);
    if (smp_read(fd, buf, 10) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("go to end buf : %s\n", buf);
    memset(buf, '\0', 1024);

    smp_file_seek_end_offset(fd, -3);
    if (smp_read(fd, buf, 10) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("go to end 3 byte buf : %s\n", buf);
    memset(buf, '\0', 1024);

    free(buf);

    return SMP_OK;
}


SMP_STATUS smp_file_test_seek_curr(const char *file)
{
    int fd = 0;
    char buf[256] = {'\0'};
    off_t len = 0;

    if ((fd = smp_open(file, SMP_FILE_RDWD, SMP_FILE_DEF_MODE)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    len = smp_file_seek_current(fd);
    printf("brfore read %ld\n", len);

    smp_read(fd, buf, 15);
    printf("buf : %s\n", buf);
    len = smp_file_seek_current(fd);
    printf("curr seek %ld\n", len);

    printf("after read %ld\n", smp_file_seek_current(fd));
    printf("file size %ld\n", smp_file_size_from_seek(fd));
    printf("file size by name %ld\n", smp_file_size(file));

    smp_file_seek_begin_offset(fd, len);
    memset(buf, '\0', 256);
    smp_read(fd, buf, 15);
    printf("buf : %s \n", buf);

    return SMP_OK;
}


SMP_STATUS smp_test_file2memmap(const char *file)
{
    int fd = 0;
    void *start = NULL;
    off_t offset = 0;
    char buf[1024];
    char *new_char = "343345678900987654321****";

    if ((fd = smp_open(file, SMP_FILE_RDWD, SMP_FILE_DEF_MODE)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    if ((offset = smp_file_seek_end_offset(fd, 10)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("%d\n", (int)smp_write(fd, "~", 1));  //must to do that before mmap

    if ((start = smp_file2memmap(fd, SMP_MAP_PROT_WD, SMP_MAP_SHARED)) == NULL) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("the file in mem is : %s\n", (char *) start);

    memcpy(start, new_char, strlen(new_char));

    if (smp_file2memmap_remove(start, fd) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    smp_close(fd);

    if ((fd = smp_open(file, SMP_FILE_RDWD, SMP_FILE_DEF_MODE)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    memset(buf, '\0', 1024);
    smp_read(fd, buf, 1024);
    printf("after mmap, the file was : %s\n", buf);

    return SMP_OK;
}


SMP_STATUS smp_test_open_dir(const char *dir_path)
{
    DIR *dir = NULL;
    struct dirent *dr = NULL;

    if ((dir = smp_dir_open(dir_path)) == NULL) {
        smp_print_error();
        return SMP_FAILURE;
    }

    while ((dr = smp_dir_read(dir)) != NULL) {
        printf("%s\n", dr->d_name);
        printf("current pint of this dir %ld\n", smp_dir_current_point(dir));
    }

    smp_dir_rewind2start(dir);
    printf("current pint of this dir after rewind %ld\n", smp_dir_current_point(dir));

    smp_dir_close(dir);

    return SMP_OK;
}


SMP_STATUS smp_test_mkdir(const char *dir_path)
{
    if ((smp_dir_create(dir_path, SMP_FILE_DEF_MODE)) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("create a dir %s\n", dir_path);

    if (smp_dir_remove(dir_path) == SMP_FAILURE) {
        smp_print_error();
        return SMP_FAILURE;
    }

    printf("remove dir %s success\n", dir_path);

    return SMP_OK;
}



void smp_test_file_main()
{
    //smp_test_create_file("/home/lele/create_file");
    //smp_test_read_file("/home/lele/create_file");
    //smp_test_write_file("/home/lele/create_file");
    //smp_test_apend_file("/home/lele/create_file");
    //smp_test_file_size("/home/lele/baidu_cloud/baidu_cloud/YLMF_GWin7Sp1_x64_Ultimate_v2015.iso");
    //smp_test_file_modify_time("/home/lele/fork");
    //smp_test_file_access_time("/home/lele/fork");
    //smp_test_file_stream2_dsp("/home/lele/fork");
    //smp_test_file_dsp2fstream("/home/lele/fotrk");
//    smp_test_file_cp_descriptor("/home/lele/fork");
    //smp_file_test_seek("/home/lele/test");
    //smp_file_test_seek_curr("/home/lele/test");
    //smp_test_file2memmap("/home/lele/test");
//    smp_test_open_dir("/home/lele");
//    smp_test_mkdir("/home/lele/tdg");
}
