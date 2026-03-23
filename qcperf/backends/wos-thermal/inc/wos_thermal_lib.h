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
 * @file wos_thermal_lib.h
 * @brief WOS Thermal library implementation for libqcperf
 * @author Skand Gupta (skangupt@qti.qualcomm.com)
 *
 * This header provides the implementation for the WOS (Windows on Snapdragon)
 * Thermal library, which collects temperature and passive cooling data from
 * various thermal zones. It provides functions for initializing the library,
 * starting and stopping data collection, and retrieving thermal information.
 *
 * The library uses high-precision nanosecond timestamps for all time measurements,
 * providing accurate timing information for performance profiling. Sampling and
 * streaming rates are specified in milliseconds but internally converted to
 * nanoseconds for precise timing calculations.
 */

#ifndef WOS_THERMAL_LIB_H
#define WOS_THERMAL_LIB_H

#include <stdint.h>
#include <stdbool.h>
#include "qcperf_common.h"
/**
 * @def MAX_THERMAL_ZONES
 * @brief Maximum number of thermal zones supported
 */
#define MAX_THERMAL_ZONES 64

/**
 * @enum WosThermalLibReturnCode
 * @brief Return codes for WOS Thermal library functions
 */
enum WosThermalLibReturnCode {
    WOS_THERMAL_LIB_SUCCESS = 0,
    WOS_THERMAL_LIB_ERROR_FAILED,
    WOS_THERMAL_LIB_ERROR_INIT_FAILED,
    WOS_THERMAL_LIB_ERROR_ALREADY_INITIALIZED,
    WOS_THERMAL_LIB_ERROR_NOT_INITIALIZED,
    WOS_THERMAL_LIB_ERROR_NULL_POINTER,
    WOS_THERMAL_LIB_ERROR_MEMORY_ALLOCATION,
    WOS_THERMAL_LIB_ERROR_INVALID_ARGUMENT,
    WOS_THERMAL_LIB_ERROR_NATIVE_FUNCTION_FAILED,
    WOS_THERMAL_LIB_ERROR_THREAD_CREATE_FAILED,
    WOS_THERMAL_LIB_ERROR_THREAD_JOIN_FAILED,
    WOS_THERMAL_LIB_ERROR_THREAD_DESTROY_FAILED,
    WOS_THERMAL_LIB_ERROR_MUTEX_OPERATION_FAILED,
    WOS_THERMAL_LIB_ERROR_THERMAL_SENSOR_INIT_FAILED,
    WOS_THERMAL_LIB_ERROR_PASSIVE_COOLING_INIT_FAILED,
    WOS_THERMAL_LIB_ERROR_THERMAL_COMMON_INIT_FAILED,
    WOS_THERMAL_LIB_ERROR_GET_ZONE_NAMES_FAILED,
    WOS_THERMAL_LIB_ERROR_GET_THERMAL_INFO_FAILED,
    WOS_THERMAL_LIB_ERROR_GET_PASSIVE_COOLING_INFO_FAILED,
    WOS_THERMAL_LIB_ERROR_THERMAL_SENSOR_CLEANUP_FAILED,
    WOS_THERMAL_LIB_ERROR_PASSIVE_COOLING_CLEANUP_FAILED,
    WOS_THERMAL_LIB_ERROR_THERMAL_COMMON_CLEANUP_FAILED,
};

/**
 * @struct WosThermalZoneData
 * @brief Structure to hold thermal zone data
 */
struct WosThermalZoneData {
    uint8_t zone_id;        /**< Thermal zone ID */
    char zone_name[64];     /**< Thermal zone name */
    size_t zone_name_length;
    double temperature;     /**< Temperature in Celsius */
    double passive_cooling; /**< Passive cooling percentage */
};

/**
 * @struct WosThermalData
 * @brief Structure to hold thermal data for all zones
 */
struct WosThermalData {
    struct WosThermalZoneData zones[MAX_THERMAL_ZONES]; /**< Array of thermal zone data */
    uint8_t zone_count;                                 /**< Number of valid zones */
    uint64_t timestamp;                                 /**< Timestamp when data was collected (in nanoseconds) */
};

/**
 * @typedef WosThermalDataCallback
 * @brief Callback function type for thermal data updates
 */
typedef void (*WosThermalDataCallback)(struct WosThermalData* data);

/**
 * @brief Initialize the WOS Thermal library
 *
 * This function initializes the WOS Thermal library, including the thermal
 * sensor, passive cooling, and thermal common components.
 *
 * @return WOS_THERMAL_LIB_SUCCESS on success
 * @return Error code on failure
 */
enum WosThermalLibReturnCode wos_thermal_lib_init(void);

/**
 * @brief Clean up the WOS Thermal library
 *
 * This function cleans up resources used by the WOS Thermal library.
 *
 * @return WOS_THERMAL_LIB_SUCCESS on success
 * @return Error code on failure
 */
enum WosThermalLibReturnCode wos_thermal_lib_cleanup(void);

/**
 * @brief Get the number of thermal zones
 *
 * This function returns the number of thermal zones available.
 *
 * @param[out] zone_count Pointer to store the zone count
 *
 * @return WOS_THERMAL_LIB_SUCCESS on success
 * @return Error code on failure
 */
enum WosThermalLibReturnCode wos_thermal_lib_get_zone_count(uint8_t* zone_count);

/**
 * @brief Get thermal zone information
 *
 * This function returns information about all thermal zones, including
 * temperature and passive cooling data. It populates the provided thermal_data
 * structure with the current thermal zone information, including zone IDs,
 * names, temperatures, and passive cooling percentages.
 *
 * The function uses the global zone name map that was populated during
 * initialization to identify thermal zones and retrieve their current state.
 *
 * @param[out] thermal_data Pointer to store thermal zone data
 *
 * @return WOS_THERMAL_LIB_SUCCESS on success
 * @return WOS_THERMAL_LIB_ERROR_NOT_INITIALIZED if library is not initialized
 * @return WOS_THERMAL_LIB_ERROR_NULL_POINTER if thermal_data is NULL
 * @return WOS_THERMAL_LIB_ERROR_GET_THERMAL_INFO_FAILED if getting thermal info fails
 * @return WOS_THERMAL_LIB_ERROR_GET_PASSIVE_COOLING_INFO_FAILED if getting passive cooling info fails
 */
enum WosThermalLibReturnCode wos_thermal_lib_get_zone_info(struct WosThermalData* data);

#endif /* WOS_THERMAL_LIB_H */
