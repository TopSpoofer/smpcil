#include "../../smpcil.h"
#include "smp_file.h"
#include "../smp_error/smp_error.h"
#include <stdio.h>


inline FILE *smp_fopen(const char *file, const char *mode)
{
    FILE *fp = NULL;

    if ((fp = fopen(file, mode)) == NULL) {
        smp_set_error(strerror(errno));
        return NULL;
    }

    return fp;
}


inline SMP_STATUS smp_fclose(FILE *fp)
{
    if (fclose(fp) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_BOOLEAN smp_sio_is_fullbuffer(FILE *fp)
{
    if (smp_unlikely(fp == NULL)) return SMP_FALSE;
    if (fp->_flags & _IOFBF) return SMP_TRUE;

    return SMP_FALSE;
}


inline SMP_BOOLEAN smp_sio_is_linebuffer(FILE *fp)
{
    if (smp_unlikely(fp == NULL)) return SMP_FALSE;
    if (fp->_flags & _IOLBF) return SMP_TRUE;

    return SMP_FALSE;
}


inline SMP_BOOLEAN smp_sio_if_nobuffer(FILE *fp)
{
    if (smp_unlikely(fp == NULL)) return SMP_FALSE;
    if (fp->_flags & _IONBF) return SMP_TRUE;

    return SMP_FALSE;
}


inline SMP_STATUS smp_sio_setbuffer(FILE *fp, char *buffer)
{
    if (smp_unlikely(fp == NULL || buffer == NULL)) {
        smp_set_error("Resource unavailable");
        return SMP_FAILURE;
    }

    setbuf(fp, buffer);

    return SMP_OK;
}


inline SMP_STATUS smp_sio_close_buffer(FILE *fp)
{
    if (smp_unlikely(fp == NULL)) {
        smp_set_error("Resource unavailable");
        return SMP_FAILURE;
    }

    setbuf(fp, NULL);

    return SMP_OK;
}


SMP_STATUS smp_sio_setvbuffer(FILE *fp, char *buffer, int modes, size_t buffer_size)
{
    if (smp_unlikely(fp == NULL || buffer_size < 128)) {
        smp_set_error("Resource unavailable");
        return SMP_FAILURE;
    }

    if (setvbuf(fp, buffer, modes, buffer_size) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_STATUS smp_sio_flush(FILE *fp)
{
    if (fflush(fp) != 0) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return  SMP_OK;
}


inline SMP_BOOLEAN smp_sio_is_stream_err(FILE *fp)
{
    if (ferror(fp) == 0) return SMP_FALSE;
    return SMP_TRUE;
}


inline void smp_sio_clearerr(FILE *fp)
{
    clearerr(fp);
}


inline SMP_BOOLEAN smp_sio_is_endofstream(FILE *fp)
{
    if (feof(fp) == 1) return SMP_TRUE;
    return SMP_FALSE;
}


SMP_STATUS smp_sio_getline(FILE *fp, char *buf, int n)
{
    if (smp_unlikely(fp == NULL || buf == NULL)) {
        smp_set_error("Resource unavailable");
        return SMP_FAILURE;
    }

    if (fgets(buf, n, fp) == NULL) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


SMP_STATUS smp_sio_read(FILE *fp, void *elems, size_t elem_size, size_t elem_count)
{
    if (smp_unlikely(fp == NULL || elems == NULL)) {
        smp_set_error("Resource unavailable");
        return SMP_FAILURE;
    }

    if (fread(elems, elem_size, elem_count, fp) != elem_count) {
        if (smp_sio_is_endofstream(fp) == SMP_TRUE) return SMP_OK;
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


SMP_STATUS smp_sio_write(FILE *fp, const void *elems, size_t elem_size, size_t elem_count)
{
    int ret = 0;

    if (smp_unlikely(fp == NULL || elems == NULL)) {
        smp_set_error("Resource unavailable");
        return SMP_FAILURE;
    }

    if ((ret = fwrite(elems, elem_size, elem_count, fp)) != (int) elem_count) {
        if (ret == -1) {
            smp_set_error(strerror(errno));
            return SMP_FAILURE;
        }
        smp_set_error("Write less than the set number");
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline long smp_sio_seek_current(FILE *fp)
{
    long point = 0;

    if ((point = ftell(fp)) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return point;
}


inline SMP_STATUS smp_sio_seek(FILE *fp, long offset, int whence)
{
    if (fseek(fp, offset, whence) == -1) {
        smp_set_error(strerror(errno));
        return SMP_FAILURE;
    }

    return SMP_OK;
}


inline SMP_STATUS smp_sio_seek_current_offset(FILE *fp, long offset)
{
    return smp_sio_seek(fp, offset, SEEK_CUR);
}


inline SMP_STATUS smp_sio_seek2begin(FILE *fp)
{
    return smp_sio_seek(fp, 0, SEEK_SET);
}


inline SMP_STATUS smp_sio_seek2end(FILE *fp)
{
    return smp_sio_seek(fp, 0, SEEK_END);
}


inline SMP_STATUS smp_sio_seek_begin_offset(FILE *fp, long offset)
{
    return smp_sio_seek(fp, offset, SEEK_SET);
}


inline SMP_STATUS smp_sio_seek_end_offset(FILE *fp, long offset)
{
    return smp_sio_seek(fp, offset, SEEK_END);
}


inline SMP_STATUS smp_sio_int2string(char *buffer, int n)
{
    return sprintf(buffer, "%d", n);
}


inline SMP_STATUS smp_sio_float2string(char *buffer, float n)
{
    return sprintf(buffer, "%f", n);
}


inline SMP_STATUS smp_sio_strcat(char *buffer, const char *str1, const char *str2)
{
    return sprintf(buffer, "%s%s", str1, str2);
}

