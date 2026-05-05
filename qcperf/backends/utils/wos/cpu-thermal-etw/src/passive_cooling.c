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
 * @file passive_cooling.c
 * @brief Implementation of passive cooling monitoring for Windows
 * @author Qualcomm Technologies, Inc.
 *
 * This file implements the passive cooling monitoring functions defined in
 * passive_cooling.h. It provides functionality for monitoring passive cooling
 * metrics on Windows platforms, which indicate thermal throttling levels
 * for various thermal zones.
 *
 * The implementation uses Windows Performance Data Helper (PDH) API to
 * collect passive cooling percentage values. It provides functions for
 * checking availability, initializing the monitoring system, collecting
 * data, and cleaning up resources.
 */

#include <pdh.h>
#include <pdhmsg.h>

#include "passive_cooling.h"

#define MAX_THERMAL_COUNTER_LENGTH 128
#define THERMAL_COMMON_COUNTER_PREFIX "\\Thermal Zone Information("
#define PASSIVE_COOLING_COUNTER_SUFFIX ")\\% Passive Limit"
#define WILDCARD_STRING "*"
#define THERMAL_ZONE_BIOS_NAME_PREFIX "\\_SB.TZ"

volatile bool g_is_passive_cooling_initialized   = false;
bool g_is_passive_cooling_available              = false;
HQUERY g_passive_cooling_query                   = NULL;
HCOUNTER* g_passive_cooling_counter_list         = NULL;
struct ThermalCommonZoneNameMap* g_zone_name_map = NULL;
double* g_zone_passive_cooling                   = NULL;

enum PassiveCoolingReturnCode passive_cooling_is_available() {
    enum PassiveCoolingReturnCode return_code        = RETURN_CODE_PASSIVE_COOLING_NOT_AVAILABLE;
    PDH_STATUS passive_cooling_pdh_status            = ERROR_SUCCESS;
    uint8_t counter_name[MAX_THERMAL_COUNTER_LENGTH] = {0};

    if (true == g_is_passive_cooling_available) {
        // Library is already marked as initialized, counting as success
        return_code = RETURN_CODE_PASSIVE_COOLING_SUCCESS;
    } else {
        snprintf((char*)counter_name, MAX_THERMAL_COUNTER_LENGTH, "%s%s%s", THERMAL_COMMON_COUNTER_PREFIX, WILDCARD_STRING, PASSIVE_COOLING_COUNTER_SUFFIX);

        passive_cooling_pdh_status = PdhValidatePathA(counter_name);
        if (ERROR_SUCCESS != passive_cooling_pdh_status) {
            // PdhValidatePathA failed, passive cooling counters not available
            RETURN_CODE_PASSIVE_COOLING_NOT_AVAILABLE;
        } else {
            return_code = RETURN_CODE_PASSIVE_COOLING_SUCCESS;
        }  // if first PdhValidatePathA success
    }  // if metrics not already initialized

    return return_code;
}

enum PassiveCoolingReturnCode passive_cooling_init(struct ThermalCommonZoneNameMap* map) {
    enum PassiveCoolingReturnCode return_code = RETURN_CODE_PASSIVE_COOLING_SUCCESS;
    PDH_STATUS passive_cooling_pdh_status     = ERROR_SUCCESS;
    uint8_t* passive_cooling_counter_name     = NULL;

    if (true == g_is_passive_cooling_initialized) {
        // Passive cooling monitoring is already initialized
        return_code = RETURN_CODE_PASSIVE_COOLING_ALREADY_INITIALIZED;
    } else if (NULL == map || 0 == map->zone_names_ids_length) {
        // Input map is NULL or has no thermal zones
        return_code = RETURN_CODE_PASSIVE_COOLING_INVALID_ARGUMENT;
    } else {
        g_is_passive_cooling_initialized = true;

        // Open a new PDH query for passive cooling data
        passive_cooling_pdh_status = PdhOpenQuery(NULL, NULL, &g_passive_cooling_query);
        if (ERROR_SUCCESS != passive_cooling_pdh_status || NULL == g_passive_cooling_query) {
            // Failed to open PDH query
            return_code = RETURN_CODE_PASSIVE_COOLING_OPEN_QUERY_FAILED;
        } else {
            // Allocate memory for counter names and data structures
            passive_cooling_counter_name   = (uint8_t*)calloc(MAX_THERMAL_COUNTER_LENGTH, sizeof(uint8_t));
            g_passive_cooling_counter_list = (HCOUNTER*)calloc(map->zone_names_ids_length, sizeof(HCOUNTER));
            g_zone_name_map                = (struct ThermalCommonZoneNameMap*)calloc(1, sizeof(struct ThermalCommonZoneNameMap));
            g_zone_passive_cooling         = (double*)calloc(MAX_NUM_OF_ZONES, sizeof(double));
            if (NULL == passive_cooling_counter_name || NULL == g_passive_cooling_counter_list || NULL == g_zone_name_map || NULL == g_zone_passive_cooling) {
                // Memory allocation failed
                return_code = RETURN_CODE_PASSIVE_COOLING_MEMORY_ALLOCATION_FAILED;
            } else {
                // Copy the zone name map and add counters for each thermal zone
                memcpy_s(g_zone_name_map, sizeof(struct ThermalCommonZoneNameMap), map, sizeof(struct ThermalCommonZoneNameMap));
                for (uint32_t thermal_list_index = 0; RETURN_CODE_PASSIVE_COOLING_SUCCESS == return_code && thermal_list_index < g_zone_name_map->zone_names_ids_length; thermal_list_index++) {
                    // Create counter path for this thermal zone
                    snprintf((char*)passive_cooling_counter_name, MAX_THERMAL_COUNTER_LENGTH, "%s%s%u%s", THERMAL_COMMON_COUNTER_PREFIX, THERMAL_ZONE_BIOS_NAME_PREFIX,
                             g_zone_name_map->zone_ids[thermal_list_index], PASSIVE_COOLING_COUNTER_SUFFIX);

                    // Add counter to the query
                    passive_cooling_pdh_status = PdhAddCounterA(g_passive_cooling_query, (LPCSTR)passive_cooling_counter_name, 0, g_passive_cooling_counter_list + thermal_list_index);
                    if (ERROR_SUCCESS != passive_cooling_pdh_status) {
                        // Failed to add counter for this thermal zone
                        return_code = RETURN_CODE_PASSIVE_COOLING_ADD_COUNTER_FAILED;
                    }
                }  // for each thermal zone
            }
        }
    }

    // Free temporary counter name buffer
    if (NULL != passive_cooling_counter_name) {
        free(passive_cooling_counter_name);
        passive_cooling_counter_name = NULL;
    }

    // Clean up if initialization failed
    if (return_code != RETURN_CODE_PASSIVE_COOLING_SUCCESS) {
        passive_cooling_cleanup();
        g_is_passive_cooling_initialized = false;
    }

    return return_code;
}

enum PassiveCoolingReturnCode passive_cooling_get_info(struct PassiveCoolingInfoQuery* request) {
    enum PassiveCoolingReturnCode return_code  = RETURN_CODE_PASSIVE_COOLING_SUCCESS;
    PDH_STATUS passive_cooling_pdh_status      = ERROR_SUCCESS;
    DWORD thermal_counter_type                 = 0;
    PDH_FMT_COUNTERVALUE thermal_display_value = {0};
    uint32_t thermal_list_index                = 0;

    if (false == g_is_passive_cooling_initialized) {
        // Library not initialized
        return_code = RETURN_CODE_PASSIVE_COOLING_NOT_INITIALIZED;
    } else if (NULL == g_zone_name_map || NULL == request || NULL == request->passive_cooling) {
        // Invalid input parameters
        return_code = RETURN_CODE_PASSIVE_COOLING_NULL_POINTER;
    } else if (0 == request->passive_cooling_length) {
        // Invalid buffer length
        return_code = RETURN_CODE_PASSIVE_COOLING_INVALID_ARGUMENT;
    } else {
        // Collect current passive cooling data
        passive_cooling_pdh_status = PdhCollectQueryData(g_passive_cooling_query);
        if (ERROR_SUCCESS != passive_cooling_pdh_status) {
            // Failed to collect query data
            return_code = RETURN_CODE_PASSIVE_COOLING_COLLECT_QUERY_DATA_FAILED;
        } else {
            // Process each thermal zone's passive cooling value
            while (RETURN_CODE_PASSIVE_COOLING_SUCCESS == return_code && thermal_list_index < g_zone_name_map->zone_names_ids_length && thermal_list_index < request->passive_cooling_length) {
                // Get formatted counter value for this thermal zone
                passive_cooling_pdh_status = PdhGetFormattedCounterValue(g_passive_cooling_counter_list[thermal_list_index], PDH_FMT_DOUBLE, &thermal_counter_type, &thermal_display_value);
                if (ERROR_SUCCESS != passive_cooling_pdh_status) {
                    // Skip this counter if it fails and continue with others
                } else {
                    // Store the passive cooling value
                    request->passive_cooling[thermal_list_index] = thermal_display_value.doubleValue;
                }
                thermal_list_index = thermal_list_index + 1;
            }

            // Adjust output length if needed
            if (thermal_list_index < request->passive_cooling_length) {
                // Adjust the length to match actual number of zones processed
                request->passive_cooling_length = thermal_list_index;
            }
        }
    }

    return return_code;
}

enum PassiveCoolingReturnCode passive_cooling_cleanup() {
    enum PassiveCoolingReturnCode status  = RETURN_CODE_PASSIVE_COOLING_SUCCESS;
    PDH_STATUS passive_cooling_pdh_status = ERROR_SUCCESS;

    if (true == g_is_passive_cooling_initialized) {
        // Remove all counters
        for (uint32_t thermal_list_index = 0; NULL != g_zone_name_map && thermal_list_index < g_zone_name_map->zone_names_ids_length; thermal_list_index++) {
            if (NULL != g_passive_cooling_counter_list && NULL != g_passive_cooling_counter_list[thermal_list_index]) {
                PdhRemoveCounter(g_passive_cooling_counter_list[thermal_list_index]);
                g_passive_cooling_counter_list[thermal_list_index] = NULL;
            }
        }

        // Free counter list
        if (NULL != g_passive_cooling_counter_list) {
            free(g_passive_cooling_counter_list);
            g_passive_cooling_counter_list = NULL;
        }

        // Close PDH query
        if (NULL != g_passive_cooling_query) {
            passive_cooling_pdh_status = PdhCloseQuery(g_passive_cooling_query);
            if (ERROR_SUCCESS != passive_cooling_pdh_status) {
                // Failed to close PDH query
                status = RETURN_CODE_PASSIVE_COOLING_CLOSE_QUERY_FAILED;
            }
        }

        // Free zone name map
        if (NULL != g_zone_name_map) {
            free(g_zone_name_map);
            g_zone_name_map = NULL;
        }

        // Free passive cooling data array
        if (NULL != g_zone_passive_cooling) {
            free(g_zone_passive_cooling);
            g_zone_passive_cooling = NULL;
        }

        g_is_passive_cooling_initialized = false;
    }

    return status;
}
