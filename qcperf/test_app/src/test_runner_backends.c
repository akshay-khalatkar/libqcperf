/**
 * @file test_runner_backends.c
 * @brief Test runner for qcperf backends tests
 */

#include "unity.h"

void test_dummy_backend_init(void);
void test_dummy_backend_info(void);
void test_dummy_backend_deinit(void);

void setUp(void) { }
void tearDown(void) { }

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    UNITY_BEGIN();
    
    RUN_TEST(test_dummy_backend_init);
    RUN_TEST(test_dummy_backend_info);
    RUN_TEST(test_dummy_backend_deinit);
    
    return UNITY_END();
}