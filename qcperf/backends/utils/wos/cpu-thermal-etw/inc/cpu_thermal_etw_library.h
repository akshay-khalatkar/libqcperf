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
 * @file cpu_thermal_etw_library.h
 * @brief CPU and thermal monitoring library using ETW
 * @author Qualcomm Technologies, Inc.
 *
 * This file provides functions and structures for monitoring CPU performance
 * metrics (frequency and utilization) and thermal sensor data on Windows
 * platforms using Event Tracing for Windows (ETW) and Windows Management
 * Instrumentation (WMI).
 */

#ifndef CPU_THERMAL_ETW_LIBRARY_H
#define CPU_THERMAL_ETW_LIBRARY_H

#include <stdint.h>

#include "thermal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return codes for CPU thermal ETW operations
 */
enum CpuThermalETWReturnCode {
    RETURN_CODE_CPU_THERMAL_SUCCESS = 0,                  /**< Operation successful */
    RETURN_CODE_CPU_THERMAL_ERROR_CO_INITIALIZE_EX,       /**< COM initialization failed */
    RETURN_CODE_CPU_THERMAL_ERROR_CO_INITIALIZE_SECURITY, /**< COM security initialization failed */
    RETURN_CODE_CPU_THERMAL_ERROR_CO_CREATE_INSTANCE,     /**< COM instance creation failed */
    RETURN_CODE_CPU_THERMAL_ERROR_CONNECT_SERVER,         /**< Failed to connect to WMI server */
    RETURN_CODE_CPU_THERMAL_ERROR_CO_SET_PROXY_BLANKET,   /**< Failed to set COM proxy blanket */
    RETURN_CODE_CPU_THERMAL_ERROR_EXEC_QUERY,             /**< Failed to execute WMI query */
    RETURN_CODE_CPU_THERMAL_ERROR_WMI_GET,                /**< Failed to get WMI data */
    RETURN_CODE_CPU_THERMAL_ERROR_UNKNOWN_QUERY,          /**< Unknown query type */
    RETURN_CODE_CPU_ERROR_NO_CORES,                       /**< No CPU cores found */
    RETURN_CODE_THERMAL_ERROR_NO_ZONES,                   /**< No thermal zones found */
    RETURN_CODE_CPU_THERMAL_ERROR_CALLOC,                 /**< Memory allocation failed */
    RETURN_CODE_CPU_THERMAL_ERROR_NULL_POINTER,           /**< Null pointer provided */
    RETURN_CODE_CPU_ERROR_LIB_UNINITIALIZED,              /**< CPU library not initialized */
    RETURN_CODE_CPU_ERROR_INVALID_ARG,                    /**< Invalid argument */
    RETURN_CODE_CPU_ERROR_OPEN_QUERY,                     /**< Failed to open performance query */
    RETURN_CODE_CPU_ERROR_UTIL_ADD_COUNTER,               /**< Failed to add utilization counter */
    RETURN_CODE_CPU_ERROR_FREQ_ADD_COUNTER,               /**< Failed to add frequency counter */
    RETURN_CODE_CPU_ERROR_COLLECT_QUERY,                  /**< Failed to collect query data */
    RETURN_CODE_CPU_ERROR_CLOSE_QUERY,                    /**< Failed to close query */
    RETURN_CODE_THERMAL_ERROR_LIB_UNINITIALIZED,          /**< Thermal library not initialized */
    RETURN_CODE_THERMAL_ERROR_INVALID_ARG,                /**< Invalid argument for thermal function */
    RETURN_CODE_CPU_ERROR_ALREADY_INITIALIZED,            /**< CPU library already initialized */
    RETURN_CODE_THERMAL_ERROR_ALREADY_INITIALIZED,        /**< Thermal library already initialized */
    RETURN_CODE_NUM_OF_ZONE_EXCEEDED,                     /**< Number of zones exceeded maximum */
    RETURN_CODE_INVALID_QUERY,                            /**< Invalid query */
    RETURN_CODE_CPU_THERMAL_NATIVE_FUNCTION_FAILED,       /**< Native function call failed */
    RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION,             /**< Caught exception */
};

/**
 * @brief CPU information types
 */
enum CpuInfoType {
    ALL,         /**< Both frequency and utilization */
    FREQUENCY,   /**< CPU frequency only */
    UTILIZATION, /**< CPU utilization only */
};

/**
 * @brief Structure for CPU information for a single core
 */
struct CpuInfo {
    enum CpuInfoType info_type; /**< Type of information requested */
    uint32_t core_id;           /**< CPU core ID */
    double frequency;           /**< CPU frequency in MHz */
    double utilization;         /**< CPU utilization percentage (0-100) */
};

/**
 * @brief Structure for CPU information query results
 */
struct CpuInfoQuery {
    enum CpuInfoType info_type; /**< Type of information requested */
    struct CpuInfo* cpu_info;   /**< Array of CPU core information */
    uint32_t cpu_info_len;      /**< Length of cpu_info array */
    double total_frequency;     /**< Average frequency across all cores */
    double total_utilization;   /**< Average utilization across all cores */
};

/**
 * @brief Structure for thermal information query results
 */
struct ThermalInfoQuery {
    double* temperatures;        /**< Array of temperature values */
    uint8_t temperatures_length; /**< Length of temperatures array */
};

/**
 * @brief Initialize thermal sensor monitoring
 *
 * @param map Thermal zone name mapping
 * @return enum CpuThermalETWReturnCode Return code indicating success or failure
 */
DllExport enum CpuThermalETWReturnCode thermal_sensor_init(struct ThermalCommonZoneNameMap* map);

/**
 * @brief Initialize CPU monitoring
 *
 * @return enum CpuThermalETWReturnCode Return code indicating success or failure
 */
DllExport enum CpuThermalETWReturnCode cpu_init();

/**
 * @brief Get number of CPU cores
 *
 * @param num_cores Pointer to store the number of cores
 * @return enum CpuThermalETWReturnCode Return code indicating success or failure
 */
DllExport enum CpuThermalETWReturnCode cpu_get_num_cores(uint8_t* num_cores);

/**
 * @brief Get CPU information
 *
 * First call yields garbage values. Make sure to only use from second call onwards.
 * Still needs valid sampling rate (1000ms) between first and second call.
 *
 * @param cpu_info_query Query structure to fill with CPU information
 * @return enum CpuThermalETWReturnCode Return code indicating success or failure
 */
DllExport enum CpuThermalETWReturnCode cpu_get_info(struct CpuInfoQuery* cpu_info_query);

/**
 * @brief Get thermal sensor information
 *
 * @param request Query structure to fill with thermal information
 * @return enum CpuThermalETWReturnCode Return code indicating success or failure
 */
DllExport enum CpuThermalETWReturnCode thermal_sensor_get_info(struct ThermalInfoQuery* request);

/**
 * @brief Clean up thermal sensor monitoring resources
 *
 * @return enum CpuThermalETWReturnCode Return code indicating success or failure
 */
DllExport enum CpuThermalETWReturnCode thermal_sensor_cleanup();

/**
 * @brief Clean up CPU monitoring resources
 *
 * @return enum CpuThermalETWReturnCode Return code indicating success or failure
 */
DllExport enum CpuThermalETWReturnCode cpu_cleanup();

#ifdef __cplusplus
}
#endif

#endif  // CPU_THERMAL_ETW_LIBRARY_H
