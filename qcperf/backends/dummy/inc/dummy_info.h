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
 * @file dummy_info.h
 * @brief Static metric definitions and initialization functions for dummy backend
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 * This header defines all metric IDs, names, descriptions, and units for the
 * dummy backend's capabilities. It also provides initialization functions to
 * populate metric information structures with these static definitions.
 */

#ifndef DUMMY_INFO_H
#define DUMMY_INFO_H

#include "qcperf_common.h"

#define DUMMY_CAPABILITY_0_ID 0
#define DUMMY_CAPABILITY_1_ID 1
#define DUMMY_CAPABILITIES_LEN 2

#define DUMMY_STREAMING_RATES_LEN 20
#define DUMMY_STREAMING_RATES 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000

#define DUMMY_SAMPLING_RATES_LEN 7
#define DUMMY_SAMPLING_RATES 1, 10, 50, 100, 200, 500, 1000

#define CAPABILITY_0 "capability_0"
#define CAPABILITY_0_METRIC_COUNT 5

/**
 * @enum DummyCapability1MetricIndex
 * @brief Array indices for Capability 1 metrics
 *
 * This enum defines the array indices for accessing metrics in the
 * metrics_data array for Capability 1, providing better code readability.
 */
enum DummyCapability1MetricIndex {
    DUMMY_CAPABILITY_0_METRIC_INDEX_0 = 0,
    DUMMY_CAPABILITY_0_METRIC_INDEX_1,
    DUMMY_CAPABILITY_0_METRIC_INDEX_2,
    DUMMY_CAPABILITY_0_METRIC_INDEX_3,
    DUMMY_CAPABILITY_0_METRIC_INDEX_4
};

#define CAPABILITY_1 "capability_1"
#define CAPABILITY_1_METRIC_COUNT 10

/**
 * @enum DummyCapability2MetricIndex
 * @brief Array indices for Capability 2 metrics
 *
 * This enum defines the array indices for accessing metrics in the
 * metrics_data array for Capability 2, providing better code readability.
 */
enum DummyCapability2MetricIndex {
    DUMMY_CAPABILITY_1_METRIC_INDEX_0 = 0,
    DUMMY_CAPABILITY_1_METRIC_INDEX_1,
    DUMMY_CAPABILITY_1_METRIC_INDEX_2,
    DUMMY_CAPABILITY_1_METRIC_INDEX_3,
    DUMMY_CAPABILITY_1_METRIC_INDEX_4,
    DUMMY_CAPABILITY_1_METRIC_INDEX_5,
    DUMMY_CAPABILITY_1_METRIC_INDEX_6,
    DUMMY_CAPABILITY_1_METRIC_INDEX_7,
    DUMMY_CAPABILITY_1_METRIC_INDEX_8,
    DUMMY_CAPABILITY_1_METRIC_INDEX_9,
};

// Capability Metric Definitions
#define DUMMY_METRIC_0_ID 0
#define DUMMY_METRIC_0_NAME "Performance metrics 0"
#define DUMMY_METRIC_0_DESCRIPTION "Dummy metric description for metric 0"
#define DUMMY_METRIC_0_UNIT "%%"

#define DUMMY_METRIC_1_ID 1
#define DUMMY_METRIC_1_NAME "Performance metrics 1"
#define DUMMY_METRIC_1_DESCRIPTION "Dummy metric description for metric 1"
#define DUMMY_METRIC_1_UNIT "MHz"

#define DUMMY_METRIC_2_ID 2
#define DUMMY_METRIC_2_NAME "Performance metrics 2"
#define DUMMY_METRIC_2_DESCRIPTION "Dummy metric description for metric 2"
#define DUMMY_METRIC_2_UNIT "GHz"

#define DUMMY_METRIC_3_ID 3
#define DUMMY_METRIC_3_NAME "Performance metrics 103"
#define DUMMY_METRIC_3_DESCRIPTION "Dummy metric description for metric 3"
#define DUMMY_METRIC_3_UNIT "counter"

#define DUMMY_METRIC_4_ID 4
#define DUMMY_METRIC_4_NAME "Performance metrics 4"
#define DUMMY_METRIC_4_DESCRIPTION "Dummy metric description for metric 4"
#define DUMMY_METRIC_4_UNIT "MBps"

#define DUMMY_METRIC_5_ID 5
#define DUMMY_METRIC_5_NAME "Performance metrics 5"
#define DUMMY_METRIC_5_DESCRIPTION "Dummy metric description for metric 5"
#define DUMMY_METRIC_5_UNIT "MHz"

#define DUMMY_METRIC_6_ID 6
#define DUMMY_METRIC_6_NAME "Performance metrics 6"
#define DUMMY_METRIC_6_DESCRIPTION "Dummy metric description for metric 6"
#define DUMMY_METRIC_6_UNIT "%%"

#define DUMMY_METRIC_7_ID 7
#define DUMMY_METRIC_7_NAME "Performance metrics 7"
#define DUMMY_METRIC_7_DESCRIPTION "Dummy metric description for metric 7"
#define DUMMY_METRIC_7_UNIT "GHz"

#define DUMMY_METRIC_8_ID 8
#define DUMMY_METRIC_8_NAME "Performance metrics 8"
#define DUMMY_METRIC_8_DESCRIPTION "Dummy metric description for metric 8"
#define DUMMY_METRIC_8_UNIT "counter"

#define DUMMY_METRIC_9_ID 9
#define DUMMY_METRIC_9_NAME "Performance metrics 9"
#define DUMMY_METRIC_9_DESCRIPTION "Dummy metric description for metric 9"
#define DUMMY_METRIC_9_UNIT "MBps"

/**
 * @brief Initialize Capability 1 metrics data
 *
 * Populates the provided metrics_data array with metric information for
 * Capability 1, including metric IDs, names, descriptions, and units.
 *
 * @param[out] metrics_data Array to be populated with Capability 1 metric information
 *                          (must have space for CAPABILITY_0_METRIC_COUNT entries)
 */
void dummy_capability_0_init_metrics(struct QcPerfMetricInfo *metrics_data);

/**
 * @brief Initialize Capability 2 metrics data
 *
 * Populates the provided metrics_data array with metric information for
 * Capability 2, including metric IDs, names, descriptions, and units.
 *
 * @param[out] metrics_data Array to be populated with Capability 2 metric information
 *                          (must have space for CAPABILITY_1_METRIC_COUNT entries)
 */
void dummy_capability_1_init_metrics(struct QcPerfMetricInfo *metrics_data);

#endif /* DUMMY_INFO_H */
