#ifndef SMP_STL_H
#define SMP_STL_H

/* get container's data */
#define smp_container_data(q, type, link) (type *) \
    ((u_char *) q - offsetof(type, link))


/* a container for spoofer system */
typedef struct smp_container_s {
    struct smp_container_s *prev;
    struct smp_container_s *next;
}smp_cnt_t;

#endif // SMP_STL_H
