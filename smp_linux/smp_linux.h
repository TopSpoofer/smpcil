#ifndef SMP_LINUX_H
#define SMP_LINUX_H


#define SMP_INT_MAX(a,b) ((((int)((a)-(b))) & 0x80000000) ? b : a)

#define SMP_CMP_MORETHAN    (1)
#define SMP_CMP_EQUAL       (0)
#define SMP_CMP_LESSTHAN    (-1)
#define SMP_CMP_NOTEQUAL    (-2)
#define SMP_CMP_POINTLESS   (-3)


#endif // SMP_LINUX_H
