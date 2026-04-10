/**
 * @file test_backends.c
 * @brief Unit tests for qcperf backends
 */

#include "unity.h"
#include "qcperf.h"
#include "qcperf_backends.h"
#include "qcperf_backend_enum.h"

void test_dummy_backend_init(void)
{
    enum QcPerfReturnCode ret = qcperf_init();
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_connect_backend(QC_PERF_BACKEND_DUMMY, NULL);
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_deinit();
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
}

void test_dummy_backend_info(void)
{
    enum QcPerfReturnCode ret = qcperf_init();
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_connect_backend(QC_PERF_BACKEND_DUMMY, NULL);
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
    
    struct QcPerfBackendInfo backend_info = {0};
    ret = qcperf_get_capabilities_info(QC_PERF_BACKEND_DUMMY, &backend_info);
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
    TEST_ASSERT_EQUAL(QC_PERF_BACKEND_DUMMY, backend_info.backend_id);
    TEST_ASSERT_GREATER_THAN(0, backend_info.capabilities_list_length);
    
    ret = qcperf_deinit();
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
}

void test_dummy_backend_deinit(void)
{
    enum QcPerfReturnCode ret = qcperf_init();
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_connect_backend(QC_PERF_BACKEND_DUMMY, NULL);
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_disconnect_backend(QC_PERF_BACKEND_DUMMY);
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
    
    ret = qcperf_deinit();
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
}