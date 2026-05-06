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
 * @file qcom_linux_cpu_utils.c
 * @brief Sysfs/procfs utility function implementations for the qcom-linux-cpu backend
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 *
 * This file implements the internal utility functions used to read CPU
 * topology, capacity, frequency, load, and DCVS limits from sysfs and
 * procfs. These utilities are shared between qcom_linux_cpu.c and
 * qcom_linux_cpu_info.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcom_linux_cpu_utils.h"

enum QcPerfReturnCode qcom_linux_cpu_util_get_num_cores(uint32_t *num_cores) {
    enum QcPerfReturnCode ret = QC_PERF_RETURN_CODE_FAILED;
    FILE *fp                  = NULL;
    uint32_t last_core        = 0;

    fp = fopen(QCOM_LINUX_CPU_COUNT_NODE, "r");
    if (NULL != fp) {
        if (fscanf(fp, "%*u-%u", &last_core) == 1) {
            *num_cores = last_core + 1U;
            ret        = QC_PERF_RETURN_CODE_SUCCESS;
        }
        fclose(fp);
    }

    return ret;
}

enum QcPerfReturnCode qcom_linux_cpu_util_get_core_capacity(uint32_t core_id, uint32_t *capacity) {
    enum QcPerfReturnCode ret = QC_PERF_RETURN_CODE_FAILED;
    char path[QCOM_LINUX_CPU_NODE_PATH_MAX_LEN];
    FILE *fp = NULL;

    snprintf(path, sizeof(path), QCOM_LINUX_CPU_CAPACITY_NODE_FMT, core_id);
    fp = fopen(path, "r");
    if (NULL != fp) {
        if (fscanf(fp, "%u", capacity) == 1) {
            ret = QC_PERF_RETURN_CODE_SUCCESS;
        }
        fclose(fp);
    }

    return ret;
}

enum QcPerfReturnCode qcom_linux_cpu_util_get_total_capacity(uint32_t *total_capacity) {
    enum QcPerfReturnCode ret = QC_PERF_RETURN_CODE_FAILED;
    uint32_t num_cores        = 0;
    uint32_t core             = 0;
    uint32_t cap              = 0;

    *total_capacity = 0U;

    ret = qcom_linux_cpu_util_get_num_cores(&num_cores);
    if (QC_PERF_RETURN_CODE_SUCCESS == ret) {
        for (core = 0; core < num_cores; core++) {
            ret = qcom_linux_cpu_util_get_core_capacity(core, &cap);
            if (QC_PERF_RETURN_CODE_SUCCESS != ret) {
                break;
            }
            *total_capacity += cap;
        }
    }

    if ((QC_PERF_RETURN_CODE_SUCCESS == ret) && (0U == *total_capacity)) {
        ret = QC_PERF_RETURN_CODE_FAILED;
    }

    return ret;
}

enum QcPerfReturnCode qcom_linux_cpu_util_get_proc_stat(struct QcomLinuxCpuLoadInfo *load_info, uint32_t num_cores) {
    enum QcPerfReturnCode ret = QC_PERF_RETURN_CODE_FAILED;
    FILE *fp                  = NULL;
    char line[QCOM_LINUX_CPU_STAT_LINE_MAX_LEN];
    uint32_t core_id    = 0;
    uint32_t cores_read = 0;
    double user         = 0.0;
    double nice         = 0.0;
    double system       = 0.0;
    double idle         = 0.0;
    double iowait       = 0.0;
    double irq          = 0.0;
    double softirq      = 0.0;
    double steal        = 0.0;
    double guest        = 0.0;
    double guest_nice   = 0.0;
    int scan_count      = 0;

    fp = fopen(QCOM_LINUX_CPU_STAT_NODE, "r");
    if (NULL != fp) {
        /* Skip the aggregate "cpu" line */
        if (NULL != fgets(line, sizeof(line), fp)) {
            while ((NULL != fgets(line, sizeof(line), fp)) && (cores_read < num_cores)) {
                scan_count = sscanf(line, "cpu%u %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &core_id, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
                if ((QCOM_LINUX_CPU_STAT_FIELD_COUNT == scan_count) && (core_id < num_cores)) {
                    load_info[core_id].idle       = idle + iowait;
                    load_info[core_id].total_time = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
                    load_info[core_id].steal_time = steal;
                    cores_read++;
                }
            }
            if (cores_read > 0U) {
                ret = QC_PERF_RETURN_CODE_SUCCESS;
            }
        }
        fclose(fp);
    }

    return ret;
}

enum QcPerfReturnCode qcom_linux_cpu_util_get_core_frequency(uint32_t core_id, double *frequency_mhz) {
    enum QcPerfReturnCode ret = QC_PERF_RETURN_CODE_FAILED;
    char path[QCOM_LINUX_CPU_NODE_PATH_MAX_LEN];
    FILE *fp      = NULL;
    long freq_khz = 0;

    snprintf(path, sizeof(path), QCOM_LINUX_CPU_FREQ_NODE_FMT, core_id);
    fp = fopen(path, "r");
    if (NULL == fp) {
        *frequency_mhz = 0.0;
    } else {
        if (fscanf(fp, "%ld", &freq_khz) == 1) {
            *frequency_mhz = (double)freq_khz / (double)QCOM_LINUX_CPU_KHZ_TO_MHZ;
            ret            = QC_PERF_RETURN_CODE_SUCCESS;
        } else {
            *frequency_mhz = 0.0;
        }
        fclose(fp);
    }

    return ret;
}

enum QcPerfReturnCode qcom_linux_cpu_util_get_dcvs_limit(uint32_t core_id, uint32_t *dcvs_limit) {
    enum QcPerfReturnCode ret = QC_PERF_RETURN_CODE_FAILED;
    char path[QCOM_LINUX_CPU_NODE_PATH_MAX_LEN];
    FILE *fp = NULL;

    snprintf(path, sizeof(path), QCOM_LINUX_CPU_DCVS_NODE_FMT, core_id);
    fp = fopen(path, "r");
    if (NULL != fp) {
        if (fscanf(fp, "%u", dcvs_limit) == 1) {
            ret = QC_PERF_RETURN_CODE_SUCCESS;
        } else {
            *dcvs_limit = 0U;
        }
        fclose(fp);
    } else {
        *dcvs_limit = 0U;
    }

    return ret;
}
