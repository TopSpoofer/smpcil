#ifndef SMP_FILE_H
#define SMP_FILE_H
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <dirent.h>
#include <stdio.h>

/* FILE BIT FLAGS */
#define SMP_FILE_RDONLY     O_RDONLY
#define SMP_FILE_WRONLY     O_WRONLY
#define SMP_FILE_RDWD       O_RDWR
#define SMP_FILE_CREAT      O_CREAT
#define SMP_FILE_EXCL       O_EXCL
#define SMP_FILE_NOCTTY     O_NOCTTY
#define SMP_FILE_TRUNC      O_TRUNC
#define SMP_FILE_APPEND     O_APPEND
#define SMP_FILE_NONBLK     O_NONBLOCK
#define SMP_FILE_SYNC       O_SYNC


/* file lock */
#define SMP_FILE_LOCK_SH    LOCK_SH
#define SMP_FILE_LOCK_EX    LOCK_EX
#define SMP_FILE_LOCK_UN    LOCK_UN
#define SMP_FILE_LOCK_NB    LOCK_NB


/* mmap prot and flags */
#define SMP_MAP_PROT_RD     PROT_READ
#define SMP_MAP_PROT_WD     PROT_WRITE
#define SMP_MAP_PROT_EXEC   PROT_EXEC
#define SMP_MAP_PROT_NONE   PROT_NONE

#define SMP_MAP_PRIVATE     MAP_PRIVATE
#define SMP_MAP_SHARED      MAP_SHARED
#define SMP_MAP_LOCKED      MAP_LOCKED


#define SMP_FILE_READ_FAIL       (-1)
#define SMP_FILE_WRITE_FAIL      (-1)


#define SMP_FILE_NO_MODE    (-1)
#define SMP_FILE_DEF_MODE   (0666)

#define SMP_DIR_NAME_LEN    (1024)


#define SMP_SIO_RDONLY          (const char *) "r"
#define SMP_SIO_WRONLY          (const char *) "w"
#define SMP_SIO_RDWD_EXIST      (const char *) "r+"
#define SMP_SIO_RDWD_CRT        (const char *) "w+"
#define SMP_SIO_APPEND_WDONLY   (const char *) "a"
#define SMP_SIO_APPEND_RDWD     (const char *) "a+"

#define SMP_SIO_FULL_BUFFER     _IOFBF
#define SMP_SIO_LINE_BUFFER     _IOLBF
#define SMP_SIO_NO_BUFFER       _IONBF




/* create and open */
SMP_VALUE smp_open(const char *file, int flg, int mode);
SMP_STATUS smp_close(int fd);
SMP_VALUE smp_create_file(const char *file, int mode);


/* read and write */
ssize_t smp_read(int fd, void *buf, size_t size);
ssize_t smp_write(int fd, const void *buf, size_t size);
ssize_t smp_read_file(const char *file, void *buf, size_t read_size);
ssize_t smp_write_file(const char *file, const void *buf, size_t write_size);
ssize_t smp_file_write_apend(const char *file, const void *buf, size_t write_size);


/* get file type */
SMP_STATUS smp_file_stat(const char *file, struct stat *info);
SMP_BOOLEAN smp_file_isDIR(const char *file);
SMP_BOOLEAN smp_file_isREG(const char *file);
SMP_BOOLEAN smp_file_isLINK(const char *file);


/* file info */
ulong smp_file_modify_time(const char *file);
ulong smp_file_access_time(const char *file);
long smp_file_size(const char *file);
SMP_STATUS smp_file_set_modify_time(const char *file, ulong modtime_stamp);
SMP_STATUS smp_file_set_access_time(const char *file, ulong acctime_stamp);


/* Descriptors and stream */
SMP_VALUE smp_file_stream2descriptor(FILE *stream);
FILE *smp_file_descriptor2stream(int fd, const char *stream_modes);
SMP_VALUE smp_file_cp_descriptor(int fd);


/* lseek */
off_t smp_lseek(int fd, off_t offset, int whence);
off_t smp_file_seek_current(int fd);
off_t smp_file_seek_begin(int fd);
off_t smp_file_seek_end(int fd);
off_t smp_file_seek_current_offset(int fd, off_t offset);
off_t smp_file_seek_end_offset(int fd, off_t offset);
off_t smp_file_seek_begin_offset(int fd, off_t offset);
long smp_file_size_from_seek(int fd);


/* mmap */
void smp_io_sync();
SMP_STATUS smp_file_io_sync(int fd);
SMP_STATUS smp_file_io_datasync(int fd);
void *smp_memmap(void *start, size_t len, int prot, int flags, int fd, off_t offset);
SMP_STATUS smp_memmap_remove(void *start, size_t length);
void *smp_file2memmap(int fd, int prot, int flags);
SMP_STATUS smp_file2memmap_remove(void *start, int fd);
SMP_STATUS smp_msync(void *start, size_t length, int flags);
SMP_STATUS smp_memmap_sync(void *start, size_t length);
SMP_STATUS smp_memmap_async(void *start, size_t length);


/* file lock */
SMP_STATUS smp_file_lock(int fd, int opeartion);
SMP_STATUS smp_file_lock_byshare(int fd);
SMP_STATUS smp_file_lock_byexclusive(int fd);
SMP_STATUS smp_file_lock_byshare_unblock(int fd);
SMP_STATUS smp_file_lock_byexclusive_unblock(int fd);
//SMP_STATUS smp_file_lock_bynonblock(int fd);
SMP_STATUS smp_file_unlock(int fd);


/* dir */
DIR *smp_dir_open(const char *dir_path);
SMP_STATUS smp_dir_close(DIR *dir);
struct dirent *smp_dir_read(DIR *dir);
long smp_dir_current_point(DIR *dir);
void smp_dir_seek_point(DIR *dir, long point);
void smp_dir_rewind2start(DIR *dir);
SMP_STATUS smp_dir_create(const char *dir_path, mode_t mode);
SMP_STATUS smp_dir_remove(const char *dir_path);
char *smp_current_working_dir();
SMP_STATUS smp_change_working_dir(const char *dir_path);


/* stream io */
FILE *smp_fopen(const char *file, const char *mode);
SMP_STATUS smp_fclose(FILE *fp);
SMP_BOOLEAN smp_sio_is_fullbuffer(FILE *fp);
SMP_BOOLEAN smp_sio_is_linebuffer(FILE *fp);
SMP_BOOLEAN smp_sio_if_nobuffer(FILE *fp);
SMP_STATUS smp_sio_setbuffer(FILE *fp, char *buffer);
SMP_STATUS smp_sio_close_buffer(FILE *fp);
SMP_STATUS smp_sio_setvbuffer(FILE *fp, char *buffer, int modes, size_t buffer_size);
SMP_STATUS smp_sio_flush(FILE *fp);
SMP_BOOLEAN smp_sio_is_stream_err(FILE *fp);
void smp_sio_clearerr(FILE *fp);
SMP_BOOLEAN smp_sio_is_endofstream(FILE *fp);


/* read and write */
SMP_STATUS smp_sio_getline(FILE *fp, char *buf, int n);
SMP_STATUS smp_sio_read(FILE *fp, void *elems, size_t elem_size, size_t elem_count);
SMP_STATUS smp_sio_write(FILE *fp, const void *elems, size_t elem_size, size_t elem_count);


/* stream seek */
long smp_sio_seek_current(FILE *fp);
SMP_STATUS smp_sio_seek(FILE *fp, long offset, int whence);
SMP_STATUS smp_sio_seek_current_offset(FILE *fp, long offset);
SMP_STATUS smp_sio_seek2begin(FILE *fp);
SMP_STATUS smp_sio_seek2end(FILE *fp);
SMP_STATUS smp_sio_seek_begin_offset(FILE *fp, long offset);
SMP_STATUS smp_sio_seek_end_offset(FILE *fp, long offset);


/* sprintf */
SMP_STATUS smp_sio_int2string(char *buffer, int n);
SMP_STATUS smp_sio_float2string(char *buffer, float n);
SMP_STATUS smp_sio_strcat(char *buffer, const char *str1, const char *str2);


#endif // SMP_FILE_H
