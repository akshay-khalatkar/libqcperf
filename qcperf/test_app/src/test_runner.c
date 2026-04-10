/**
 * @file test_runner.c
 * @brief Main test runner for qcperf core tests
 */

#include "unity.h"

void test_qcperf_init(void);
void test_qcperf_deinit(void);
void test_qcperf_acquire_handle(void);
void test_qcperf_release_handle(void);
void test_qcperf_backend_selection(void);

void setUp(void) { }
void tearDown(void) { }

int main(int argc, char *argv[])
{
    UNITY_BEGIN();
    
    RUN_TEST(test_qcperf_init);
    RUN_TEST(test_qcperf_deinit);
    RUN_TEST(test_qcperf_acquire_handle);
    RUN_TEST(test_qcperf_release_handle);
    RUN_TEST(test_qcperf_backend_selection);
    
    return UNITY_END();
}