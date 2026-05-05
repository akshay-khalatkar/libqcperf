/*
    Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted (subject to the limitations in the
    disclaimer below) provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above
          copyright notice, this list of conditions and the following
          disclaimer in the documentation and/or other materials provided
          with the distribution.
        * Neither the name of Qualcomm Technologies, Inc. nor the names of its
          contributors may be used to endorse or promote products derived
          from this software without specific prior written permission.
    NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
    GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
    HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
    GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
    IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
    IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * @file qcom_linux_cpu_info.h
 * @brief Metric definitions and initialization functions for qcom-linux-cpu backend
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 *
 * This header contains all metric IDs, names, descriptions, and units for the
 * qcom-linux-cpu backend. Metric IDs are static compile-time constants.
 */

#ifndef QCOM_LINUX_CPU_INFO_H
#define QCOM_LINUX_CPU_INFO_H

#include <stdint.h>

#include "qcperf_common.h"

#define QCOM_LINUX_CPU_MAX_CORES 18
#define QCOM_LINUX_CPU_METRICS_PER_CORE 5
#define QCOM_LINUX_CPU_TOTAL_METRICS 2

#define QCOM_LINUX_CPU_CAPABILITY_ID 0
#define QCOM_LINUX_CPU_CAPABILITIES_LEN 1
#define QCOM_LINUX_CPU_CAPABILITY "cpu"

#define QCOM_LINUX_CPU_STREAMING_RATES_LEN 3
#define QCOM_LINUX_CPU_STREAMING_RATES 200, 500, 1000

#define QCOM_LINUX_CPU_SAMPLING_RATES_LEN 3
#define QCOM_LINUX_CPU_SAMPLING_RATES 50, 100, 200

/* ============================================================================
 * Metrics
 * ============================================================================ */

#define QCOM_LINUX_CPU_TOTAL_LOAD_ID 0     /**< Total CPU load */
#define QCOM_LINUX_CPU_TOTAL_EFF_UTIL_ID 1 /**< Total CPU effective utilization */

/* Core 0: IDs 2-6 */
#define QCOM_LINUX_CPU_CORE_0_LOAD 2
#define QCOM_LINUX_CPU_CORE_0_FREQ 3
#define QCOM_LINUX_CPU_CORE_0_STEAL_TIME 4
#define QCOM_LINUX_CPU_CORE_0_EFF_UTIL 5
#define QCOM_LINUX_CPU_CORE_0_DCVS_LIMIT 6

/* Core 1: IDs 7-11 */
#define QCOM_LINUX_CPU_CORE_1_LOAD 7
#define QCOM_LINUX_CPU_CORE_1_FREQ 8
#define QCOM_LINUX_CPU_CORE_1_STEAL_TIME 9
#define QCOM_LINUX_CPU_CORE_1_EFF_UTIL 10
#define QCOM_LINUX_CPU_CORE_1_DCVS_LIMIT 11

/* Core 2: IDs 12-16 */
#define QCOM_LINUX_CPU_CORE_2_LOAD 12
#define QCOM_LINUX_CPU_CORE_2_FREQ 13
#define QCOM_LINUX_CPU_CORE_2_STEAL_TIME 14
#define QCOM_LINUX_CPU_CORE_2_EFF_UTIL 15
#define QCOM_LINUX_CPU_CORE_2_DCVS_LIMIT 16

/* Core 3: IDs 17-21 */
#define QCOM_LINUX_CPU_CORE_3_LOAD 17
#define QCOM_LINUX_CPU_CORE_3_FREQ 18
#define QCOM_LINUX_CPU_CORE_3_STEAL_TIME 19
#define QCOM_LINUX_CPU_CORE_3_EFF_UTIL 20
#define QCOM_LINUX_CPU_CORE_3_DCVS_LIMIT 21

/* Core 4: IDs 22-26 */
#define QCOM_LINUX_CPU_CORE_4_LOAD 22
#define QCOM_LINUX_CPU_CORE_4_FREQ 23
#define QCOM_LINUX_CPU_CORE_4_STEAL_TIME 24
#define QCOM_LINUX_CPU_CORE_4_EFF_UTIL 25
#define QCOM_LINUX_CPU_CORE_4_DCVS_LIMIT 26

/* Core 5: IDs 27-31 */
#define QCOM_LINUX_CPU_CORE_5_LOAD 27
#define QCOM_LINUX_CPU_CORE_5_FREQ 28
#define QCOM_LINUX_CPU_CORE_5_STEAL_TIME 29
#define QCOM_LINUX_CPU_CORE_5_EFF_UTIL 30
#define QCOM_LINUX_CPU_CORE_5_DCVS_LIMIT 31

/* Core 6: IDs 32-36 */
#define QCOM_LINUX_CPU_CORE_6_LOAD 32
#define QCOM_LINUX_CPU_CORE_6_FREQ 33
#define QCOM_LINUX_CPU_CORE_6_STEAL_TIME 34
#define QCOM_LINUX_CPU_CORE_6_EFF_UTIL 35
#define QCOM_LINUX_CPU_CORE_6_DCVS_LIMIT 36

/* Core 7: IDs 37-41 */
#define QCOM_LINUX_CPU_CORE_7_LOAD 37
#define QCOM_LINUX_CPU_CORE_7_FREQ 38
#define QCOM_LINUX_CPU_CORE_7_STEAL_TIME 39
#define QCOM_LINUX_CPU_CORE_7_EFF_UTIL 40
#define QCOM_LINUX_CPU_CORE_7_DCVS_LIMIT 41

/* Core 8: IDs 42-46 */
#define QCOM_LINUX_CPU_CORE_8_LOAD 42
#define QCOM_LINUX_CPU_CORE_8_FREQ 43
#define QCOM_LINUX_CPU_CORE_8_STEAL_TIME 44
#define QCOM_LINUX_CPU_CORE_8_EFF_UTIL 45
#define QCOM_LINUX_CPU_CORE_8_DCVS_LIMIT 46

/* Core 9: IDs 47-51 */
#define QCOM_LINUX_CPU_CORE_9_LOAD 47
#define QCOM_LINUX_CPU_CORE_9_FREQ 48
#define QCOM_LINUX_CPU_CORE_9_STEAL_TIME 49
#define QCOM_LINUX_CPU_CORE_9_EFF_UTIL 50
#define QCOM_LINUX_CPU_CORE_9_DCVS_LIMIT 51

/* Core 10: IDs 52-56 */
#define QCOM_LINUX_CPU_CORE_10_LOAD 52
#define QCOM_LINUX_CPU_CORE_10_FREQ 53
#define QCOM_LINUX_CPU_CORE_10_STEAL_TIME 54
#define QCOM_LINUX_CPU_CORE_10_EFF_UTIL 55
#define QCOM_LINUX_CPU_CORE_10_DCVS_LIMIT 56

/* Core 11: IDs 57-61 */
#define QCOM_LINUX_CPU_CORE_11_LOAD 57
#define QCOM_LINUX_CPU_CORE_11_FREQ 58
#define QCOM_LINUX_CPU_CORE_11_STEAL_TIME 59
#define QCOM_LINUX_CPU_CORE_11_EFF_UTIL 60
#define QCOM_LINUX_CPU_CORE_11_DCVS_LIMIT 61

/* Core 12: IDs 62-66 */
#define QCOM_LINUX_CPU_CORE_12_LOAD 62
#define QCOM_LINUX_CPU_CORE_12_FREQ 63
#define QCOM_LINUX_CPU_CORE_12_STEAL_TIME 64
#define QCOM_LINUX_CPU_CORE_12_EFF_UTIL 65
#define QCOM_LINUX_CPU_CORE_12_DCVS_LIMIT 66

/* Core 13: IDs 67-71 */
#define QCOM_LINUX_CPU_CORE_13_LOAD 67
#define QCOM_LINUX_CPU_CORE_13_FREQ 68
#define QCOM_LINUX_CPU_CORE_13_STEAL_TIME 69
#define QCOM_LINUX_CPU_CORE_13_EFF_UTIL 70
#define QCOM_LINUX_CPU_CORE_13_DCVS_LIMIT 71

/* Core 14: IDs 72-76 */
#define QCOM_LINUX_CPU_CORE_14_LOAD 72
#define QCOM_LINUX_CPU_CORE_14_FREQ 73
#define QCOM_LINUX_CPU_CORE_14_STEAL_TIME 74
#define QCOM_LINUX_CPU_CORE_14_EFF_UTIL 75
#define QCOM_LINUX_CPU_CORE_14_DCVS_LIMIT 76

/* Core 15: IDs 77-81 */
#define QCOM_LINUX_CPU_CORE_15_LOAD 77
#define QCOM_LINUX_CPU_CORE_15_FREQ 78
#define QCOM_LINUX_CPU_CORE_15_STEAL_TIME 79
#define QCOM_LINUX_CPU_CORE_15_EFF_UTIL 80
#define QCOM_LINUX_CPU_CORE_15_DCVS_LIMIT 81

/* Core 16: IDs 82-86 */
#define QCOM_LINUX_CPU_CORE_16_LOAD 82
#define QCOM_LINUX_CPU_CORE_16_FREQ 83
#define QCOM_LINUX_CPU_CORE_16_STEAL_TIME 84
#define QCOM_LINUX_CPU_CORE_16_EFF_UTIL 85
#define QCOM_LINUX_CPU_CORE_16_DCVS_LIMIT 86

/* Core 17: IDs 87-91 */
#define QCOM_LINUX_CPU_CORE_17_LOAD 87
#define QCOM_LINUX_CPU_CORE_17_FREQ 88
#define QCOM_LINUX_CPU_CORE_17_STEAL_TIME 89
#define QCOM_LINUX_CPU_CORE_17_EFF_UTIL 90
#define QCOM_LINUX_CPU_CORE_17_DCVS_LIMIT 91

/* ============================================================================
 * Per-core metric offsets
 * ============================================================================ */

#define QCOM_LINUX_CPU_LOAD_OFFSET 0       /**< CPU load */
#define QCOM_LINUX_CPU_FREQ_OFFSET 1       /**< CPU frequency */
#define QCOM_LINUX_CPU_STEAL_TIME_OFFSET 2 /**< CPU steal time */
#define QCOM_LINUX_CPU_EFF_UTIL_OFFSET 3   /**< CPU effective utilization */
#define QCOM_LINUX_CPU_DCVS_LIMIT_OFFSET 4 /**< CPU DCVS frequency limit */

/* ============================================================================
 * Per-core metric ID lookup macro
 *   QCOM_LINUX_CPU_CORE_BASE_ID(core_id)                                    -> LOAD ID
 *   QCOM_LINUX_CPU_CORE_BASE_ID(core_id) + QCOM_LINUX_CPU_FREQ_OFFSET       -> FREQ ID
 *   etc.
 * ============================================================================ */

#define QCOM_LINUX_CPU_CORE_BASE_ID(core_id) ((uint16_t)(QCOM_LINUX_CPU_CORE_0_LOAD + (core_id)*QCOM_LINUX_CPU_METRICS_PER_CORE))

enum QcPerfReturnCode qcom_linux_cpu_init_metrics(struct QcPerfMetricInfo *metrics_data, uint8_t *metric_data_len);

#endif /* QCOM_LINUX_CPU_INFO_H */
