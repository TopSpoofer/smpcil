#include "../../smpcil.h"
#include "smp_file.h"
#include "../smp_error/smp_error.h"


SMP_VALUE smp_open(const char *file, int flg, int mode)
{
    int fd = 0;

    if (mode == SMP_FILE_NO_MODE) {
        if ((fd = open(file, flg)) == -1) {
            smp_set_error(strerror(errno));
            return SMP_FAILURE;
        }
    }else {
        if ((fd = open(file, flg, mode)) == -1) {
            smp_set_error(strerror(errno));
            return SMP_FAILURE;
        }
    }

    return fd;
}


inline SMP_STATUS smp_close(int fd)
{
    return close(fd);
}


inline SMP_VALUE smp_create_file(const char *file, int mode)
{
    return smp_open(file, SMP_FILE_CREAT | SMP_FILE_RDWD | SMP_FILE_TRUNC, mode);
}


ssize_t smp_read(int fd, void *buf, size_t size)
{
    ssize_t read_size = 0;

    if (smp_unlikely(buf == NULL || size <= 0)) {
        smp_set_error("no source");
        return SMP_FILE_READ_FAIL;
    }

    read_size = read(fd, buf, size);
    if (read_size >= 0) return read_size;

    return SMP_FAILURE;
}


ssize_t smp_write(int fd, const void *buf, size_t size)
{
    ssize_t write_size = 0;

    if (smp_unlikely(buf == NULL || size <= 0)) {
        smp_set_error("no source");
        return SMP_FILE_WRITE_FAIL;
    }

    write_size = write(fd, buf, size);
    if (write_size >= 0) return write_size;

    return SMP_FAILURE;
}


ssize_t smp_read_file(const char *file, void *buf, size_t read_size)
{
    ssize_t read_ret = 0;
    int fd = 0;

    if (smp_unlikely(buf == NULL || read_size == 0)) {
        smp_set_error("no source");
        return SMP_FILE_READ_FAIL;
    }

    if ((fd = smp_open(file, SMP_FILE_RDONLY, SMP_FILE_NO_MODE)) == SMP_FAILURE)
        return SMP_FILE_READ_FAIL;

    read_ret = smp_read(fd, buf, read_size);
    smp_close(fd);

    return read_ret;
}


ssize_t smp_write_file(const char *file, const void *buf, size_t write_size)
{
    ssize_t write_ret = 0;
    int fd = 0;

    if (smp_unlikely(buf == NULL || write_size <= 0)) {
        smp_set_error("no source");
        return SMP_FILE_WRITE_FAIL;
    }

    if ((fd = smp_open(file, SMP_FILE_WRONLY | SMP_FILE_CREAT, SMP_FILE_DEF_MODE)) == SMP_FAILURE)
        return SMP_FILE_WRITE_FAIL;

    write_ret = smp_write(fd, buf, write_size);
    smp_close(fd);

    return write_ret;
}


ssize_t smp_file_write_apend(const char *file, const void *buf, size_t write_size)
{
    ssize_t write_ret = 0;
    int fd = 0;

    if (smp_unlikely(buf == NULL || write_size <= 0)) {
        smp_set_error("no source");
        return SMP_FILE_WRITE_FAIL;
    }

    fd = smp_open(file, SMP_FILE_WRONLY | SMP_FILE_APPEND | SMP_FILE_CREAT, SMP_FILE_DEF_MODE);
    if (fd == SMP_FAILURE) return SMP_FILE_WRITE_FAIL;

    write_ret = smp_write(fd, buf, write_size);
    smp_close(fd);

    return write_ret;
}


SMP_STATUS smp_file_stat(const char *file, struct stat *info)
{
    if (info == NULL) {
        smp_set_error("no source");
        return SMP_FAILURE;
    }

    if (stat(file, info) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_BOOLEAN smp_file_isDIR(const char *file)
{
    struct stat info;

    if (smp_file_stat(file, &info) == SMP_FAILURE) return SMP_FALSE;

    if (S_ISDIR(info.st_mode)) return SMP_TRUE;

    return SMP_FALSE;
}


inline SMP_BOOLEAN smp_file_isREG(const char *file)
{
    struct stat file_info;

    if (smp_file_stat(file, &file_info) == SMP_FAILURE) return SMP_FALSE;

    if (S_ISREG(file_info.st_mode)) return SMP_TRUE;

    return SMP_FALSE;
}


inline SMP_BOOLEAN smp_file_isLINK(const char *file)
{
    struct stat file_info;

    if (smp_file_stat(file, &file_info) == SMP_FAILURE) return SMP_FALSE;

    if (S_ISLNK(file_info.st_mode)) return SMP_TRUE;

    return SMP_FALSE;
}


inline ulong smp_file_modify_time(const char *file)
{
    struct stat file_info;

    if (smp_file_stat(file, &file_info) == SMP_FAILURE) return 0;

    return file_info.st_mtime;
}


inline ulong smp_file_access_time(const char *file)
{
    struct stat file_info;

    if (smp_file_stat(file, &file_info) == SMP_FAILURE) return 0;

    return file_info.st_atime;
}


inline long smp_file_size(const char *file)
{
    struct stat info;

    if (smp_file_stat(file, &info) == SMP_FAILURE) return SMP_FAILURE;

    if (!S_ISREG(info.st_mode)) return 0;

    return info.st_size;
}


inline SMP_STATUS smp_file_set_modify_time(const char *file, ulong modtime_stamp)
{
    struct utimbuf time_buf;
    time_buf.modtime = modtime_stamp;
    time_buf.actime = modtime_stamp;

    if (utime(file, &time_buf) != -1) return SMP_OK;

    return SMP_FAILURE;
}


SMP_STATUS smp_file_set_access_time(const char *file, ulong acctime_stamp)
{
    struct utimbuf time_buf;

    if (smp_unlikely(file == NULL)) return SMP_FAILURE;

    time_buf.actime = acctime_stamp;
    time_buf.modtime = smp_file_modify_time(file);

    if (utime(file, &time_buf) != -1) return SMP_OK;

    return SMP_FAILURE;
}


SMP_VALUE smp_file_stream2descriptor(FILE *stream)
{
    int fd = 0;

    if (smp_unlikely(stream == NULL)) {
        smp_set_error("no source");
        return SMP_FAILURE;
    }

    if ((fd = fileno(stream)) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return fd;
}


inline FILE *smp_file_descriptor2stream(int fd, const char *stream_modes)
{
    FILE *fp = NULL;

    if (stream_modes == NULL) stream_modes = SMP_SIO_RDWD_EXIST;

    if ((fp = fdopen(fd, stream_modes)) == NULL) {
        smp_set_error(strerror(errno));
        return NULL;
    }

    return fp;
}


inline SMP_VALUE smp_file_cp_descriptor(int fd)
{
    int cp_fd = 0;

    if ((cp_fd = fcntl(fd, F_DUPFD)) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return cp_fd;
}


off_t smp_lseek(int fd, off_t offset, int whence)
{
    off_t offset_ret = 0;

    if ((offset_ret = lseek(fd, offset, whence)) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return offset_ret;
}


inline off_t smp_file_seek_current(int fd)
{
    return smp_lseek(fd, 0, SEEK_CUR);
}


inline off_t smp_file_seek_begin(int fd)
{
    return smp_lseek(fd, 0, SEEK_SET);
}


inline off_t smp_file_seek_end(int fd)
{
    return smp_lseek(fd, 0, SEEK_END);
}


inline off_t smp_file_seek_current_offset(int fd, off_t offset)
{
    return smp_lseek(fd, offset, SEEK_CUR);
}


inline off_t smp_file_seek_end_offset(int fd, off_t offset)
{
    return smp_lseek(fd, offset, SEEK_END);
}


inline off_t smp_file_seek_begin_offset(int fd, off_t offset)
{
    return smp_lseek(fd, offset, SEEK_SET);
}


long smp_file_size_from_seek(int fd)
{
    long file_size = 0;
    off_t curr_offset = 0;

    if ((curr_offset = smp_file_seek_current(fd)) == SMP_FAILURE) return SMP_FAILURE;

    if ((file_size = smp_file_seek_end(fd)) == SMP_FAILURE) return SMP_FAILURE;

    if (smp_file_seek_begin_offset(fd, curr_offset) == SMP_FAILURE) return SMP_FAILURE;

    return file_size;
}


inline void smp_io_sync()
{
    sync();
}


inline SMP_STATUS smp_file_io_sync(int fd)
{
    if (fsync(fd) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_STATUS smp_file_io_datasync(int fd)
{
    if (fdatasync(fd) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline void *smp_memmap(void *start, size_t len, int prot, int flags, int fd, off_t offset)
{
    void *address = NULL;

    if ((address = mmap(start, len, prot, flags, fd, offset)) == MAP_FAILED) {
        smp_set_error(strerror(errno));
        return NULL;
    }

    return address;
}


inline SMP_STATUS smp_memmap_remove(void *start, size_t length)
{
    if (munmap(start, length) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline void *smp_file2memmap(int fd, int prot, int flags)
{
    long file_size = 0;

    if ((file_size = smp_file_size_from_seek(fd)) == SMP_FAILURE) return NULL;

    return smp_memmap(NULL, file_size, prot, flags, fd, 0);
}


inline SMP_STATUS smp_file2memmap_remove(void *start, int fd)
{
    int file_size = 0;

    if ((file_size = smp_file_size_from_seek(fd)) == SMP_FAILURE) return SMP_FAILURE;

    return smp_memmap_remove(start, file_size);
}


inline SMP_STATUS smp_msync(void *start, size_t length, int flags)
{
    if (msync(start, length, flags) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_STATUS smp_memmap_sync(void *start, size_t length)
{
    return smp_msync(start, length, MS_SYNC);
}


inline SMP_STATUS smp_memmap_async(void *start, size_t length)
{
    return smp_msync(start, length, MS_ASYNC);
}


inline SMP_STATUS smp_file_lock(int fd, int opeartion)
{
    if (flock(fd, opeartion) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_STATUS smp_file_lock_byshare(int fd)
{
    return smp_file_lock(fd, SMP_FILE_LOCK_SH);
}


inline SMP_STATUS smp_file_lock_byexclusive(int fd)
{
    return smp_file_lock(fd, SMP_FILE_LOCK_EX);
}


//inline SMP_STATUS smp_file_lock_bynonblock(int fd)
//{
//    return smp_file_lock(fd, SMP_FILE_LOCK_NB);
//}


inline SMP_STATUS smp_file_lock_byshare_unblock(int fd)
{
    return smp_file_lock(fd, SMP_FILE_LOCK_SH | SMP_FILE_LOCK_NB);
}


inline SMP_STATUS smp_file_lock_byexclusive_unblock(int fd)
{
    return smp_file_lock(fd, SMP_FILE_LOCK_EX | SMP_FILE_LOCK_NB);
}


inline SMP_STATUS smp_file_unlock(int fd)
{
    return smp_file_lock(fd, SMP_FILE_LOCK_UN);
}


inline DIR *smp_dir_open(const char *dir_path)
{
    DIR *dir = NULL;

    if ((dir = opendir(dir_path)) == NULL) {
        smp_set_error(strerror(errno));
        return NULL;
    }

    return dir;
}


inline SMP_STATUS smp_dir_close(DIR *dir)
{
    if (closedir(dir) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline struct dirent *smp_dir_read(DIR *dir)
{
    return readdir(dir);
}


inline long smp_dir_current_point(DIR *dir)
{
    long point = 0;

    if ((point = telldir(dir)) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return point;
}


inline void smp_dir_seek_point(DIR *dir, long point)
{
    seekdir(dir, point);
}


inline void smp_dir_rewind2start(DIR *dir)
{
    rewinddir(dir);
}


inline SMP_STATUS smp_dir_create(const char *dir_path, mode_t mode)
{
    if (mkdir(dir_path, mode) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_STATUS smp_dir_remove(const char *dir_path)
{
    if (rmdir(dir_path) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


char *smp_current_working_dir()
{
    static char *dir = NULL;
    char *ret = NULL;

    if (dir == NULL)
        if ((dir = malloc(SMP_DIR_NAME_LEN)) == NULL) return NULL;

    memset(dir, '\0', SMP_DIR_NAME_LEN);

    if ((ret = getcwd(dir, SMP_DIR_NAME_LEN)) == NULL) {
        smp_set_error(strerror(errno));
        return NULL;
    }

    return dir;
}


inline SMP_STATUS smp_change_working_dir(const char *dir_path)
{
    if (chdir(dir_path) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}
