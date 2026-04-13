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
 * @file power_telemetry.c
 * @brief Implementations of the APIs which will help to capture power telemetry metrics for
 * WOS Power backend for libqcperf
 * @author Sourav Sarkar (sousar@qti.qualcomm.com)
 *
 * This file implements the API which will capture power metrics from different power rails
 * the WOS (Windows on Snapdragon) platform. Power backend will use these power telemetry metrics
 * for the QcPerf library.
 */

#include <initguid.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <stdbool.h>

#include "power_telemetry.h"
#include "wos_power_backend_logger.h"

// Define a macro to prevent QcPepGroupNames from being defined in this file
#define QcPepGroupNames QcPepGroupNames_PowerTelemetry
#undef QcPepGroupNames

#include "wos_ioctl.h"

#define MAX_COUNTER_NAME_LENGTH 128
#define COUNTER_NAME_FORMAT "\\Energy Meter(%s)\\Power"
#define WILDCARD_STRING "*"
#define NULL_TERMINATOR_LENGTH 1

struct RailCounterAttributes {
    HCOUNTER counter;
    enum ePowerTelemetryRail rail;
};

static bool g_rails_initialized                                   = false;
static PDH_STATUS g_pdh_status                                    = 0;
static HQUERY g_h_query                                           = NULL;
static struct RailCounterAttributes* g_rail_counters_requested    = NULL;
static uint8_t g_rail_counters_requested_length                   = 0;
static PDH_FMT_COUNTERVALUE g_display_value                       = {0};
static DWORD g_counter_type                                       = 0;
static const char* g_counter_instances[MAX_POWER_TELEMETRY_RAILS] = {
    "CPU_CLUSTER_0", "CPU_CLUSTER_1", "CPU_CLUSTER_2", "GPU", "PSU_USB", "SYS", "SYSTEM", "USBC_TOTAL", "ROP", "NPU", "MULTIMEDIA", "INFRA", "MEMORY", "SOC",
};
static struct RailsSupported* g_rails_supported = NULL;

#ifdef QCPERF_TARGET_V1
#include "qcpep_power_ioctl.h"
static HANDLE   g_pep_device_handle                            = NULL;
static uint16_t g_v1_rail_count                                = 0;
static uint8_t  g_v1_rail_index_map[MAX_POWER_TELEMETRY_RAILS] = {0};
static uint64_t g_v1_prev_energy[MAX_POWER_TELEMETRY_RAILS]    = {0};
static uint64_t g_v1_prev_timestamp                            = 0;
static uint32_t g_v1_rails_bitmap                              = 0;
#endif

/**
 * @brief Get the supported power rails.
 *
 * @param[out] rails_supported A pointer to a struct RailsSupported to be populated.
 * @param[in] is_persistent A flag indicating whether to perform a persistent query.
 *
 * @return RETURN_CODE_POWER_TELEMETRY_SUCCESS on success, error code otherwise.
 */
enum ePowerTelemetryReturnCode powerTelemetry_railsGetSupported(struct RailsSupported* rails_supported, bool is_persistent) {
    enum ePowerTelemetryReturnCode return_code = RETURN_CODE_POWER_TELEMETRY_NOT_AVAILABLE;
    char counter_name[MAX_COUNTER_NAME_LENGTH] = {0};
    uint8_t counter_name_index                 = 0;
    uint8_t counter_name_iterator              = 0;
    char* available_counters_list              = NULL;
    uint32_t available_counters_list_length    = 0;
    char* available_counters_list_iterator     = NULL;
#ifdef QCPERF_TARGET_V1
    enum IoctlCommonReturnCode ioctl_rc              = RETURN_CODE_IOCTL_COMMON_SUCCESS;
    uint32_t caps_size                               = 0;
    DWORD bytes_returned                             = 0;
    BOOL ok                                          = FALSE;
    uint8_t* caps_buf                                = NULL;
    const struct QcPepCapabilities* caps              = NULL;
    const uint8_t* entry                             = NULL;
    const uint8_t* end                               = NULL;
    uint16_t ri                                      = 0;
    uint16_t name_bytes                              = 0;
    const WCHAR* wname                               = NULL;
    char narrow[64]                                  = {0};
    int wlen                                         = 0;
#endif

    if (NULL != g_rails_supported) {
        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_WARNING, "%s", "Available metrics have already been initialized, reusing");
        return_code = RETURN_CODE_POWER_TELEMETRY_SUCCESS;
    } else {
#ifdef QCPERF_TARGET_V1
        g_rails_supported = (struct RailsSupported*)calloc(1, sizeof(struct RailsSupported));
        if (NULL == g_rails_supported) {
            return_code = RETURN_CODE_POWER_TELEMETRY_CALLOC_FAILED;
        } else {
            ioctl_rc = ioctl_get_device_handle(GUID_QCPEP_POWER_DEVICE_INTERFACE, &g_pep_device_handle);
            if (RETURN_CODE_IOCTL_COMMON_SUCCESS != ioctl_rc) {
                SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR,
                    "ioctl_get_device_handle failed rc=%d GLE=%lu", (int)ioctl_rc, GetLastError());
                return_code = RETURN_CODE_POWER_TELEMETRY_IOCTL_HANDLE_COMMON_FAILED;
            } else {
                ok = DeviceIoControl(g_pep_device_handle,
                                     IOCTL_QCPEP_GET_CAPS_SIZE,
                                     NULL, 0,
                                     &caps_size, (DWORD)sizeof(caps_size),
                                     &bytes_returned, NULL);
                if (FALSE == ok || bytes_returned < (DWORD)sizeof(caps_size)) {
                    SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR,
                        "IOCTL_QCPEP_GET_CAPS_SIZE failed GLE=%lu", GetLastError());
                    return_code = RETURN_CODE_POWER_TELEMETRY_IOCTL_HANDLE_COMMON_FAILED;
                } else if (caps_size > QCPEP_CAPS_BUFFER_MAX) {
                    SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR,
                        "caps_size=%u exceeds QCPEP_CAPS_BUFFER_MAX", caps_size);
                    return_code = RETURN_CODE_POWER_TELEMETRY_IOCTL_HANDLE_COMMON_FAILED;
                } else {
                    caps_buf = (uint8_t*)calloc(caps_size, 1);
                    if (NULL == caps_buf) {
                        return_code = RETURN_CODE_POWER_TELEMETRY_CALLOC_FAILED;
                    } else {
                        ok = DeviceIoControl(g_pep_device_handle,
                                             IOCTL_QCPEP_GET_CAPABILITIES,
                                             NULL, 0,
                                             caps_buf, caps_size,
                                             &bytes_returned, NULL);
                        if (FALSE == ok || bytes_returned < (DWORD)sizeof(struct QcPepCapabilities)) {
                            SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR,
                                "IOCTL_QCPEP_GET_CAPABILITIES failed GLE=%lu", GetLastError());
                            return_code = RETURN_CODE_POWER_TELEMETRY_IOCTL_HANDLE_COMMON_FAILED;
                        } else {
                            caps = (const struct QcPepCapabilities*)caps_buf;
                            g_v1_rail_count = caps->numRails;
                            memset(g_v1_rail_index_map, QCPEP_RAIL_INDEX_UNMATCHED, sizeof(g_v1_rail_index_map));
                            entry = caps_buf + sizeof(struct QcPepCapabilities);
                            end   = caps_buf + bytes_returned;
                            ri    = 0;
                            while (ri < caps->numRails && entry + 2 <= end) {
                                name_bytes = *(const uint16_t*)entry;
                                if (name_bytes < 2u) {
                                    entry += 2;
                                    continue;
                                }
                                wname = (const WCHAR*)(entry + 2);
                                entry += 2u + name_bytes;
                                if (entry > end) {
                                    break;
                                }
                                // Convert UTF-16LE rail name to narrow string
                                memset(narrow, 0, sizeof(narrow));
                                wlen = (int)(name_bytes / 2u) - 1;  // exclude null terminator
                                if (wlen <= 0 || wlen >= (int)sizeof(narrow)) {
                                    ri++;
                                    continue;
                                }
                                for (int c = 0; c < wlen; c++) {
                                    narrow[c] = (char)(wname[c] & 0x7F);
                                }
                                for (uint8_t ei = 0; ei < MAX_POWER_TELEMETRY_RAILS; ei++) {
                                    if (0 == strcmp(narrow, g_counter_instances[ei])) {
                                        g_v1_rail_index_map[ri] = ei;
                                        g_rails_supported->supportedRailsMap[ei] = true;
                                        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_INFO,
                                            "V1 rail[%u] '%s' -> ePowerTelemetryRail %u",
                                            (unsigned)ri, narrow, (unsigned)ei);
                                        return_code = RETURN_CODE_POWER_TELEMETRY_SUCCESS;
                                        break;
                                    }
                                }
                                if (QCPEP_RAIL_INDEX_UNMATCHED == g_v1_rail_index_map[ri]) {
                                    SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_WARNING,
                                        "V1 rail[%u] '%s' has no matching ePowerTelemetryRail",
                                        (unsigned)ri, narrow);
                                }
                                ri++;
                            }
                        }
                        free(caps_buf);
                    }
                }
            }
        }
#else
        g_rails_supported = (struct RailsSupported*)calloc(1, sizeof(struct RailsSupported));
        if (NULL == g_rails_supported) {
            SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s", "Memory allocation failed for available metrics");
            return_code = RETURN_CODE_POWER_TELEMETRY_CALLOC_FAILED;
        } else {
            snprintf(counter_name, MAX_COUNTER_NAME_LENGTH, COUNTER_NAME_FORMAT, WILDCARD_STRING);
            g_pdh_status = PdhExpandWildCardPath(NULL, counter_name, NULL, &available_counters_list_length, 0);
            if (PDH_MORE_DATA != g_pdh_status) {
                SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s%x%s%s", "PdhExpandWildCardPath failed with status 0x", g_pdh_status, " for ", counter_name);
            } else {
                available_counters_list = (char*)calloc(available_counters_list_length, sizeof(char));
                if (NULL == available_counters_list) {
                    SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s", "Memory allocation failed for available counters");
                    return_code = RETURN_CODE_POWER_TELEMETRY_CALLOC_FAILED;
                } else {
                    g_pdh_status = PdhExpandWildCardPath(NULL, counter_name, available_counters_list, &available_counters_list_length, 0);
                    if (ERROR_SUCCESS != g_pdh_status) {
                        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s%x%s%s", "PdhExpandWildCardPath failed with status 0x", g_pdh_status, " for ", counter_name);
                    } else {
                        available_counters_list_iterator = available_counters_list;
                        while ('\0' != *available_counters_list_iterator) {
                            available_counters_list_iterator = (char*)strchr((char*)available_counters_list_iterator + 2, '\\');
                            if (NULL == available_counters_list_iterator) {
                                SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s", "The current counter only has a machine name");
                            } else {
                                SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_DEBUG, "%s%s", "Looking for match to counter: ", available_counters_list_iterator);

                                counter_name_index = 0;
                                while (counter_name_index < MAX_POWER_TELEMETRY_RAILS) {
                                    snprintf(counter_name, MAX_COUNTER_NAME_LENGTH, COUNTER_NAME_FORMAT, (char*)g_counter_instances[counter_name_iterator]);
                                    if (false == g_rails_supported->supportedRailsMap[counter_name_iterator] && 0 == strcmp(available_counters_list_iterator, counter_name)) {
                                        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_DEBUG, "%s%s%s%u%s", "Found a match for counter ", counter_name, ", marking index ", counter_name_iterator,
                                                     " as available.");
                                        g_rails_supported->supportedRailsMap[counter_name_iterator] = true;
                                        return_code                                                 = RETURN_CODE_POWER_TELEMETRY_SUCCESS;
                                    }  // if match

                                    counter_name_index = counter_name_index + 1;
                                    // Use mod to reset iterator within bounds, but keep it cycling list rather than resetting
                                    counter_name_iterator = (counter_name_iterator + 1) % MAX_POWER_TELEMETRY_RAILS;
                                }  // for each possible counter
                                available_counters_list_iterator = available_counters_list_iterator + strlen(available_counters_list_iterator) + NULL_TERMINATOR_LENGTH;
                            }
                        }
                    }
                }
            }
        }
#endif
    }

    if (RETURN_CODE_POWER_TELEMETRY_SUCCESS == return_code && g_rails_supported != rails_supported) {
        if (NULL == rails_supported) {
            SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_WARNING, "%s", "The input is NULL, will not populate on return");
        } else {
            memcpy((void*)rails_supported, (void*)g_rails_supported, sizeof(struct RailsSupported));
        }  // if input is not NULL
    }  // if no errors occurred, input is valid, and input/global pointers are not the same

    if (false == is_persistent && NULL != g_rails_supported) {
        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_DEBUG, "%s", "Call to isAvailable is not meant to be persistent, freeing data");
        free(g_rails_supported);
        g_rails_supported = NULL;
    }

    if (NULL != available_counters_list) {
        free(available_counters_list);
        available_counters_list = NULL;
    }

    return return_code;
}

/**
 * @brief Initialize the power telemetry for a set of rails.
 *
 * @param[in] request A pointer to a RailsInfoRequest structure.
 *
 * @return RETURN_CODE_POWER_TELEMETRY_SUCCESS on success, error code otherwise.
 */
enum ePowerTelemetryReturnCode powerTelemetry_railsInit(struct RailsInfoRequest* request) {
    enum ePowerTelemetryReturnCode status      = RETURN_CODE_POWER_TELEMETRY_UNCAUGHT_FAILURE;
    char counter_name[MAX_COUNTER_NAME_LENGTH] = {0};
    uint8_t current_rail                       = POWER_TELEMETRY_RAIL_CPU_CLUSTER_0;

    if (true == g_rails_initialized) {
        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s", "Rails library is already initialized");
        status = RETURN_CODE_POWER_TELEMETRY_ALREADY_INITIALIZED;
    } else if (NULL == request || 0 == request->railsLength) {
        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s", "Input is NULL or empty");
        status = RETURN_CODE_POWER_TELEMETRY_NULL_POINTER;
    } else {
#ifdef QCPERF_TARGET_V1
        g_rails_initialized = true;
        status              = powerTelemetry_railsGetSupported(NULL, true);
        if (RETURN_CODE_POWER_TELEMETRY_SUCCESS == status) {
            g_v1_rails_bitmap   = 0;
            g_v1_prev_timestamp = 0;
            memset(g_v1_prev_energy, 0, sizeof(g_v1_prev_energy));
            for (uint8_t i = 0; i < request->railsLength; i++) {
                uint8_t rail = request->rails[i];
                if (rail < MAX_POWER_TELEMETRY_RAILS &&
                    g_rails_supported->supportedRailsMap[rail]) {
                    g_v1_rails_bitmap |= (1u << rail);
                }
            }
        }
        if (status != RETURN_CODE_POWER_TELEMETRY_SUCCESS) {
            powerTelemetry_railsDestroy();
            g_rails_initialized = false;
        }
#else
        g_rails_initialized = true;
        status              = powerTelemetry_railsGetSupported(NULL, true);
        if (RETURN_CODE_POWER_TELEMETRY_SUCCESS == status) {
            g_pdh_status = PdhOpenQuery(NULL, 0, &g_h_query);
            if (ERROR_SUCCESS != g_pdh_status || NULL == g_h_query) {
                SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "PdhOpenQuery failed with status 0x%x", g_pdh_status);
                status = RETURN_CODE_POWER_TELEMETRY_OPEN_QUERY_FAILED;
            } else {
                g_rail_counters_requested = (struct RailCounterAttributes*)calloc(request->railsLength, sizeof(struct RailCounterAttributes));
                if (NULL == g_rail_counters_requested) {
                    SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s", "Memory allocation for counter list failed");
                    status = RETURN_CODE_POWER_TELEMETRY_CALLOC_FAILED;
                } else {
                    for (uint8_t request_iterator = 0; request_iterator < request->railsLength; request_iterator++) {
                        current_rail = request->rails[request_iterator];
                        if (MAX_POWER_TELEMETRY_RAILS <= current_rail) {
                            SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_WARNING, "", "Requested rail index=", current_rail, " is not available");
                        } else if (false == g_rails_supported->supportedRailsMap[current_rail]) {
                            SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_WARNING, "%s%u%s%s", "Skipping following counter instance because index ", current_rail,
                                         " is not available: ", g_counter_instances[current_rail]);
                        } else {
                            snprintf(counter_name, MAX_COUNTER_NAME_LENGTH, COUNTER_NAME_FORMAT, (char*)g_counter_instances[current_rail]);
                            g_pdh_status = PdhAddCounterA(g_h_query, counter_name, 0, &g_rail_counters_requested[g_rail_counters_requested_length].counter);
                            if (ERROR_SUCCESS != g_pdh_status) {
                                SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_WARNING, "%s%x%s%s", "PdhAddCounter failed with status 0x", g_pdh_status, " for ", counter_name);
                            } else {
                                g_rail_counters_requested[g_rail_counters_requested_length].rail = current_rail;
                                g_rail_counters_requested_length++;
                                SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_INFO, "%s%s", "PdhAddCounter succeeded for ", counter_name);
                                status = RETURN_CODE_POWER_TELEMETRY_SUCCESS;
                            }
                        }
                    }
                }
            }
        }
        // If anything could've been allocated, cleanup before returning error
        if (status != RETURN_CODE_POWER_TELEMETRY_SUCCESS) {
            powerTelemetry_railsDestroy();
            g_rails_initialized = false;
        }
#endif
    }

    return status;
}

/**
 * @brief Get information for the initialized power rails.
 *
 * @param[out] response A pointer to a RailsInfoResponse structure to be populated.
 *
 * @return RETURN_CODE_POWER_TELEMETRY_SUCCESS on success, error code otherwise.
 */
enum ePowerTelemetryReturnCode powerTelemetry_railsGetInfo(struct RailsInfoResponse* response) {
    enum ePowerTelemetryReturnCode status = RETURN_CODE_POWER_TELEMETRY_SUCCESS;
    uint8_t request_iterator              = 0;
    enum ePowerTelemetryRail current_rail = POWER_TELEMETRY_RAIL_CPU_CLUSTER_0;
#ifdef QCPERF_TARGET_V1
    struct QcPepRailMeasurement meas[MAX_POWER_TELEMETRY_RAILS] = {{0}};
    DWORD bytes_returned                                        = 0;
    BOOL ok                                                     = FALSE;
    uint64_t delta_time                                         = 0;
    uint8_t rail_enum                                           = QCPEP_RAIL_INDEX_UNMATCHED;
    uint64_t delta_energy                                       = 0;
    uint16_t i                                                  = 0;
#endif

    if (false == g_rails_initialized) {
        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s", "Rails library is not yet initialized");
        status = RETURN_CODE_POWER_TELEMETRY_NOT_INITIALIZED;
    } else if (NULL == response) {
        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s", "Input is null");
        status = RETURN_CODE_POWER_TELEMETRY_NULL_POINTER;
#ifdef QCPERF_TARGET_V1
    } else {
        ok = DeviceIoControl(g_pep_device_handle,
                             IOCTL_QCPEP_GET_MEASUREMENTS,
                             NULL, 0,
                             meas, (DWORD)(sizeof(struct QcPepRailMeasurement) * g_v1_rail_count),
                             &bytes_returned, NULL);
        if (FALSE == ok) {
            SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR,
                "IOCTL_QCPEP_GET_MEASUREMENTS failed GLE=%lu", GetLastError());
            status = RETURN_CODE_POWER_TELEMETRY_IOCTL_HANDLE_COMMON_FAILED;
        } else if (0 == g_v1_prev_timestamp) {
            for (i = 0; i < g_v1_rail_count; i++) {
                g_v1_prev_energy[i] = meas[i].energy100nJ;
            }
            g_v1_prev_timestamp            = meas[0].filetimeTick;
            response->railsAndValuesLength = 0;
            status = RETURN_CODE_POWER_TELEMETRY_WARNING_INVALID_DATA;
        } else {
            delta_time = meas[0].filetimeTick - g_v1_prev_timestamp;
            if (0 == delta_time) {
                response->railsAndValuesLength = 0;
                status = RETURN_CODE_POWER_TELEMETRY_WARNING_INVALID_DATA;
            } else {
                // power_W = delta_energy_100nJ / delta_time_100ns (units cancel to Watts)
                response->railsAndValuesLength = 0;
                for (i = 0; i < g_v1_rail_count; i++) {
                    rail_enum = g_v1_rail_index_map[i];
                    if (QCPEP_RAIL_INDEX_UNMATCHED == rail_enum) {
                        continue;
                    }
                    if (0 == (g_v1_rails_bitmap & (1u << rail_enum))) {
                        continue;
                    }
                    delta_energy = meas[i].energy100nJ - g_v1_prev_energy[i];
                    response->rails[response->railsAndValuesLength]  = rail_enum;
                    response->values[response->railsAndValuesLength] = (double)delta_energy / (double)delta_time;
                    SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_DEBUG,
                        "V1 rail[%u] ePowerTelemetryRail=%u power=%.3fW",
                        (unsigned)i, (unsigned)rail_enum,
                        response->values[response->railsAndValuesLength]);
                    response->railsAndValuesLength++;
                }
                for (i = 0; i < g_v1_rail_count; i++) {
                    g_v1_prev_energy[i] = meas[i].energy100nJ;
                }
                g_v1_prev_timestamp = meas[0].filetimeTick;
                if (response->railsAndValuesLength > 0) {
                    status = RETURN_CODE_POWER_TELEMETRY_SUCCESS;
                } else {
                    status = RETURN_CODE_POWER_TELEMETRY_WARNING_INVALID_DATA;
                }
            }
        }
    }
#else
    } else if (NULL == g_rail_counters_requested || 0 == g_rail_counters_requested_length) {
        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s", "Request is empty");
        status = RETURN_CODE_POWER_TELEMETRY_INVALID_INPUT;
    } else {
        if (NULL == g_h_query) {
            SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_DEBUG, "%s", "No PDH data marked as requested");
        } else {
            g_pdh_status = PdhCollectQueryData(g_h_query);
            if (g_pdh_status != ERROR_SUCCESS) {
                SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s%x", "PdhCollectQueryData failed with status 0x", g_pdh_status);
                status = RETURN_CODE_POWER_TELEMETRY_COLLECT_QUERY_FAILED;
            } else {
                status                         = RETURN_CODE_POWER_TELEMETRY_WARNING_INVALID_DATA;
                response->railsAndValuesLength = 0;
                while (request_iterator < g_rail_counters_requested_length) {
                    current_rail = g_rail_counters_requested[request_iterator].rail;
                    SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_DEBUG, "%s%s", "Getting formatted counter value for counter=", (char*)g_counter_instances[current_rail]);
                    g_pdh_status = PdhGetFormattedCounterValue(g_rail_counters_requested[request_iterator].counter, PDH_FMT_DOUBLE, &g_counter_type, &g_display_value);
                    if (g_pdh_status != ERROR_SUCCESS && POWER_TELEMETRY_RAIL_SYS == current_rail) {
                        g_pdh_status = PdhGetFormattedCounterValue(g_rail_counters_requested[POWER_TELEMETRY_RAIL_SYSTEM].counter, PDH_FMT_DOUBLE, &g_counter_type, &g_display_value);
                    }
                    if (g_pdh_status != ERROR_SUCCESS) {
                        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_WARNING, "%s%s%s%x%s", "PdhGetFormattedCounterValue failed, instance=", g_counter_instances[current_rail], " status=0x", g_pdh_status,
                                     ". Maybe this is first call");
                    } else {
                        SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_DEBUG, "%s%s%s%f", "Counter=", g_counter_instances[current_rail], " Value=", g_display_value.doubleValue);
                        response->rails[response->railsAndValuesLength]  = current_rail;
                        response->values[response->railsAndValuesLength] = g_display_value.doubleValue;
                        status                                           = RETURN_CODE_POWER_TELEMETRY_SUCCESS;
                        response->railsAndValuesLength++;
                    }

                    request_iterator++;
                }
            }
        }
    }
#endif
    return status;
}

/**
 * @brief Destroy the power telemetry instance.
 *
 * @return RETURN_CODE_POWER_TELEMETRY_SUCCESS on success, error code otherwise.
 */
enum ePowerTelemetryReturnCode powerTelemetry_railsDestroy() {
    enum ePowerTelemetryReturnCode status = RETURN_CODE_POWER_TELEMETRY_SUCCESS;
    if (true == g_rails_initialized) {
#ifdef QCPERF_TARGET_V1
        if (NULL != g_pep_device_handle) {
            ioctl_free_device_handle(&g_pep_device_handle);
            g_pep_device_handle = NULL;
        }
        g_v1_rail_count     = 0;
        g_v1_rails_bitmap   = 0;
        g_v1_prev_timestamp = 0;
        memset(g_v1_rail_index_map, QCPEP_RAIL_INDEX_UNMATCHED, sizeof(g_v1_rail_index_map));
        memset(g_v1_prev_energy, 0, sizeof(g_v1_prev_energy));
        g_rails_initialized = false;
        if (NULL != g_rails_supported) {
            free(g_rails_supported);
            g_rails_supported = NULL;
        }
#else
        if (NULL != g_h_query) {
            // Closes all counters contained in the specified query,
            // closes all handles related to the query, and frees all memory associated with the query.
            g_pdh_status = PdhCloseQuery(g_h_query);
            if (ERROR_SUCCESS != g_pdh_status) {
                SEND_MESSAGE(QC_PERF_MESSAGE_LEVEL_ERROR, "%s%x", "PdhCloseQuery failed with status 0x", g_pdh_status);
                status = RETURN_CODE_POWER_TELEMETRY_CLOSE_QUERY_FAILED;
            }
        }
        if (NULL != g_rail_counters_requested) {
            for (uint8_t i = 0; i < g_rail_counters_requested_length; i++) {
                PdhRemoveCounter(g_rail_counters_requested->counter);
            }
            free(g_rail_counters_requested);
            g_rail_counters_requested        = NULL;
            g_rail_counters_requested_length = 0;
        }

        if (NULL != g_rails_supported) {
            free(g_rails_supported);
            g_rails_supported = NULL;
        }
        g_rails_initialized = false;
#endif
    }
    return status;
}
