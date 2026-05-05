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
 * @file thermal_common.cpp
 * @brief Implementation of common thermal monitoring utilities using ETW
 * @author Qualcomm Technologies, Inc.
 *
 * This file implements the common thermal monitoring functions defined in
 * thermal_common.h. It provides shared functionality for thermal monitoring
 * on Windows platforms using Event Tracing for Windows (ETW).
 *
 * The implementation includes utilities for managing ETW sessions, parsing
 * thermal zone information, and coordinating data collection between
 * different thermal monitoring components. It handles the complexities of
 * ETW session management, including stopping and restarting sessions when
 * conflicts occur.
 */

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <evntrace.h>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <cctype>
#include "thermal_common.h"

#ifdef __cplusplus
#include "krabs.hpp"
#endif

extern "C" {
#include "qmutex.h"
#include "qsleep.h"
}

#define THERMAL_ZONE_RUNDOWN_EVENT_ID 12
#define TIMEOUT_WAIT_FOR_EXCEPTION_MS 500
#define THERMAL_THREAD_NAME "thread-thermal-zone-parser"
#define THERMAL_FUNCTION_MUTEX_NAME "mutex-thermal-function-guard"
#define THERMAL_CALLBACK_MUTEX_NAME "mutex-thermal-callback-parser"
#define THERMAL_EXCEPTION_CV_NAME "cv-thermal-exception-caught"
#define THERMAL_STOPPED_SESSIONS_MUTEX_NAME "mutex-thermal-stopped-sessions"
#define TZ_ID_PREFIX_W L"TZ"
#define TZ_ID_PREFIX_A "TZ"
#define DECIMAL_BASE 10
#define ANY_FLAG_ACPI_PROVIDER 0x2
#define MAX_NUM_WINDOWS_SESSIONS 64
#define MAX_WINDOWS_SESSION_NAME_LENGTH 1024
#define PREFIX_FOR_SESSIONS_TO_STOP "qc"
#define PREFIX_FOR_SESSIONS_TO_STOP_LENGTH 2
#define MAX_SESSION_NAME_LEN 1024
#define MAX_LOGFILE_PATH_LEN 1024
#define MAX_SESSION_NAME_TO_STOP_LENGTH 64

struct EtwSessionConfig {
    uint8_t session_name[MAX_SESSION_NAME_TO_STOP_LENGTH];  // Windows allows up to 1024, but realistically we won't be stopping sessions with long names
    uint8_t session_name_length;
    PEVENT_TRACE_PROPERTIES properties;
    uint32_t properties_size;
};

// Microsoft-Windows-Kernel-Acpi GUID: C514638F-7723-485B-BCFC-96565D735D4A
krabs::provider<> g_thermal_common_acpi_provider(krabs::guid(ACPI_PROVIDER_GUID));
// Microsoft-Windows-Thermal-Polling GUID: e8a7c168-81ee-465c-8e8e-d39a2ac1ca41
krabs::provider<> g_thermal_common_polling_provider(krabs::guid(POLLING_PROVIDER_GUID));
// Trace for Thermal events
krabs::user_trace g_thermal_commonTrace(L"Qualcomm Profiler Thermal Common Trace");

MutexInfo g_function_mutex_info                   = {0};
MutexInfo g_callback_mutex_info                   = {0};
CvInfo g_exception_cv_info                        = {0};
struct ThermalCommonZoneNameMap *g_zone_name_map  = NULL;
volatile uint8_t g_instance_count                 = 0;
static const size_t g_event_trace_properties_size = sizeof(EVENT_TRACE_PROPERTIES) + (MAX_SESSION_NAME_LEN * sizeof(char)) + (MAX_LOGFILE_PATH_LEN * sizeof(char));
struct List *g_stopped_etw_sessions               = NULL;

/**
 * @brief Get active ETW sessions
 *
 * Retrieves a list of active ETW sessions on the system.
 *
 * @param sessions Vector to store the names of active sessions
 * @return enum ThermalCommonReturnCode Return code indicating success or failure
 */
enum ThermalCommonReturnCode thermal_common_get_active_etw_sessions(std::vector<std::string> &sessions) {
    enum ThermalCommonReturnCode return_code           = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    uint64_t windows_return_code                       = ERROR_SUCCESS;
    std::vector<EVENT_TRACE_PROPERTIES *> event_traces = {};
    std::vector<BYTE> event_traces_buffer              = {};
    ULONG event_traces_length                          = MAX_NUM_WINDOWS_SESSIONS;
    LPCSTR session_name                                = NULL;
    bool should_retry_size_fetch                       = false;

    try {
        sessions.clear();

        do {
            event_traces.resize(event_traces_length);
            event_traces_buffer.resize(g_event_trace_properties_size * event_traces_length);

            for (size_t i = 0; i != event_traces.size(); i++) {
                event_traces[i]                    = (EVENT_TRACE_PROPERTIES *)&event_traces_buffer[i * g_event_trace_properties_size];
                event_traces[i]->Wnode.BufferSize  = g_event_trace_properties_size;
                event_traces[i]->LoggerNameOffset  = sizeof(EVENT_TRACE_PROPERTIES);
                event_traces[i]->LogFileNameOffset = sizeof(EVENT_TRACE_PROPERTIES) + (MAX_SESSION_NAME_LEN * sizeof(char));
            }

            windows_return_code = QueryAllTracesA(&event_traces[0], event_traces_length, &event_traces_length);
            if (windows_return_code != ERROR_SUCCESS && ERROR_MORE_DATA != windows_return_code) {
                // QueryAllTraces failed
                return_code             = RETURN_CODE_THERMAL_COMMON_QUERY_ALL_TRACES_FAILED;
                should_retry_size_fetch = false;
            } else if (UINT8_MAX < event_traces_length) {
                // Session count is greater than maximum allowed
                return_code             = RETURN_CODE_THERMAL_COMMON_VALUE_WOULD_OVERFLOW;
                should_retry_size_fetch = false;
            } else if (0 == event_traces_length) {
                // No traces found
                return_code             = RETURN_CODE_THERMAL_COMMON_NO_TRACES_FOUND;
                should_retry_size_fetch = false;
            } else if (ERROR_MORE_DATA == windows_return_code) {
                // Size changed, checking size again
                should_retry_size_fetch = true;
            } else {
                should_retry_size_fetch = false;
                return_code             = RETURN_CODE_THERMAL_COMMON_SUCCESS;

                // Process active sessions found
                for (uint8_t i = 0; i < (uint8_t)event_traces_length; i++) {
                    if (event_traces[i]->LoggerNameOffset > 0) {
                        session_name = reinterpret_cast<LPCSTR>(reinterpret_cast<BYTE *>(event_traces[i]) + event_traces[i]->LoggerNameOffset);
                        if (NULL != session_name && lstrlenA(session_name) > 0) {
                            sessions.push_back(std::string(session_name));
                        }
                    }
                }
            }
        } while (true == should_retry_size_fetch);
    } catch (const std::exception &ex) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    } catch (...) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    }

    return return_code;
}

/**
 * @brief Stop an ETW session
 *
 * Stops the specified ETW session and stores its configuration for later restart.
 *
 * @param session_name Name of the ETW session to stop
 * @param stopped_sessions List to store stopped session information
 * @return enum ThermalCommonReturnCode Return code indicating success or failure
 */
enum ThermalCommonReturnCode thermal_common_stop_etw_session(const std::string &session_name, struct List *stopped_sessions) {
    enum ThermalCommonReturnCode return_code = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum QListReturnCode list_return_code    = RETURN_CODE_LIST_SUCCESS;
    uint64_t windows_return_code             = 0;
    ULONG buffer_size                        = 0;
    PEVENT_TRACE_PROPERTIES properties       = NULL;
    BYTE *properties_buffer                  = NULL;
    struct EtwSessionConfig *session_config  = NULL;

    try {
        // Allocate buffer for ETW session properties
        buffer_size       = sizeof(EVENT_TRACE_PROPERTIES) + MAX_WINDOWS_SESSION_NAME_LENGTH + MAX_LOGFILE_PATH_LEN;
        properties_buffer = (BYTE *)calloc(buffer_size, sizeof(BYTE));
        if (NULL == properties_buffer) {
            // Memory allocation failed
            return_code = RETURN_CODE_THERMAL_COMMON_MEMORY_ALLOCATION_FAILED;
        } else {
            // Initialize properties structure
            properties                    = reinterpret_cast<PEVENT_TRACE_PROPERTIES>(properties_buffer);
            properties->Wnode.BufferSize  = buffer_size;
            properties->LoggerNameOffset  = sizeof(EVENT_TRACE_PROPERTIES);
            properties->LogFileNameOffset = sizeof(EVENT_TRACE_PROPERTIES) + MAX_LOGFILE_PATH_LEN;

            // Query session configuration
            windows_return_code = ControlTraceA(0, session_name.c_str(), properties, EVENT_TRACE_CONTROL_QUERY);
            if (windows_return_code != ERROR_SUCCESS) {
                // Failed to query ETW session configuration, use default settings
                properties->Wnode.BufferSize = sizeof(EVENT_TRACE_PROPERTIES);
                properties->Wnode.Flags      = WNODE_FLAG_TRACED_GUID;
                properties->LogFileMode      = EVENT_TRACE_REAL_TIME_MODE;
            }

            // Stop the ETW session
            windows_return_code = ControlTraceA(0, session_name.c_str(), properties, EVENT_TRACE_CONTROL_STOP);
            if (windows_return_code != ERROR_SUCCESS) {
                // Failed to stop ETW session
                return_code = RETURN_CODE_THERMAL_COMMON_EVENT_CONTROL_TRACE_STOP_FAILED;
            } else {
                // Successfully stopped ETW session
                if (NULL == stopped_sessions) {
                    // No need to store session info
                    return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;
                } else {
                    // Store session configuration for later restart
                    session_config = (struct EtwSessionConfig *)calloc(1, sizeof(struct EtwSessionConfig));
                    if (NULL == session_config) {
                        // Memory allocation failed
                        return_code = RETURN_CODE_THERMAL_COMMON_MEMORY_ALLOCATION_FAILED;
                    } else {
                        // Save session name and properties
                        session_config->session_name_length = snprintf((char *)session_config->session_name, MAX_SESSION_NAME_TO_STOP_LENGTH, "%s", session_name.c_str());
                        session_config->properties_size     = buffer_size;
                        session_config->properties          = reinterpret_cast<PEVENT_TRACE_PROPERTIES>(properties_buffer);

                        // Add to list of stopped sessions
                        list_return_code = list_push_back(g_stopped_etw_sessions, session_config);
                        if (RETURN_CODE_LIST_SUCCESS != list_return_code) {
                            return_code = RETURN_CODE_THERMAL_COMMON_NATIVE_FUNCTION_FAILED;
                        } else {
                            return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;
                        }
                    }
                }
            }
        }
    } catch (const std::exception &ex) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    } catch (...) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    }

    // Clean up on failure
    if (RETURN_CODE_THERMAL_COMMON_SUCCESS != return_code) {
        if (NULL != properties_buffer) {
            free(properties_buffer);
            properties_buffer = NULL;
        }

        if (NULL != session_config) {
            free(session_config);
            session_config = NULL;
        }
    }

    return return_code;
}

/**
 * @brief Stop the first eligible ETW session
 *
 * Finds and stops the first ETW session with a name starting with "qc".
 *
 * @param stopped_sessions List to store stopped session information
 * @return enum ThermalCommonReturnCode Return code indicating success or failure
 */
enum ThermalCommonReturnCode thermal_common_stop_first_eligible_session(struct List *stopped_sessions) {
    enum ThermalCommonReturnCode return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;
    std::vector<std::string> active_sessions = {};
    bool found_qc_session                    = false;
    struct EtwSessionConfig config           = {0};
    std::string original_name                = "";

    try {
        // Get list of active ETW sessions
        return_code = thermal_common_get_active_etw_sessions(active_sessions);
        if (RETURN_CODE_THERMAL_COMMON_SUCCESS == return_code) {
            if (true == active_sessions.empty()) {
                // No active sessions found
                return_code = RETURN_CODE_THERMAL_COMMON_NO_TRACES_FOUND;
            } else {
                // Look for sessions starting with "qc"
                for (std::string &session : active_sessions) {
                    original_name = session;
                    std::transform(session.begin(), session.end(), session.begin(), [](unsigned char c) { return std::tolower(c); });

                    if (session.length() >= PREFIX_FOR_SESSIONS_TO_STOP_LENGTH && session.substr(0, PREFIX_FOR_SESSIONS_TO_STOP_LENGTH) == PREFIX_FOR_SESSIONS_TO_STOP) {
                        // Found eligible session, try to stop it
                        return_code = thermal_common_stop_etw_session(original_name, stopped_sessions);
                        if (RETURN_CODE_THERMAL_COMMON_SUCCESS == return_code) {
                            found_qc_session = true;
                            break;
                        }
                    }
                }

                if (false == found_qc_session) {
                    // No eligible sessions found
                    return_code = RETURN_CODE_THERMAL_COMMON_NO_QC_TRACES_FOUND;
                }
            }
        }
    } catch (const std::exception &ex) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    } catch (...) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    }

    return return_code;
}

/**
 * @brief Restart previously stopped ETW sessions
 *
 * Restarts all ETW sessions that were previously stopped and stored in the list.
 *
 * @param stopped_sessions List containing stopped session information
 * @return enum ThermalCommonReturnCode Return code indicating success or failure
 */
enum ThermalCommonReturnCode thermal_common_restart_stopped_sessions(struct ThermalCommonSessions *stopped_sessions) {
    enum ThermalCommonReturnCode return_code       = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum ThermalCommonReturnCode inner_return_code = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum QListReturnCode list_return_code          = RETURN_CODE_LIST_SUCCESS;
    uint64_t windows_return_code                   = 0;
    struct EtwSessionConfig *session_entry         = NULL;
    TRACEHANDLE session_handle                     = 0;

    try {
        // Validate input
        if (NULL == stopped_sessions || NULL == stopped_sessions->qlist) {
            return_code = RETURN_CODE_THERMAL_COMMON_NULL_POINTER;
        } else {
            // Get first session from list
            list_return_code = list_pop_front((struct List *)stopped_sessions->qlist, (void **)&session_entry);
            if (RETURN_CODE_LIST_SUCCESS != list_return_code) {
                // List is empty or error occurred
            } else {
                return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;

                // Process all sessions in the list
                do {
                    if (NULL == session_entry || NULL == session_entry->session_name || NULL == session_entry->properties) {
                        // Invalid session entry
                        return_code = RETURN_CODE_THERMAL_COMMON_NULL_POINTER;
                    } else if (0 == session_entry->session_name_length) {
                        // Empty session name
                        return_code = RETURN_CODE_THERMAL_COMMON_INVALID_INPUT;
                    } else {
                        // Restart the session
                        windows_return_code = StartTraceA(&session_handle, (char *)session_entry->session_name, session_entry->properties);
                        if (ERROR_SUCCESS != windows_return_code) {
                            // Failed to restart session - continue with others
                        }
                    }

                    // Clean up this session entry
                    if (NULL != session_entry) {
                        if (NULL != session_entry->properties) {
                            free(session_entry->properties);
                            session_entry->properties = NULL;
                        }
                        free(session_entry);
                        session_entry = NULL;
                    }

                    // Get next session from list
                    list_return_code = list_pop_front((struct List *)stopped_sessions->qlist, (void **)&session_entry);
                } while (RETURN_CODE_LIST_SUCCESS == list_return_code);
            }
        }
    } catch (const std::exception &ex) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    } catch (...) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    }

    return return_code;
}

/**
 * @brief Parse thermal zone ID from BIOS name
 *
 * Extracts the numeric ID from a thermal zone BIOS name string.
 *
 * @param bios_name Thermal zone BIOS name string
 * @param id Pointer to store the extracted ID
 * @return enum ThermalCommonReturnCode Return code indicating success or failure
 */
enum ThermalCommonReturnCode thermal_common_parse_id_from_bios_name(struct ThermalCommonString *bios_name, uint8_t *id) {
    enum ThermalCommonReturnCode return_code = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    const char *thermal_zone_id_pointer      = NULL;
    uint64_t long_id                         = 0;

    try {
        // Validate input parameters
        if (NULL == bios_name || NULL == id) {
            return_code = RETURN_CODE_THERMAL_COMMON_NULL_POINTER;
        } else if (0 == bios_name->string_length) {
            return_code = RETURN_CODE_THERMAL_COMMON_INVALID_INPUT;
        } else {
            // Find the TZ prefix in the BIOS name
            thermal_zone_id_pointer = strstr((char *)bios_name->string, TZ_ID_PREFIX_A);
            if (NULL == thermal_zone_id_pointer) {
                // TZ prefix not found
            } else {
                // Extract the numeric ID after the prefix
                long_id = (uint8_t)strtoul(thermal_zone_id_pointer + strlen(TZ_ID_PREFIX_A), NULL, DECIMAL_BASE);
                if (UINT8_MAX < long_id || MAX_THERMAL_ZONE_ID < long_id) {
                    // ID out of valid range
                    return_code = RETURN_CODE_THERMAL_COMMON_ZONE_ID_OUT_OF_RANGE;
                } else {
                    // Store the parsed ID
                    *id         = static_cast<uint8_t>(long_id);
                    return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;
                }
            }
        }
    } catch (const std::exception &ex) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    } catch (...) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    }

    return return_code;
}

/**
 * @brief Start ETW listening thread
 *
 * Starts a thread to listen for ETW events, handling any conflicts with other ETW sessions.
 *
 * @param request Thread start request parameters
 * @param stopped_sessions List to store any sessions that need to be stopped
 * @return enum ThermalCommonReturnCode Return code indicating success or failure
 */
enum ThermalCommonReturnCode thermal_common_start_etw_listening_thread(struct ThermalCommonStartRequest *request, struct ThermalCommonSessions *stopped_sessions) {
    enum ThermalCommonReturnCode return_code         = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum ThermalCommonReturnCode ignored_return_code = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum QThreadReturnCode thread_return_code        = RET_QTHREAD_CREATE_SUCCESS;
    enum QCvReturnCode cv_return_code                = RETURN_CODE_CV_CREATE_SUCCESS;
    bool should_retry                                = false;
    struct QThreadInfo thread_info                   = {0};

    // Validate input parameters
    if (NULL == request || NULL == stopped_sessions || NULL == request->callback_condition_variable || NULL == request->thread_attributes || NULL == request->callback_function) {
        return_code = RETURN_CODE_THERMAL_COMMON_NULL_POINTER;
    } else {
        do {
            // Enable ETW providers
            g_thermal_common_acpi_provider.enable_rundown_events();
            ENABLE_THERMAL_PROVIDERS(reinterpret_cast<krabs::c_provider_callback>(request->callback_function), g_thermal_commonTrace, g_thermal_common_acpi_provider,
                                     g_thermal_common_polling_provider);

            // Create thread to start ETW trace
            thread_return_code = thread_create(request->thread_attributes, &thread_info);
            if (RET_QTHREAD_CREATE_SUCCESS != thread_return_code) {
                return_code = RETURN_CODE_THERMAL_COMMON_NATIVE_FUNCTION_FAILED;
            } else {
                // Wait for thread to start or throw exception
                cv_return_code = cv_wait_for((struct CvInfo *)request->callback_condition_variable, request->timeout_ms);
                if (RETURN_CODE_CV_WAIT_COMPLETED == cv_return_code) {
                    // Exception caught, need to retry
                    should_retry = true;
                } else if (RETURN_CODE_CV_WAIT_TIMEOUT == cv_return_code) {
                    // Timeout without exception means success
                    return_code  = RETURN_CODE_THERMAL_COMMON_SUCCESS;
                    should_retry = false;
                } else {
                    // Error waiting for condition variable
                    return_code  = RETURN_CODE_THERMAL_COMMON_NATIVE_FUNCTION_FAILED;
                    should_retry = false;
                }

                // Stop the trace and clean up thread
                g_thermal_commonTrace.stop();
                thread_return_code = thread_join(&thread_info);
                if (RET_QTHREAD_JOIN_SUCCESS == thread_return_code) {
                    thread_destroy(&thread_info);
                }

                // If exception occurred, try to stop a conflicting ETW session
                if (true == should_retry) {
                    ignored_return_code = thermal_common_stop_first_eligible_session((struct List *)stopped_sessions->qlist);
                    if (RETURN_CODE_THERMAL_COMMON_SUCCESS != ignored_return_code) {
                        return_code  = ignored_return_code;
                        should_retry = false;
                    }
                }
            }
        } while (true == should_retry);
    }

    return return_code;
}

/**
 * @brief ETW callback for thermal zone parser
 *
 * Processes ETW events related to thermal zones, extracting zone information.
 *
 * @param record ETW event record
 * @param trace_context ETW trace context
 */
void thermal_common_zone_parser_trace_callback(const EVENT_RECORD &record, const krabs::trace_context &trace_context) {
    try {
        enum ThermalCommonReturnCode return_code  = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
        enum QMutexReturnCode mutex_return_code   = RETURN_CODE_MUTEX_LOCK_SUCCESS;
        bool try_parse_success                    = false;
        krabs::schema schema                      = krabs::schema(record, trace_context.schema_locator);
        std::wstring zone_bios_name_wide_string   = L"";
        std::wstring zone_description_wide_string = L"";
        ThermalCommonString bios_name             = {0};
        uint8_t id                                = 0;
        size_t wide_name_length;

        // Check if global zone name map is initialized
        if (NULL == g_zone_name_map) {
            // Global zone name map not initialized
        } else {
            // Process thermal zone rundown events
            if (schema.event_id() == THERMAL_ZONE_RUNDOWN_EVENT_ID) {
                krabs::parser parser(schema);

                // Parse zone description
                try_parse_success = parser.try_parse<std::wstring>(L"Description", zone_description_wide_string);
                if (false == try_parse_success) {
                    // Failed to parse description
                } else if (zone_description_wide_string.size() >= MAX_THERMAL_ZONE_DESCRIPTIVE_NAME_LENGTH) {
                    // Description string too long
                } else {
                    // Parse BIOS name
                    try_parse_success = parser.try_parse<std::wstring>(L"ThermalZoneBiosName", zone_bios_name_wide_string);
                    if (false == try_parse_success) {
                        // Failed to parse BIOS name
                    } else if (zone_bios_name_wide_string.size() >= MAX_THERMAL_ZONE_BIOS_NAME_LENGTH) {
                        // BIOS name string too long
                    } else {
                        // Lock mutex for thread safety
                        mutex_return_code = mutex_lock(&g_callback_mutex_info);
                        if (RETURN_CODE_MUTEX_LOCK_SUCCESS == mutex_return_code) {
                            // Check if we have room for more zones
                            if (g_zone_name_map->zone_names_ids_length >= MAX_NUM_OF_ZONES) {
                                // Too many thermal zones
                            } else {
                                // Convert wide string to narrow string
                                wcstombs_s(&wide_name_length, (char *)bios_name.string, MAX_THERMAL_ZONE_NAME_LENGTH, zone_bios_name_wide_string.c_str(), zone_bios_name_wide_string.size());
                                bios_name.string_length = static_cast<uint8_t>(wide_name_length);

                                // Parse zone ID from BIOS name
                                return_code = thermal_common_parse_id_from_bios_name(&bios_name, &id);
                                if (RETURN_CODE_THERMAL_COMMON_SUCCESS == return_code) {
                                    // Check if this zone ID is already in the map
                                    if (g_zone_name_map->zone_id_to_name_index_map[id] != UINT8_MAX) {
                                        // Skip duplicate entry
                                    } else {
                                        // Use description if available, otherwise use BIOS name
                                        if (0 == zone_description_wide_string.size()) {
                                            g_zone_name_map->zone_names[g_zone_name_map->zone_names_ids_length].string_length =
                                                snprintf((char *)g_zone_name_map->zone_names[g_zone_name_map->zone_names_ids_length].string, MAX_THERMAL_ZONE_NAME_LENGTH, "%s", bios_name.string);
                                        } else {
                                            wcstombs_s(&wide_name_length, (char *)g_zone_name_map->zone_names[g_zone_name_map->zone_names_ids_length].string, MAX_THERMAL_ZONE_NAME_LENGTH,
                                                       zone_description_wide_string.c_str(), zone_description_wide_string.size());
                                            g_zone_name_map->zone_names[g_zone_name_map->zone_names_ids_length].string_length = static_cast<uint8_t>(wide_name_length);
                                        }

                                        // Store zone ID and update mapping
                                        g_zone_name_map->zone_ids[g_zone_name_map->zone_names_ids_length] = id;
                                        g_zone_name_map->zone_id_to_name_index_map[id]                    = g_zone_name_map->zone_names_ids_length;
                                        g_zone_name_map->zone_names_ids_length++;
                                    }
                                }
                            }

                            // Unlock mutex
                            mutex_unlock(&g_callback_mutex_info);
                        }
                    }
                }
            }
        }
    } catch (const std::exception &ex) {
        // Exception handling
    } catch (...) {
        // Unknown exception handling
    }
}

/**
 * @brief Thread function for ETW listening
 *
 * Thread entry point for starting ETW trace and handling exceptions.
 *
 * @param unusedParam Unused parameter
 * @return uint32_t Status code
 */
uint32_t thermal_common_start_listening_thread(void *unusedParam) {
    enum ThermalCommonReturnCode return_code = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum QCvReturnCode cv_return_code        = RETURN_CODE_CV_CREATE_SUCCESS;

    try {
        // Start ETW trace
        g_thermal_commonTrace.start();
        return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;
    } catch (const std::exception &ex) {
        // Exception caught during trace start
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    } catch (...) {
        // Unknown exception caught
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    }

    // Notify waiting thread if exception occurred
    if (RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION == return_code) {
        cv_notify(&g_exception_cv_info);
    }

    return return_code;
}

/**
 * @brief Initialize thermal common module
 *
 * Initializes the thermal common module, creating necessary synchronization objects.
 *
 * @return enum ThermalCommonReturnCode Return code indicating success or failure
 */
enum ThermalCommonReturnCode thermal_common_init() {
    enum ThermalCommonReturnCode return_code = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum QMutexReturnCode mutex_return_code  = RETURN_CODE_MUTEX_CREATE_SUCCESS;
    MutexAttributes mutex_attributes         = {0};
    enum QListReturnCode list_return_code    = RETURN_CODE_LIST_SUCCESS;

    try {
        // Create function mutex
        mutex_attributes.mutex_name_len = snprintf((char *)mutex_attributes.mutex_name, MUTEX_NAME_SIZE, "%s", THERMAL_FUNCTION_MUTEX_NAME);
        mutex_return_code               = mutex_create(&mutex_attributes, &g_function_mutex_info);

        if (RETURN_CODE_MUTEX_CREATE_SUCCESS != mutex_return_code && RETURN_CODE_MUTEX_CREATE_EXISTS != mutex_return_code) {
            // Failed to create mutex
            return_code = RETURN_CODE_THERMAL_COMMON_NATIVE_FUNCTION_FAILED;
        } else {
            // Lock mutex for thread safety
            mutex_return_code = mutex_lock(&g_function_mutex_info);
            if (RETURN_CODE_MUTEX_LOCK_SUCCESS != mutex_return_code) {
                // Failed to lock mutex
                return_code = RETURN_CODE_THERMAL_COMMON_NATIVE_FUNCTION_FAILED;
            } else {
                // Check if we can increment instance count
                if (UINT8_MAX < static_cast<uint16_t>(g_instance_count) + 1) {
                    // Instance count would overflow
                    return_code = RETURN_CODE_THERMAL_COMMON_VALUE_WOULD_OVERFLOW;
                } else {
                    // Create list for stopped ETW sessions if needed
                    if (NULL == g_stopped_etw_sessions) {
                        list_return_code = list_create(&g_stopped_etw_sessions, THERMAL_STOPPED_SESSIONS_MUTEX_NAME);
                    }

                    // Increment instance count
                    g_instance_count++;
                    return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;
                }

                // Unlock mutex
                mutex_unlock(&g_function_mutex_info);
            }
        }
    } catch (const std::exception &ex) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    } catch (...) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    }

    return return_code;
}

/**
 * @brief Get thermal zone names
 *
 * Retrieves the names and IDs of thermal zones from the system.
 *
 * @param map Pointer to store thermal zone name map
 * @return enum ThermalCommonReturnCode Return code indicating success or failure
 */
enum ThermalCommonReturnCode thermal_common_getzone_names(struct ThermalCommonZoneNameMap *map) {
    enum ThermalCommonReturnCode return_code          = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum ThermalCommonReturnCode ignored_return_code  = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum QMutexReturnCode mutex_return_code           = RETURN_CODE_MUTEX_CREATE_SUCCESS;
    enum QCvReturnCode cv_return_code                 = RETURN_CODE_CV_CREATE_SUCCESS;
    enum QThreadReturnCode thread_return_code         = RET_QTHREAD_CREATE_SUCCESS;
    struct QThreadAttributes thermalthread_attributes = {0};
    struct QThreadInfo thread_info                    = {0};
    MutexAttributes mutex_attributes                  = {0};
    CvAttributes cv_attributes                        = {0};
    ThermalCommonStartRequest start_request           = {0};
    ThermalCommonSessions stopped_sessions            = {0};

    try {
        // Validate input
        if (NULL == map) {
            return_code = RETURN_CODE_THERMAL_COMMON_NULL_POINTER;
        } else {
            // Lock function mutex
            mutex_return_code = mutex_lock(&g_function_mutex_info);
            if (RETURN_CODE_MUTEX_LOCK_SUCCESS != mutex_return_code) {
                return_code = RETURN_CODE_THERMAL_COMMON_NATIVE_FUNCTION_FAILED;
            } else {
                // Check if module is initialized
                if (0 == g_instance_count) {
                    return_code = RETURN_CODE_THERMAL_COMMON_NO_INSTANCES;
                } else {
                    // Check if zone names are already cached
                    if (NULL != g_zone_name_map) {
                        // Use cached zone names
                        return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;
                    } else {
                        // Allocate new zone name map
                        g_zone_name_map = (struct ThermalCommonZoneNameMap *)calloc(1, sizeof(struct ThermalCommonZoneNameMap));
                        if (NULL == g_zone_name_map) {
                            return_code = RETURN_CODE_THERMAL_COMMON_MEMORY_ALLOCATION_FAILED;
                        } else {
                            // Initialize zone ID to name index map
                            memset(g_zone_name_map->zone_id_to_name_index_map, UINT8_MAX, sizeof(uint8_t) * MAX_THERMAL_ZONE_ID);

                            // Create callback mutex
                            mutex_attributes.mutex_name_len = snprintf((char *)mutex_attributes.mutex_name, MUTEX_NAME_SIZE, "%s", THERMAL_CALLBACK_MUTEX_NAME);
                            mutex_return_code               = mutex_create(&mutex_attributes, &g_callback_mutex_info);
                            if (RETURN_CODE_MUTEX_CREATE_SUCCESS != mutex_return_code) {
                                return_code = RETURN_CODE_THERMAL_COMMON_NATIVE_FUNCTION_FAILED;
                            } else {
                                // Create condition variable for exception notification
                                cv_attributes.cv_name_len = snprintf((char *)cv_attributes.cv_name, CV_NAME_SIZE, "%s", THERMAL_EXCEPTION_CV_NAME);
                                cv_return_code            = cv_create(&cv_attributes, &g_exception_cv_info);
                                if (RETURN_CODE_CV_CREATE_SUCCESS != cv_return_code) {
                                    return_code = RETURN_CODE_THERMAL_COMMON_NATIVE_FUNCTION_FAILED;
                                } else {
                                    // Set up thread attributes
                                    thermalthread_attributes.thread_name_len = snprintf((char *)thermalthread_attributes.thread_name, THREAD_NAME_SIZE, "%s", (char *)THERMAL_THREAD_NAME);
                                    thermalthread_attributes.stack_size      = 0;
                                    thermalthread_attributes.thread_params   = NULL;
                                    thermalthread_attributes.thread_fn       = reinterpret_cast<QThreadFunction>(thermal_common_start_listening_thread);

                                    // Set up start request
                                    start_request.thread_attributes           = &thermalthread_attributes;
                                    start_request.callback_function           = reinterpret_cast<void *>(&thermal_common_zone_parser_trace_callback);
                                    start_request.callback_condition_variable = &g_exception_cv_info;
                                    start_request.timeout_ms                  = TIMEOUT_WAIT_FOR_EXCEPTION_MS;
                                    stopped_sessions.qlist                    = g_stopped_etw_sessions;

                                    // Start ETW listening thread
                                    return_code = thermal_common_start_etw_listening_thread(&start_request, &stopped_sessions);
                                }

                                // Clean up condition variable
                                cv_destroy(&g_exception_cv_info);
                            }

                            // Clean up callback mutex
                            mutex_destroy(&g_callback_mutex_info);
                        }
                    }

                    // Handle failure or copy zone names to output
                    if (RETURN_CODE_THERMAL_COMMON_SUCCESS != return_code) {
                        if (NULL != g_zone_name_map) {
                            free(g_zone_name_map);
                            g_zone_name_map = NULL;
                        }
                    } else {
                        if (NULL == g_zone_name_map) {
                            return_code = RETURN_CODE_THERMAL_COMMON_UNEXPECTED_STATE;
                        } else {
                            memcpy_s(map, sizeof(ThermalCommonZoneNameMap), g_zone_name_map, sizeof(ThermalCommonZoneNameMap));
                        }
                    }
                }

                // Unlock function mutex
                mutex_unlock(&g_function_mutex_info);
            }
        }
    } catch (const std::exception &ex) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    } catch (...) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    }

    return return_code;
}

/**
 * @brief Clean up thermal common module
 *
 * Cleans up resources used by the thermal common module.
 *
 * @return enum ThermalCommonReturnCode Return code indicating success or failure
 */
enum ThermalCommonReturnCode thermal_common_cleanup() {
    enum ThermalCommonReturnCode return_code = RETURN_CODE_THERMAL_COMMON_UNCAUGHT_FAILURE;
    enum QMutexReturnCode mutex_return_code  = RETURN_CODE_MUTEX_CREATE_SUCCESS;
    enum QListReturnCode list_return_code    = RETURN_CODE_LIST_SUCCESS;
    MutexAttributes mutex_attributes         = {0};
    bool should_destroy_mutex                = false;
    ThermalCommonSessions stopped_sessions   = {0};

    try {
        // Lock function mutex
        mutex_return_code = mutex_lock(&g_function_mutex_info);
        if (RETURN_CODE_MUTEX_LOCK_SUCCESS != mutex_return_code) {
            return_code = RETURN_CODE_THERMAL_COMMON_NATIVE_FUNCTION_FAILED;
        } else {
            // Check if we can decrement instance count
            if (0 == g_instance_count) {
                return_code = RETURN_CODE_THERMAL_COMMON_VALUE_WOULD_OVERFLOW;
            } else {
                // Decrement instance count
                g_instance_count--;
                return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;
            }

            // If last instance, perform complete cleanup
            if (0 == g_instance_count) {
                should_destroy_mutex = true;

                // Free zone name map
                if (NULL != g_zone_name_map) {
                    free(g_zone_name_map);
                    g_zone_name_map = NULL;
                }

                // Restart any stopped ETW sessions
                stopped_sessions.qlist = g_stopped_etw_sessions;
                return_code            = thermal_common_restart_stopped_sessions(&stopped_sessions);

                // Delete stopped sessions list
                list_delete(&g_stopped_etw_sessions);
            }

            // Unlock function mutex
            mutex_unlock(&g_function_mutex_info);

            // Destroy function mutex if needed
            if (true == should_destroy_mutex) {
                mutex_destroy(&g_function_mutex_info);
            }
        }
    } catch (const std::exception &ex) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    } catch (...) {
        return_code = RETURN_CODE_THERMAL_COMMON_CAUGHT_EXCEPTION;
    }

    return return_code;
}
