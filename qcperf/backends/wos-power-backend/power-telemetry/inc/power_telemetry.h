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
    OTHERWISE) ARISING IN ANY WAY OUT of THE USE of THIS SOFTWARE, EVEN
    IF ADVISED of THE POSSIBILITY of SUCH DAMAGE.
*/
/**
 * @file power_telemetry.h
 * @brief Power Telemetry API for collecting power data.
 * @author Sourav Sarkar (sousar@qti.qualcomm.com)
 *
 * This header defines the interface for the Power Telemetry library, which
 * provides functions to initialize, query, and destroy power rail information.
 */

#ifndef POWER_TELEMETRY_H
#define POWER_TELEMETRY_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @enum ePowerTelemetryReturnCode
 * @brief Return codes for the Power Telemetry library.
 */
enum ePowerTelemetryReturnCode {
    RETURN_CODE_POWER_TELEMETRY_SUCCESS = 0,
    RETURN_CODE_POWER_TELEMETRY_NOT_INITIALIZED,
    RETURN_CODE_POWER_TELEMETRY_ALREADY_INITIALIZED,
    RETURN_CODE_POWER_TELEMETRY_INVALID_INPUT,
    RETURN_CODE_POWER_TELEMETRY_NULL_POINTER,
    RETURN_CODE_POWER_TELEMETRY_CALLOC_FAILED,
    RETURN_CODE_POWER_TELEMETRY_OPEN_QUERY_FAILED,
    RETURN_CODE_POWER_TELEMETRY_CLOSE_QUERY_FAILED,
    RETURN_CODE_POWER_TELEMETRY_COLLECT_QUERY_FAILED,
    RETURN_CODE_POWER_TELEMETRY_REMOVE_COUNTER_FAILED,
    RETURN_CODE_POWER_TELEMETRY_WARNING_INVALID_DATA,
    RETURN_CODE_POWER_TELEMETRY_NOT_AVAILABLE,
    RETURN_CODE_POWER_TELEMETRY_INTERNAL_LIBRARY_FAILED,
    RETURN_CODE_POWER_TELEMETRY_BCL_GET_CAPABILITIES_FAILED,
    RETURN_CODE_POWER_TELEMETRY_UNCAUGHT_FAILURE,
    RETURN_CODE_POWER_TELEMETRY_IOCTL_HANDLE_COMMON_FAILED,
};

/**
 * @enum ePowerTelemetryRail
 * @brief Identifiers for different power rails.
 */
enum ePowerTelemetryRail {
    POWER_TELEMETRY_RAIL_CPU_CLUSTER_0,
    POWER_TELEMETRY_RAIL_CPU_CLUSTER_1,
    POWER_TELEMETRY_RAIL_CPU_CLUSTER_2,
    POWER_TELEMETRY_RAIL_GPU,
    POWER_TELEMETRY_RAIL_PSU_USB,
    POWER_TELEMETRY_RAIL_SYS,
    POWER_TELEMETRY_RAIL_SYSTEM,
    POWER_TELEMETRY_RAIL_USBC_TOTAL,
    POWER_TELEMETRY_RAIL_ROP,
    POWER_TELEMETRY_RAIL_NSP,
    POWER_TELEMETRY_RAIL_MULTIMEDIA,
    POWER_TELEMETRY_RAIL_INFRA,
    POWER_TELEMETRY_RAIL_DRAM_DDR,
    POWER_TELEMETRY_RAIL_SOC,
    MAX_POWER_TELEMETRY_RAILS,
};


/**
 * @struct RailsSupported
 * @brief Structure to hold the supported status of power rails.
 */
struct RailsSupported {
    bool supportedRailsMap[MAX_POWER_TELEMETRY_RAILS];
};

/**
 * @struct RailsInfoRequest
 * @brief Structure to request information about specific power rails.
 */
struct RailsInfoRequest {
    enum ePowerTelemetryRail rails[MAX_POWER_TELEMETRY_RAILS];
    uint8_t railsLength;
};

/**
 * @struct RailsInfoResponse
 * @brief Structure to hold the response for a power rail information request.
 */
struct RailsInfoResponse {
    enum ePowerTelemetryRail rails[MAX_POWER_TELEMETRY_RAILS];
    double values[MAX_POWER_TELEMETRY_RAILS];
    uint8_t railsAndValuesLength;
};

/**
 * @brief Get the supported power rails.
 * @param[out] railsSupported A pointer to a RailsSupported structure to be populated.
 * @param[in] isPersistent A flag indicating whether to perform a persistent query.
 * @return RETURN_CODE_POWER_TELEMETRY_SUCCESS on success.
 */
enum ePowerTelemetryReturnCode powerTelemetry_railsGetSupported(struct RailsSupported *railsSupported, bool isPersistent);

/**
 * @brief Initialize the power telemetry for a set of rails.
 * @param[in] request A pointer to a RailsInfoRequest structure.
 * @return RETURN_CODE_POWER_TELEMETRY_SUCCESS on success.
 */
enum ePowerTelemetryReturnCode powerTelemetry_railsInit(struct RailsInfoRequest *request);

/**
 * @brief Get information for the initialized power rails.
 * @param[out] response A pointer to a RailsInfoResponse structure to be populated.
 * @return RETURN_CODE_POWER_TELEMETRY_SUCCESS on success.
 */
enum ePowerTelemetryReturnCode powerTelemetry_railsGetInfo(struct RailsInfoResponse *response);

/**
 * @brief Destroy the power telemetry instance.
 * @return RETURN_CODE_POWER_TELEMETRY_SUCCESS on success.
 */
enum ePowerTelemetryReturnCode powerTelemetry_railsDestroy();

#endif  // POWER_TELEMETRY_H
