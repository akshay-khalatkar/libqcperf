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
