//-----------------------------------------------------------------------------
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// All data and information contained in or disclosed by this document are
// confidential and proprietary information of Qualcomm Technologies, Inc., and
// all rights therein are expressly reserved. By accepting this material, the
// recipient agrees that this material and the information contained therein
// are held in confidence and in trust and will not be used, copied, reproduced
// in whole or in part, nor its contents revealed in any manner to others
// without the express written permission of Qualcomm Technologies, Inc.
//
// This software may be subject to U.S. and international export, re-export, or
// transfer ("export") laws.  Diversion contrary to U.S. and international law
// is strictly prohibited.
//-----------------------------------------------------------------------------

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
