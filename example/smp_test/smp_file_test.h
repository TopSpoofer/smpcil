#ifndef SMP_FILE_TEST_H
#define SMP_FILE_TEST_H

SMP_STATUS smp_test_create_file(const char *file);
SMP_STATUS smp_test_read_file(const char *file);
SMP_STATUS smp_test_write_file(const char *file);
SMP_STATUS smp_test_apend_file(const char *file);
void smp_test_file_main();
void smp_test_sio_main();
#endif // SMP_FILE_TEST_H
