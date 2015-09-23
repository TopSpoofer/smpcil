#ifndef SMP_ERROR_H
#define SMP_ERROR_H

void smp_print_error();
const char *smp_get_error();
void smp_set_error(const char *error);

#endif // SMP_ERROR_H
