/**
 * @file test_qcperf_core.c
 * @brief Unit tests for qcperf core functionality
 */

#include "unity.h"
#include "qcperf.h"
#include "qcperf_backends.h"
#include "qcperf_backend_enum.h"

void setUp(void)
{
    /* Called before each test */
}

void tearDown(void)
{
    /* Called after each test */
}

void test_qcperf_init_success(void)
{
    QcPerfReturnCode ret = qcperf_init();
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    /* Clean up */
    qcperf_deinit();
}

void test_qcperf_deinit_success(void)
{
    QcPerfReturnCode ret = qcperf_init();
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_deinit();
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
}

void test_qcperf_connect_backend_dummy(void)
{
    QcPerfReturnCode ret = qcperf_init();
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_connect_backend(QCPERF_BACKEND_DUMMY, NULL);
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_disconnect_backend(QCPERF_BACKEND_DUMMY);
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_deinit();
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
}

void test_qcperf_disconnect_backend_dummy(void)
{
    QcPerfReturnCode ret = qcperf_init();
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_connect_backend(QCPERF_BACKEND_DUMMY, NULL);
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_disconnect_backend(QCPERF_BACKEND_DUMMY);
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_deinit();
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
}

void test_qcperf_get_backend_info(void)
{
    QcPerfReturnCode ret = qcperf_init();
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_connect_backend(QCPERF_BACKEND_DUMMY, NULL);
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    struct QcPerfBackendInfo backend_info = {0};
    ret = qcperf_get_capabilities_info(QCPERF_BACKEND_DUMMY, &backend_info);
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    TEST_ASSERT_NOT_NULL(backend_info.name);
    
    ret = qcperf_disconnect_backend(QCPERF_BACKEND_DUMMY);
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_deinit();
    TEST_ASSERT_EQUAL(QCPERF_RETURN_CODE_SUCCESS, ret);
}