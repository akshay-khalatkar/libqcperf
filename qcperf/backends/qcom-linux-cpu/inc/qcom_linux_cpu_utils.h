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
 * @file qcom_linux_cpu_utils.h
 * @brief Sysfs/procfs utility functions for the qcom-linux-cpu backend
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 *
 * This header declares the internal utility functions used to read CPU
 * topology, capacity, frequency, load, and DCVS limits from sysfs and
 * procfs. These utilities are shared between qcom_linux_cpu.c and
 * qcom_linux_cpu_info.c.
 */

#ifndef QCOM_LINUX_CPU_UTILS_H
#define QCOM_LINUX_CPU_UTILS_H

#include <stdbool.h>
#include <stdint.h>

#include "qcperf_common.h"

/* ============================================================================
 * Sysfs / procfs Path Definitions
 * ============================================================================ */

/** Path to read the number of possible CPU cores (e.g., "0-7") */
#define QCOM_LINUX_CPU_COUNT_NODE "/sys/devices/system/cpu/possible"

/** Path to read per-core CPU statistics */
#define QCOM_LINUX_CPU_STAT_NODE "/proc/stat"

/** Format string for per-core current CPU frequency node (value in kHz) */
#define QCOM_LINUX_CPU_FREQ_NODE_FMT "/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_cur_freq"

/** Format string for per-core CPU capacity node */
#define QCOM_LINUX_CPU_CAPACITY_NODE_FMT "/sys/devices/system/cpu/cpu%u/cpu_capacity"

/** Format string for per-core DCVS frequency limit node (value in Hz) */
#define QCOM_LINUX_CPU_DCVS_NODE_FMT "/sys/devices/system/cpu/cpu%u/dcvsh_freq_limit"

/** Maximum length for sysfs node path strings */
#define QCOM_LINUX_CPU_NODE_PATH_MAX_LEN 128

/** Maximum length for a single line read from /proc/stat */
#define QCOM_LINUX_CPU_STAT_LINE_MAX_LEN 256

/** Conversion factor from kHz to MHz */
#define QCOM_LINUX_CPU_KHZ_TO_MHZ 1000

/** Conversion factor from ms to us for usleep */
#define QCOM_LINUX_CPU_MS_TO_US 1000

/** Number of fields expected in a /proc/stat per-core line (cpu_id + 10 values) */
#define QCOM_LINUX_CPU_STAT_FIELD_COUNT 11

/**
 * @struct QcomLinuxCpuLoadInfo
 * @brief Stores a single /proc/stat snapshot for delta-based utilization calculation
 */
struct QcomLinuxCpuLoadInfo {
    double idle;       /**< Idle + iowait ticks */
    double total_time; /**< Sum of all CPU time ticks */
    double steal_time; /**< Steal time ticks */
};

/**
 * @brief Read the number of CPU cores from /sys/devices/system/cpu/possible
 *
 * Parses the "0-N" format to determine the total core count (N+1).
 *
 * @param[out] num_cores Populated with the number of CPU cores
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_FAILED if the node cannot be opened or parsed
 */
enum QcPerfReturnCode qcom_linux_cpu_util_get_num_cores(uint32_t *num_cores);

/**
 * @brief Read CPU capacity for a single core from sysfs
 *
 * Reads the cpu_capacity (or cpuinfo_max_freq) sysfs node for the given core.
 *
 * @param[in]  core_id  CPU core index
 * @param[out] capacity Populated with the capacity value for the core
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_FAILED if the node cannot be opened or parsed
 */
enum QcPerfReturnCode qcom_linux_cpu_util_get_core_capacity(uint32_t core_id, uint32_t *capacity);

/**
 * @brief Read and sum CPU capacity across all cores
 *
 * Calls qcom_linux_cpu_util_get_num_cores() to determine the core count, then
 * calls qcom_linux_cpu_util_get_core_capacity() for each core and accumulates
 * the total.
 *
 * @param[out] total_capacity Sum of all per-core capacity values
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_FAILED if the core count or any core's capacity cannot be read, or total is zero
 */
enum QcPerfReturnCode qcom_linux_cpu_util_get_total_capacity(uint32_t *total_capacity);

/**
 * @brief Read /proc/stat and populate per-core load snapshots
 *
 * Parses each "cpuN ..." line and stores idle, total, and steal ticks.
 *
 * @param[out] load_info Array of per-core load snapshots (must have num_cores entries)
 * @param[in]  num_cores Number of cores to read
 * @return QC_PERF_RETURN_CODE_SUCCESS if at least one core was read
 * @return QC_PERF_RETURN_CODE_FAILED if the file cannot be opened or no cores were parsed
 */
enum QcPerfReturnCode qcom_linux_cpu_util_get_proc_stat(struct QcomLinuxCpuLoadInfo *load_info, uint32_t num_cores);

/**
 * @brief Read the current CPU frequency for a specific core
 *
 * Reads the cpuinfo_cur_freq (or scaling_cur_freq) sysfs node (value in kHz)
 * and converts to MHz.
 *
 * @param[in]  core_id       CPU core index
 * @param[out] frequency_mhz Populated with the current frequency in MHz (0.0 on failure)
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_FAILED if the node cannot be read
 */
enum QcPerfReturnCode qcom_linux_cpu_util_get_core_frequency(uint32_t core_id, double *frequency_mhz);

/**
 * @brief Read the DCVS frequency limit for a specific core
 *
 * Reads /sys/devices/system/cpu/cpu%u/dcvsh_freq_limit (value in Hz).
 *
 * @param[in]  core_id    CPU core index
 * @param[out] dcvs_limit Populated with the DCVS frequency limit in Hz
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_FAILED if the node is unavailable or cannot be parsed
 */
enum QcPerfReturnCode qcom_linux_cpu_util_get_dcvs_limit(uint32_t core_id, uint32_t *dcvs_limit);

#endif /* QCOM_LINUX_CPU_UTILS_H */
