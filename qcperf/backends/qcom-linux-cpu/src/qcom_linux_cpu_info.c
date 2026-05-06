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
 * @file qcom_linux_cpu_info.c
 * @brief Metric initialization implementation for qcom-linux-cpu backend
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 */

#include <stdio.h>
#include <string.h>

#include "qcom_linux_cpu_info.h"
#include "qcom_linux_cpu_utils.h"

/* ============================================================================
 * Metric name format strings
 * ============================================================================ */

#define QCOM_LINUX_CPU_METRIC_LOAD_NAME_FMT "CPU Core %u Load"
#define QCOM_LINUX_CPU_METRIC_FREQ_NAME_FMT "CPU Core %u Frequency"
#define QCOM_LINUX_CPU_METRIC_STEAL_TIME_NAME_FMT "CPU Core %u Steal Time"
#define QCOM_LINUX_CPU_METRIC_EFF_UTIL_NAME_FMT "CPU Core %u Effective Utilization"
#define QCOM_LINUX_CPU_METRIC_DCVS_LIMIT_NAME_FMT "CPU Core %u DCVS Frequency Limit"
#define QCOM_LINUX_CPU_METRIC_TOTAL_LOAD_NAME "Total CPU Load"
#define QCOM_LINUX_CPU_METRIC_TOTAL_EFF_UTIL_NAME "Total CPU Effective Utilization"

/* ============================================================================
 * Metric description format strings
 * ============================================================================ */

#define QCOM_LINUX_CPU_METRIC_LOAD_DESC_FMT "CPU utilization percentage for core %u"
#define QCOM_LINUX_CPU_METRIC_FREQ_DESC_FMT "Current CPU frequency in MHz for core %u"
#define QCOM_LINUX_CPU_METRIC_STEAL_TIME_DESC_FMT "CPU steal time percentage for core %u"
#define QCOM_LINUX_CPU_METRIC_EFF_UTIL_DESC_FMT "Effective CPU utilization percentage for core %u"
#define QCOM_LINUX_CPU_METRIC_DCVS_LIMIT_DESC_FMT "DCVS frequency limit in Hz for core %u"
#define QCOM_LINUX_CPU_METRIC_TOTAL_LOAD_DESC "Weighted total CPU utilization percentage across all cores"
#define QCOM_LINUX_CPU_METRIC_TOTAL_EFF_UTIL_DESC "Weighted total effective CPU utilization percentage across all cores"

/* ============================================================================
 * Metric unit strings
 * ============================================================================ */

#define QCOM_LINUX_CPU_METRIC_LOAD_UNIT "%%"
#define QCOM_LINUX_CPU_METRIC_FREQ_UNIT "MHz"
#define QCOM_LINUX_CPU_METRIC_STEAL_TIME_UNIT "%%"
#define QCOM_LINUX_CPU_METRIC_EFF_UTIL_UNIT "%%"
#define QCOM_LINUX_CPU_METRIC_DCVS_LIMIT_UNIT "Hz"
#define QCOM_LINUX_CPU_METRIC_TOTAL_LOAD_UNIT "%%"
#define QCOM_LINUX_CPU_METRIC_TOTAL_EFF_UTIL_UNIT "%%"

enum QcPerfReturnCode qcom_linux_cpu_init_metrics(struct QcPerfMetricInfo *metrics_data, uint8_t *metric_data_len) {
    enum QcPerfReturnCode ret = QC_PERF_RETURN_CODE_SUCCESS;
    uint32_t num_cores        = 0;
    uint32_t core_id          = 0;
    uint32_t metric_idx       = 0;
    uint32_t dcvs_limit_probe = 0U;

    ret = qcom_linux_cpu_util_get_num_cores(&num_cores);
    if (QC_PERF_RETURN_CODE_SUCCESS == ret) {
        /* ------------------------------------------------------------------ */
        /* Total metrics (IDs 0-1) — placed first                             */
        /* ------------------------------------------------------------------ */

        /* Total CPU Load */
        metrics_data[metric_idx].metric_id = QCOM_LINUX_CPU_TOTAL_LOAD_ID;
        snprintf(metrics_data[metric_idx].metric_name, METRIC_NAME_MAX_LEN, "%s", QCOM_LINUX_CPU_METRIC_TOTAL_LOAD_NAME);
        metrics_data[metric_idx].metric_name_len = strlen(metrics_data[metric_idx].metric_name);
        snprintf(metrics_data[metric_idx].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", QCOM_LINUX_CPU_METRIC_TOTAL_LOAD_DESC);
        metrics_data[metric_idx].metric_description_len = strlen(metrics_data[metric_idx].metric_description);
        snprintf(metrics_data[metric_idx].metric_unit, MAX_METRIC_UNIT_LEN, "%s", QCOM_LINUX_CPU_METRIC_TOTAL_LOAD_UNIT);
        metrics_data[metric_idx].metric_unit_len = strlen(metrics_data[metric_idx].metric_unit);
        metric_idx++;

        /* Total CPU Effective Utilization */
        metrics_data[metric_idx].metric_id = QCOM_LINUX_CPU_TOTAL_EFF_UTIL_ID;
        snprintf(metrics_data[metric_idx].metric_name, METRIC_NAME_MAX_LEN, "%s", QCOM_LINUX_CPU_METRIC_TOTAL_EFF_UTIL_NAME);
        metrics_data[metric_idx].metric_name_len = strlen(metrics_data[metric_idx].metric_name);
        snprintf(metrics_data[metric_idx].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", QCOM_LINUX_CPU_METRIC_TOTAL_EFF_UTIL_DESC);
        metrics_data[metric_idx].metric_description_len = strlen(metrics_data[metric_idx].metric_description);
        snprintf(metrics_data[metric_idx].metric_unit, MAX_METRIC_UNIT_LEN, "%s", QCOM_LINUX_CPU_METRIC_TOTAL_EFF_UTIL_UNIT);
        metrics_data[metric_idx].metric_unit_len = strlen(metrics_data[metric_idx].metric_unit);
        metric_idx++;

        /* ------------------------------------------------------------------ */
        /* Per-core metrics — IDs from static lookup table                    */
        /* ------------------------------------------------------------------ */

        for (core_id = 0; core_id < num_cores && core_id < QCOM_LINUX_CPU_MAX_CORES; core_id++) {
            /* CPU Core Load */
            metrics_data[metric_idx].metric_id = QCOM_LINUX_CPU_CORE_BASE_ID(core_id);
            snprintf(metrics_data[metric_idx].metric_name, METRIC_NAME_MAX_LEN, QCOM_LINUX_CPU_METRIC_LOAD_NAME_FMT, core_id);
            metrics_data[metric_idx].metric_name_len = strlen(metrics_data[metric_idx].metric_name);
            snprintf(metrics_data[metric_idx].metric_description, MAX_METRIC_DESCRIPTION_LEN, QCOM_LINUX_CPU_METRIC_LOAD_DESC_FMT, core_id);
            metrics_data[metric_idx].metric_description_len = strlen(metrics_data[metric_idx].metric_description);
            snprintf(metrics_data[metric_idx].metric_unit, MAX_METRIC_UNIT_LEN, "%s", QCOM_LINUX_CPU_METRIC_LOAD_UNIT);
            metrics_data[metric_idx].metric_unit_len = strlen(metrics_data[metric_idx].metric_unit);
            metric_idx++;

            /* CPU Core Frequency */
            metrics_data[metric_idx].metric_id = QCOM_LINUX_CPU_CORE_BASE_ID(core_id) + QCOM_LINUX_CPU_FREQ_OFFSET;
            snprintf(metrics_data[metric_idx].metric_name, METRIC_NAME_MAX_LEN, QCOM_LINUX_CPU_METRIC_FREQ_NAME_FMT, core_id);
            metrics_data[metric_idx].metric_name_len = strlen(metrics_data[metric_idx].metric_name);
            snprintf(metrics_data[metric_idx].metric_description, MAX_METRIC_DESCRIPTION_LEN, QCOM_LINUX_CPU_METRIC_FREQ_DESC_FMT, core_id);
            metrics_data[metric_idx].metric_description_len = strlen(metrics_data[metric_idx].metric_description);
            snprintf(metrics_data[metric_idx].metric_unit, MAX_METRIC_UNIT_LEN, "%s", QCOM_LINUX_CPU_METRIC_FREQ_UNIT);
            metrics_data[metric_idx].metric_unit_len = strlen(metrics_data[metric_idx].metric_unit);
            metric_idx++;

            /* CPU Core Steal Time */
            metrics_data[metric_idx].metric_id = QCOM_LINUX_CPU_CORE_BASE_ID(core_id) + QCOM_LINUX_CPU_STEAL_TIME_OFFSET;
            snprintf(metrics_data[metric_idx].metric_name, METRIC_NAME_MAX_LEN, QCOM_LINUX_CPU_METRIC_STEAL_TIME_NAME_FMT, core_id);
            metrics_data[metric_idx].metric_name_len = strlen(metrics_data[metric_idx].metric_name);
            snprintf(metrics_data[metric_idx].metric_description, MAX_METRIC_DESCRIPTION_LEN, QCOM_LINUX_CPU_METRIC_STEAL_TIME_DESC_FMT, core_id);
            metrics_data[metric_idx].metric_description_len = strlen(metrics_data[metric_idx].metric_description);
            snprintf(metrics_data[metric_idx].metric_unit, MAX_METRIC_UNIT_LEN, "%s", QCOM_LINUX_CPU_METRIC_STEAL_TIME_UNIT);
            metrics_data[metric_idx].metric_unit_len = strlen(metrics_data[metric_idx].metric_unit);
            metric_idx++;

            /* CPU Core Effective Utilization */
            metrics_data[metric_idx].metric_id = QCOM_LINUX_CPU_CORE_BASE_ID(core_id) + QCOM_LINUX_CPU_EFF_UTIL_OFFSET;
            snprintf(metrics_data[metric_idx].metric_name, METRIC_NAME_MAX_LEN, QCOM_LINUX_CPU_METRIC_EFF_UTIL_NAME_FMT, core_id);
            metrics_data[metric_idx].metric_name_len = strlen(metrics_data[metric_idx].metric_name);
            snprintf(metrics_data[metric_idx].metric_description, MAX_METRIC_DESCRIPTION_LEN, QCOM_LINUX_CPU_METRIC_EFF_UTIL_DESC_FMT, core_id);
            metrics_data[metric_idx].metric_description_len = strlen(metrics_data[metric_idx].metric_description);
            snprintf(metrics_data[metric_idx].metric_unit, MAX_METRIC_UNIT_LEN, "%s", QCOM_LINUX_CPU_METRIC_EFF_UTIL_UNIT);
            metrics_data[metric_idx].metric_unit_len = strlen(metrics_data[metric_idx].metric_unit);
            metric_idx++;

            if (QC_PERF_RETURN_CODE_SUCCESS == qcom_linux_cpu_util_get_dcvs_limit(core_id, &dcvs_limit_probe)) {
                metrics_data[metric_idx].metric_id = QCOM_LINUX_CPU_CORE_BASE_ID(core_id) + QCOM_LINUX_CPU_DCVS_LIMIT_OFFSET;
                snprintf(metrics_data[metric_idx].metric_name, METRIC_NAME_MAX_LEN, QCOM_LINUX_CPU_METRIC_DCVS_LIMIT_NAME_FMT, core_id);
                metrics_data[metric_idx].metric_name_len = strlen(metrics_data[metric_idx].metric_name);
                snprintf(metrics_data[metric_idx].metric_description, MAX_METRIC_DESCRIPTION_LEN, QCOM_LINUX_CPU_METRIC_DCVS_LIMIT_DESC_FMT, core_id);
                metrics_data[metric_idx].metric_description_len = strlen(metrics_data[metric_idx].metric_description);
                snprintf(metrics_data[metric_idx].metric_unit, MAX_METRIC_UNIT_LEN, "%s", QCOM_LINUX_CPU_METRIC_DCVS_LIMIT_UNIT);
                metrics_data[metric_idx].metric_unit_len = strlen(metrics_data[metric_idx].metric_unit);
                metric_idx++;
            }
        }

        *metric_data_len = (uint8_t)metric_idx;
    }

    return ret;
}
