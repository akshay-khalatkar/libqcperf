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
 * @file passive_cooling.h
 * @brief Passive cooling monitoring library for Windows
 * @author Qualcomm Technologies, Inc.
 *
 * This file provides functions and structures for monitoring passive cooling
 * metrics on Windows platforms. Passive cooling refers to thermal management
 * techniques that reduce system performance to manage heat, such as CPU throttling.
 */

#ifndef PASSIVE_COOLING_H
#define PASSIVE_COOLING_H

#include <stdbool.h>
#include "thermal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return codes for passive cooling operations
 */
enum PassiveCoolingReturnCode {
    RETURN_CODE_PASSIVE_COOLING_SUCCESS,                   /**< Operation successful */
    RETURN_CODE_PASSIVE_COOLING_OPEN_QUERY_FAILED,         /**< Failed to open performance query */
    RETURN_CODE_PASSIVE_COOLING_MEMORY_ALLOCATION_FAILED,  /**< Memory allocation failed */
    RETURN_CODE_PASSIVE_COOLING_ADD_COUNTER_FAILED,        /**< Failed to add performance counter */
    RETURN_CODE_PASSIVE_COOLING_COLLECT_QUERY_DATA_FAILED, /**< Failed to collect query data */
    RETURN_CODE_PASSIVE_COOLING_NATIVE_FUNCTION_FAILED,    /**< Native function call failed */
    RETURN_CODE_PASSIVE_COOLING_ALREADY_INITIALIZED,       /**< Library already initialized */
    RETURN_CODE_PASSIVE_COOLING_NOT_INITIALIZED,           /**< Library not initialized */
    RETURN_CODE_PASSIVE_COOLING_NULL_POINTER,              /**< Null pointer provided */
    RETURN_CODE_PASSIVE_COOLING_INVALID_ARGUMENT,          /**< Invalid argument */
    RETURN_CODE_PASSIVE_COOLING_CLOSE_QUERY_FAILED,        /**< Failed to close performance query */
    RETURN_CODE_PASSIVE_COOLING_NOT_AVAILABLE,             /**< Passive cooling metrics not available */
};

/**
 * @brief Structure for passive cooling information query results
 */
struct PassiveCoolingInfoQuery {
    double* passive_cooling;       /**< Array of passive cooling values (0-100%) */
    uint8_t passive_cooling_length; /**< Length of passive_cooling array */
};

/**
 * @brief Check if passive cooling metrics are available
 *
 * Updates library metric availability status.
 *
 * @return RETURN_CODE_PASSIVE_COOLING_SUCCESS on success,
 *         RETURN_CODE_PASSIVE_COOLING_NOT_AVAILABLE if not available
 */
DllExport enum PassiveCoolingReturnCode passive_cooling_is_available();

/**
 * @brief Initialize passive cooling monitoring
 *
 * @param map Thermal zone name mapping
 * @return enum PassiveCoolingReturnCode Return code indicating success or failure
 */
DllExport enum PassiveCoolingReturnCode passive_cooling_init(struct ThermalCommonZoneNameMap* map);

/**
 * @brief Get passive cooling information
 *
 * @param request Query structure to fill with passive cooling information
 * @return enum PassiveCoolingReturnCode Return code indicating success or failure
 */
DllExport enum PassiveCoolingReturnCode passive_cooling_get_info(struct PassiveCoolingInfoQuery* request);

/**
 * @brief Clean up passive cooling monitoring resources
 *
 * @return enum PassiveCoolingReturnCode Return code indicating success or failure
 */
DllExport enum PassiveCoolingReturnCode passive_cooling_cleanup();

#ifdef __cplusplus
}
#endif

#endif  // PASSIVE_COOLING_H
