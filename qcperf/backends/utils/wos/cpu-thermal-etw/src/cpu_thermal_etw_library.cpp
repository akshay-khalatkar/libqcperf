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
 * @file cpu_thermal_etw_library.cpp
 * @brief Implementation of CPU and thermal monitoring using ETW
 * @author Qualcomm Technologies, Inc.
 *
 * This file implements the CPU thermal ETW library functions defined in
 * cpu_thermal_etw_library.h. It provides functionality for monitoring CPU
 * performance metrics (frequency and utilization) and thermal sensor data
 * on Windows platforms using Event Tracing for Windows (ETW) and
 * Windows Management Instrumentation (WMI).
 *
 * The implementation uses Windows Performance Data Helper (PDH) API for
 * CPU metrics and ETW for thermal data collection. It provides functions
 * for initializing the monitoring system, collecting data, and cleaning up
 * resources.
 */

#define NOMINMAX
#include "cpu_thermal_etw_library.h"

#ifdef __cplusplus
#include "krabs.hpp"

#define _WIN32_DCOM
#include <comdef.h>
#include <WbemIdl.h>
#include <atlstr.h>

#include <pdh.h>
#include <pdhmsg.h>
#include <string>
#include <algorithm>
#endif

extern "C" {
#include "qthread.h"
}

#define HIGH_PRECISION_RATIO 10.0
#define KELVIN_TO_CELSIUS 273.15
#define CPU_TOTAL_SAMPLE 1
#define CPU_COUNTER_COMMON_PREFIX "\\Processor Information(0,"
#define CPU_COUNTER_FREQUENCY_SUFFIX ")\\Actual Frequency"
#define CPU_COUNTER_UTILIZATION_SUFFIX ")\\% Processor Time"
#define CPU_COUNTER_TOTAL_INSTANCE "_Total"
#define TEMPERATURE_CHANGE_EVENT_ID 11
#define MAX_NUM_OF_ZONES 64
#define THERMAL_ZONE_RUNDOWN_EVENT_ID 12
#define MAX_THERMAL_ZONE_BIOS_NAME_LENGTH 32
#define TIME_ALLOW_LISTENING_THREAD_TO_START 100
#define THERMAL_THREAD_NAME "thread-thermal-zone-parser"
#define THERMAL_MUTEX_NAME "mutex-thermal-zone-parser"
#define TZ_ID_PREFIX_W L"TZ"
#define TZ_ID_PREFIX_A "TZ"
#define DECIMAL_BASE 10
#define ANY_FLAG_ACPI_PROVIDER 0x2

#ifdef __cplusplus
typedef enum {
    WMI_QUERY_TYPE_CORES,
    // WMI_QUERY_TYPE_TEMPERATURES, In future, we might get initial temperatures through WMI since ETW waits for first callback to update
} eWMIQueryType;

bool g_cpu_initialized         = false;
bool g_thermal_initialized     = false;
uint32_t g_num_available_cores = 0;

// Each PDH function returns a status
PDH_STATUS g_cpu_status     = 0;
PDH_STATUS g_thermal_status = 0;
// Query initialized in init() and is re-used
HQUERY g_cpu_query     = NULL;
HQUERY g_thermal_query = NULL;

// Vector of counters to add
HCOUNTER *g_cpu_util_counter_list  = NULL;
HCOUNTER *g_cpu_freq_counter_list  = NULL;
HCOUNTER *g_cpu_total_util_counter = NULL;
HCOUNTER *g_cpu_total_freq_counter = NULL;
HCOUNTER *g_thermal_counter_list   = NULL;

// Populated by GetFormattedData() and contains the Frequency/Utilization value
PDH_FMT_COUNTERVALUE g_cpu_display_value     = {};
PDH_FMT_COUNTERVALUE g_thermal_display_value = {};
// Specifies counter type in GetFormattedData()
DWORD g_cpu_counter_type     = 0;
DWORD g_thermal_counter_type = 0;

// Microsoft-Windows-Kernel-Acpi GUID: C514638F-7723-485B-BCFC-96565D735D4A
krabs::provider<> g_acpiProvider(krabs::guid(ACPI_PROVIDER_GUID));
// Microsoft-Windows-Thermal-Polling GUID: e8a7c168-81ee-465c-8e8e-d39a2ac1ca41
krabs::provider<> g_pollingProvider(krabs::guid(POLLING_PROVIDER_GUID));
// Trace for Thermal events
krabs::user_trace g_thermal_trace(L"Qualcomm Profiler Thermal Trace");
struct QThreadInfo *g_thermal_thread_info                    = NULL;
struct MutexInfo *g_mutex_info                               = NULL;
struct ThermalCommonZoneNameMap *g_cpu_thermal_zone_name_map = NULL;
double *g_zone_temperatures                                  = NULL;

/**
 * @brief Helper function to query WMI for system information
 *
 * @param query_type Type of WMI query to perform
 * @param query_str WQL query string to execute
 * @return CpuThermalETWReturnCode Return code indicating success or failure
 */
enum CpuThermalETWReturnCode helperQueryWMI(eWMIQueryType query_type, char *query_str) {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;
    HRESULT h_result                    = 0;
    // Pointer to enumerator with results of WQL Query provided in ExecQuery call
    IEnumWbemClassObject *p_enumerator = NULL;
    // Pointer to IWBemServices proxy which is connected to, secured, and executes a query
    IWbemServices *p_svc = NULL;
    // Pointer to IWBem locator object used to locate IWBemServices proxy
    IWbemLocator *p_loc        = NULL;
    IWbemClassObject *pcls_obj = NULL;
    ULONG u_return             = 0;

    // For storing return from ConvertBTRToString and freeing after deep copying
    char *temp_str = NULL;

    try {
        // ----------     WMI Initialization     ----------
        if (NULL == query_str) {
            status = RETURN_CODE_CPU_THERMAL_ERROR_NULL_POINTER;
        } else {
            h_result = CoInitializeEx(0, COINIT_MULTITHREADED);

            if (FAILED(h_result)) {
                status = RETURN_CODE_CPU_THERMAL_ERROR_CO_INITIALIZE_EX;
            } else {
                h_result = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
                if (FAILED(h_result)) {
                    status = RETURN_CODE_CPU_THERMAL_ERROR_CO_INITIALIZE_SECURITY;
                } else {
                    h_result = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&p_loc);

                    if (FAILED(h_result)) {
                        status = RETURN_CODE_CPU_THERMAL_ERROR_CO_CREATE_INSTANCE;
                    } else {
                        h_result = p_loc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &p_svc);

                        if (FAILED(h_result)) {
                            status = RETURN_CODE_CPU_THERMAL_ERROR_CONNECT_SERVER;
                        } else {
                            h_result = CoSetProxyBlanket(p_svc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

                            if (FAILED(h_result)) {
                                status = RETURN_CODE_CPU_THERMAL_ERROR_CO_SET_PROXY_BLANKET;
                            } else {
                                h_result = p_svc->ExecQuery(bstr_t("WQL"), bstr_t(query_str), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &p_enumerator);
                                if (FAILED(h_result)) {
                                    status = RETURN_CODE_CPU_THERMAL_ERROR_EXEC_QUERY;
                                }
                            }  // FAILED(CoSetProxyBlanket)
                        }      // FAILED(ConnectServer)
                    }          // FAILED(CoCreateInstance)
                }              // FAILED(CoInitializeSecurity)
            }                  // FAILED(CoInitializeEx)
        }                      // query_str

        // ----------     WMI Queries     ----------
        if (RETURN_CODE_CPU_THERMAL_SUCCESS == status) {
            if (query_type == WMI_QUERY_TYPE_CORES) {
                while (NULL != p_enumerator) {
                    h_result = p_enumerator->Next(WBEM_INFINITE, 1, &pcls_obj, &u_return);
                    if (FAILED(h_result) || NULL == pcls_obj || 0 == u_return) {
                        // Ignore error. We reached the end of enumerator
                        break;
                    } else {
                        VARIANT vtProp;
                        VariantInit(&vtProp);
                        h_result = pcls_obj->Get(L"NumberOfCores", 0, &vtProp, 0, 0);
                        if (FAILED(h_result)) {
                            status = RETURN_CODE_CPU_THERMAL_ERROR_WMI_GET;
                        } else {
                            g_num_available_cores = vtProp.intVal;
                        }  // FAILED(pcls_obj->Get())
                        VariantClear(&vtProp);
                        pcls_obj->Release();
                        pcls_obj = NULL;
                        u_return = 0;
                    }
                }  // FAILED(p_enumerator->Next())
            } else {
                status = RETURN_CODE_INVALID_QUERY;
            }
        }  // if (RETURN_CODE_CPU_THERMAL_SUCCESS == status)

        // General Cleanup
        if (NULL != p_svc) {
            p_svc->Release();
            p_svc = NULL;
        }
        if (NULL != p_loc) {
            p_loc->Release();
            p_loc = NULL;
        }
        if (NULL != p_enumerator) {
            p_enumerator->Release();
            p_enumerator = NULL;
        }
        CoUninitialize();
    } catch (const std::exception &ex) {
        // Exception caught
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    } catch (...) {
        // Unknown exception caught
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    }

    if (RETURN_CODE_CPU_THERMAL_SUCCESS == status) {
        switch (query_type) {
        case WMI_QUERY_TYPE_CORES:
            if (0 == g_num_available_cores) {
                status = RETURN_CODE_CPU_ERROR_NO_CORES;
            }
            break;
        default:
            status = RETURN_CODE_CPU_THERMAL_ERROR_UNKNOWN_QUERY;
            break;
        }
    }

    return status;
}
#endif __cplusplus
/**
 * @brief Helper function to get the number of CPU cores using GetSystemInfo
 *
 * Uses GetSystemInfo API instead of WMI since WMI API internally uses COM component,
 * which can fail with multiple initializations when used with UI components.
 *
 * @return CpuThermalETWReturnCode Return code indicating success or failure
 */
enum CpuThermalETWReturnCode helperUpdateCores() {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;
    SYSTEM_INFO sysinfo                 = {};

    try {
        GetSystemInfo(&sysinfo);
        g_num_available_cores = sysinfo.dwNumberOfProcessors;
        if (0 == g_num_available_cores) {
            status = RETURN_CODE_CPU_ERROR_NO_CORES;
        }
    } catch (const std::exception &ex) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    } catch (...) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    }

    return status;
}

enum CpuThermalETWReturnCode cpu_init() {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;

    if (true == g_cpu_initialized) {
        status = RETURN_CODE_CPU_ERROR_ALREADY_INITIALIZED;
    } else {
        try {
            g_cpu_initialized = true;
            status            = helperUpdateCores();
            if (status == RETURN_CODE_CPU_THERMAL_SUCCESS) {
                g_cpu_status = PdhOpenQuery(NULL, NULL, &g_cpu_query);
                if (ERROR_SUCCESS != g_cpu_status || NULL == g_cpu_query) {
                    status = RETURN_CODE_CPU_ERROR_OPEN_QUERY;
                } else {
                    g_cpu_util_counter_list  = (HCOUNTER *)calloc(g_num_available_cores, sizeof(HCOUNTER));
                    g_cpu_freq_counter_list  = (HCOUNTER *)calloc(g_num_available_cores, sizeof(HCOUNTER));
                    g_cpu_total_util_counter = (HCOUNTER *)calloc(CPU_TOTAL_SAMPLE, sizeof(HCOUNTER));
                    g_cpu_total_freq_counter = (HCOUNTER *)calloc(CPU_TOTAL_SAMPLE, sizeof(HCOUNTER));
                    if (NULL == g_cpu_util_counter_list || NULL == g_cpu_freq_counter_list || NULL == g_cpu_total_util_counter || NULL == g_cpu_total_freq_counter) {
                        status = RETURN_CODE_CPU_THERMAL_ERROR_CALLOC;
                    } else {
                        std::string counterInstance = "";
                        std::string utilCounterPath = "";
                        std::string freqCounterPath = "";

                        for (uint32_t coreIdx = 0; RETURN_CODE_CPU_THERMAL_SUCCESS == status && coreIdx < g_num_available_cores; coreIdx++) {
                            counterInstance.clear();
                            counterInstance = std::to_string(coreIdx);

                            utilCounterPath.clear();
                            utilCounterPath = CPU_COUNTER_COMMON_PREFIX + counterInstance + CPU_COUNTER_UTILIZATION_SUFFIX;
                            if (PdhAddCounterA(g_cpu_query, utilCounterPath.c_str(), 0, g_cpu_util_counter_list + coreIdx) != ERROR_SUCCESS) {
                                status = RETURN_CODE_CPU_ERROR_UTIL_ADD_COUNTER;
                                // Failed to add CPU utilization counter
                            }

                            freqCounterPath.clear();
                            freqCounterPath = CPU_COUNTER_COMMON_PREFIX + counterInstance + CPU_COUNTER_FREQUENCY_SUFFIX;
                            if (PdhAddCounterA(g_cpu_query, freqCounterPath.c_str(), 0, g_cpu_freq_counter_list + coreIdx) != ERROR_SUCCESS) {
                                status = RETURN_CODE_CPU_ERROR_FREQ_ADD_COUNTER;
                                // Failed to add CPU frequency counter
                            }
                        }  // for (coreIdx in g_num_available_cores)

                        counterInstance.clear();
                        counterInstance = CPU_COUNTER_TOTAL_INSTANCE;
                        utilCounterPath.clear();
                        utilCounterPath = CPU_COUNTER_COMMON_PREFIX + counterInstance + CPU_COUNTER_UTILIZATION_SUFFIX;
                        if (PdhAddCounterA(g_cpu_query, utilCounterPath.c_str(), 0, g_cpu_total_util_counter) != ERROR_SUCCESS) {
                            status = RETURN_CODE_CPU_ERROR_FREQ_ADD_COUNTER;
                            // Failed to add CPU total utilization counter
                        }
                        freqCounterPath.clear();
                        freqCounterPath = CPU_COUNTER_COMMON_PREFIX + counterInstance + CPU_COUNTER_FREQUENCY_SUFFIX;
                        if (PdhAddCounterA(g_cpu_query, freqCounterPath.c_str(), 0, g_cpu_total_freq_counter) != ERROR_SUCCESS) {
                            status = RETURN_CODE_CPU_ERROR_FREQ_ADD_COUNTER;
                            // Failed to add CPU total frequency counter
                        }
                    }  // if (!calloc)
                }      // if (!PdhOpenQuery)
            }          // if (helperQueryWMI)
        } catch (const std::exception &ex) {
            status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
        } catch (...) {
            status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
        }

        // If anything could've been allocated, cleanup before returning error
        if (status != RETURN_CODE_CPU_THERMAL_SUCCESS) {
            cpu_cleanup();
            g_cpu_initialized = false;
        }
    }

    return status;
}

enum CpuThermalETWReturnCode cpu_get_num_cores(uint8_t *num_cores) {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;

    try {
        if (NULL == num_cores) {
            status = RETURN_CODE_CPU_THERMAL_ERROR_NULL_POINTER;
        } else if (false == g_cpu_initialized) {
            status = RETURN_CODE_CPU_ERROR_LIB_UNINITIALIZED;
        } else {
            *num_cores = g_num_available_cores;
        }
    } catch (const std::exception &ex) {
        // Exception caught
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    } catch (...) {
        // Unknown exception caught
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    }

    return status;
}

enum CpuThermalETWReturnCode cpu_get_info(struct CpuInfoQuery *cpu_info_query) {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;

    try {
        // Allow calling PdhCollectQueryData if library is at least initialized.
        // Would allow user to make first call to PdhCollectQueryData
        // (for counters that require at least 2 calls for valid data)
        if (false == g_cpu_initialized) {
            status = RETURN_CODE_CPU_ERROR_LIB_UNINITIALIZED;
        } else {
            if (NULL == cpu_info_query || NULL == cpu_info_query->cpu_info) {
                status = RETURN_CODE_CPU_THERMAL_ERROR_NULL_POINTER;
            } else if (0 == cpu_info_query->cpu_info_len) {
                status = RETURN_CODE_CPU_ERROR_INVALID_ARG;
            } else if (ERROR_SUCCESS != PdhCollectQueryData(g_cpu_query)) {
                status = RETURN_CODE_CPU_ERROR_COLLECT_QUERY;
            } else {
                uint32_t coreIdx = 0;
                while (coreIdx < cpu_info_query->cpu_info_len && coreIdx < g_num_available_cores) {
                    // Utilization
                    if (ALL == cpu_info_query->info_type || UTILIZATION == cpu_info_query->info_type) {
                        g_cpu_status = PdhGetFormattedCounterValue(g_cpu_util_counter_list[coreIdx], PDH_FMT_DOUBLE, &g_cpu_counter_type, &g_cpu_display_value);
                        if (ERROR_SUCCESS != g_cpu_status) {
                            // Ignore errors
                            // status = WARNING_INVALID_DATA;
                        } else {
                            cpu_info_query->cpu_info[coreIdx].utilization = g_cpu_display_value.doubleValue;
                            // CPU utilization value retrieved
                        }
                    }  // if (utilization)

                    // Frequency
                    if (ALL == cpu_info_query->info_type || FREQUENCY == cpu_info_query->info_type) {
                        // counterList_ has { util per core ... util total ... freq per core ... freq total }
                        g_cpu_status = PdhGetFormattedCounterValue(g_cpu_freq_counter_list[coreIdx], PDH_FMT_DOUBLE, &g_cpu_counter_type, &g_cpu_display_value);
                        if (g_cpu_status != ERROR_SUCCESS) {
                            // Ignore errors
                            // status = CPU_WARNING_INVALID_DATA;
                        } else {
                            cpu_info_query->cpu_info[coreIdx].frequency = g_cpu_display_value.doubleValue;
                            // CPU frequency value retrieved
                        }
                    }  // if (frequency)

                    cpu_info_query->cpu_info[coreIdx].core_id   = coreIdx;
                    cpu_info_query->cpu_info[coreIdx].info_type = cpu_info_query->info_type;
                    coreIdx++;
                }  // while (coreIdx in range)
                cpu_info_query->cpu_info_len = coreIdx;

                g_cpu_status = PdhGetFormattedCounterValue(*g_cpu_total_util_counter, PDH_FMT_DOUBLE, &g_cpu_counter_type, &g_cpu_display_value);
                if (g_cpu_status != ERROR_SUCCESS) {
                    // Ignore errors
                    // status = WARNING_INVALID_DATA;
                } else {
                    cpu_info_query->total_utilization = g_cpu_display_value.doubleValue;
                    // Total CPU utilization value retrieved
                }

                g_cpu_status = PdhGetFormattedCounterValue(*g_cpu_total_freq_counter, PDH_FMT_DOUBLE, &g_cpu_counter_type, &g_cpu_display_value);
                if (g_cpu_status != ERROR_SUCCESS) {
                    // Ignore errors
                    // status = WARNING_INVALID_DATA;
                } else {
                    cpu_info_query->total_frequency = g_cpu_display_value.doubleValue;
                    // Total CPU frequency value retrieved
                }
            }  // if(invalid args)
        }      // if (!g_cpu_initialized)
    } catch (const std::exception &ex) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    } catch (...) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    }

    return status;
}

enum CpuThermalETWReturnCode cpu_cleanup() {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;

    try {
        if (true == g_cpu_initialized) {
            // empty counter vector
            for (uint32_t i = 0; i < g_num_available_cores; i++) {
                if (NULL != g_cpu_util_counter_list && NULL != g_cpu_util_counter_list[i]) {
                    // allocated by PdhAddCounter
                    PdhRemoveCounter(g_cpu_util_counter_list[i]);
                    g_cpu_util_counter_list[i] = NULL;
                }
                if (NULL != g_cpu_freq_counter_list && NULL != g_cpu_freq_counter_list[i]) {
                    // allocated by PdhAddCounter
                    PdhRemoveCounter(g_cpu_freq_counter_list[i]);
                    g_cpu_freq_counter_list[i] = NULL;
                }
            }
            if (NULL != g_cpu_util_counter_list) {
                free(g_cpu_util_counter_list);
                g_cpu_util_counter_list = NULL;
            }
            if (NULL != g_cpu_freq_counter_list) {
                free(g_cpu_freq_counter_list);
                g_cpu_freq_counter_list = NULL;
            }

            if (NULL != g_cpu_total_util_counter) {
                PdhRemoveCounter(*g_cpu_total_util_counter);
                free(g_cpu_total_util_counter);
                g_cpu_total_util_counter = NULL;
            }
            if (NULL != g_cpu_total_freq_counter) {
                PdhRemoveCounter(g_cpu_total_freq_counter);
                free(g_cpu_total_freq_counter);
                g_cpu_total_freq_counter = NULL;
            }

            // Close the query.
            if (NULL != g_cpu_query && ERROR_SUCCESS != PdhCloseQuery(g_cpu_query)) {
                status = RETURN_CODE_CPU_ERROR_CLOSE_QUERY;
            }

            g_num_available_cores = 0;
            g_cpu_initialized     = false;
        }
    } catch (const std::exception &ex) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    } catch (...) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    }

    return status;
}

void thermal_sensor_temperatureTraceCallback(const EVENT_RECORD &record, const krabs::trace_context &trace_context) {
    enum ThermalCommonReturnCode common_return_code = RETURN_CODE_THERMAL_COMMON_SUCCESS;
    ThermalCommonString thermal_string              = {0};

    try {
        enum QMutexReturnCode mutex_return_code = RETURN_CODE_MUTEX_LOCK_SUCCESS;
        bool try_parse_success                  = false;
        krabs::schema schema(record, trace_context.schema_locator);
        uint32_t temperature                              = 0;
        std::wstring name_wstr                            = L"";
        char name_cstr[MAX_THERMAL_ZONE_BIOS_NAME_LENGTH] = {0};
        uint32_t name_len                                 = 0;
        uint8_t thermal_zone_index                        = 0;
        double celsius                                    = 0.0;

        if (schema.event_id() == TEMPERATURE_CHANGE_EVENT_ID) {
            krabs::parser parser(schema);
            // Can get memory errors when using <std::string>
            try_parse_success = parser.try_parse<std::wstring>(L"ThermalZoneDeviceInstance", name_wstr);
            if (true == try_parse_success) {
                name_len          = (uint32_t)wcstombs(name_cstr, name_wstr.c_str(), name_wstr.size());
                try_parse_success = parser.try_parse<std::uint32_t>(L"Temperature", temperature);
                if (true == try_parse_success) {
                    celsius = (temperature / HIGH_PRECISION_RATIO) - KELVIN_TO_CELSIUS;

                    thermal_string.string_length = snprintf((char *)thermal_string.string, MAX_THERMAL_ZONE_NAME_LENGTH, "%s", name_cstr);
                    common_return_code           = thermal_common_parse_id_from_bios_name(&thermal_string, &thermal_zone_index);
                    if (RETURN_CODE_THERMAL_COMMON_SUCCESS == common_return_code) {
                        if (UINT8_MAX == g_cpu_thermal_zone_name_map->zone_id_to_name_index_map[thermal_zone_index]) {
                            // Zone ID doesn't map to any name
                        } else {
                            mutex_return_code = mutex_lock(g_mutex_info);
                            if (RETURN_CODE_MUTEX_LOCK_SUCCESS != mutex_return_code) {
                                // Mutex lock failed
                            } else {
                                g_zone_temperatures[g_cpu_thermal_zone_name_map->zone_id_to_name_index_map[thermal_zone_index]] = celsius;
                                mutex_return_code                                                                               = mutex_unlock(g_mutex_info);
                                if (RETURN_CODE_MUTEX_UNLOCK_SUCCESS != mutex_return_code) {
                                    // Mutex unlock failed
                                }  // if mutex_unlock failure
                            }      // if mutex_lock success
                        }          // if known id
                    }              // SUCCESS(thermal_common_parse_id_from_bios_name)
                }                  // if try_parse success
            }                      // if try_parse success
        }                          // if event is TemperatureChange
    } catch (const std::exception &ex) {
        // Exception handling
    } catch (...) {
        // Unknown exception handling
    }
}

uint32_t thermal_sensor_startListeningThread(void *unusedParam) {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;

    try {
        g_thermal_trace.start();
    } catch (const std::exception &ex) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    } catch (...) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    }
    return status;
}

enum CpuThermalETWReturnCode thermal_sensor_init(struct ThermalCommonZoneNameMap *map) {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;

    enum QThreadReturnCode thread_return_code          = RET_QTHREAD_CREATE_SUCCESS;
    enum QMutexReturnCode mutex_return_code            = RETURN_CODE_MUTEX_CREATE_SUCCESS;
    struct QThreadAttributes *thermalthread_attributes = NULL;
    struct MutexAttributes *mutex_attributes            = NULL;

    try {
        if (true == g_thermal_initialized) {
            status = RETURN_CODE_THERMAL_ERROR_ALREADY_INITIALIZED;
        } else if (NULL == map || 0 == map->zone_names_ids_length) {
            status = RETURN_CODE_CPU_THERMAL_ERROR_NULL_POINTER;
        } else {
            g_thermal_initialized = true;

            thermalthread_attributes    = (struct QThreadAttributes *)calloc(1, sizeof(struct QThreadAttributes));
            g_thermal_thread_info       = (struct QThreadInfo *)calloc(1, sizeof(struct QThreadInfo));
            mutex_attributes             = (struct MutexAttributes *)calloc(1, sizeof(struct MutexAttributes));
            g_mutex_info                = (struct MutexInfo *)calloc(1, sizeof(struct MutexInfo));
            g_cpu_thermal_zone_name_map = (struct ThermalCommonZoneNameMap *)calloc(1, sizeof(struct ThermalCommonZoneNameMap));
            g_zone_temperatures         = (double *)calloc(MAX_NUM_OF_ZONES, sizeof(double));
            if (NULL == thermalthread_attributes || NULL == g_thermal_thread_info || NULL == mutex_attributes || NULL == g_mutex_info || NULL == g_cpu_thermal_zone_name_map ||
                NULL == g_zone_temperatures) {
                status = RETURN_CODE_CPU_THERMAL_ERROR_CALLOC;
            } else {
                memcpy_s(g_cpu_thermal_zone_name_map, sizeof(struct ThermalCommonZoneNameMap), map, sizeof(struct ThermalCommonZoneNameMap));

                mutex_attributes->mutex_name_len = snprintf((char *)mutex_attributes, MUTEX_NAME_SIZE, "%s", THERMAL_MUTEX_NAME);
                mutex_return_code               = mutex_create(mutex_attributes, g_mutex_info);
                if (RETURN_CODE_MUTEX_CREATE_SUCCESS != mutex_return_code) {
                    status = RETURN_CODE_CPU_THERMAL_NATIVE_FUNCTION_FAILED;
                } else {
                    ENABLE_THERMAL_PROVIDERS(thermal_sensor_temperatureTraceCallback, g_thermal_trace, g_acpiProvider, g_pollingProvider);

                    thermalthread_attributes->thread_name_len = snprintf((char *)thermalthread_attributes->thread_name, (size_t)strlen(THERMAL_THREAD_NAME), "%s", (char *)THERMAL_THREAD_NAME);
                    thermalthread_attributes->stack_size      = 0;
                    thermalthread_attributes->thread_params   = NULL;
                    thermalthread_attributes->thread_fn       = reinterpret_cast<QThreadFunction>(thermal_sensor_startListeningThread);
                    thread_return_code                        = thread_create(thermalthread_attributes, g_thermal_thread_info);
                    if (RET_QTHREAD_CREATE_SUCCESS != thread_return_code) {
                        status = RETURN_CODE_CPU_THERMAL_NATIVE_FUNCTION_FAILED;
                    }
                }
            }
        }
    } catch (const std::exception &ex) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    } catch (...) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    }

    if (status != RETURN_CODE_CPU_THERMAL_SUCCESS) {
        // something might've been allocated. free before returning the error
        thermal_sensor_cleanup();
        g_thermal_initialized = false;
    }

    if (NULL != thermalthread_attributes) {
        free(thermalthread_attributes);
        thermalthread_attributes = NULL;
    }

    if (NULL != mutex_attributes) {
        free(mutex_attributes);
        mutex_attributes = NULL;
    }

    return status;
}

enum CpuThermalETWReturnCode thermal_sensor_get_info(struct ThermalInfoQuery *request) {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;

    try {
        enum QMutexReturnCode mutex_return_code = RETURN_CODE_MUTEX_CREATE_SUCCESS;
        if (false == g_thermal_initialized) {
            status = RETURN_CODE_THERMAL_ERROR_LIB_UNINITIALIZED;
        } else if (NULL == request || NULL == request->temperatures || 0 == request->temperatures_length) {
            status = RETURN_CODE_CPU_THERMAL_ERROR_NULL_POINTER;
        } else {
            mutex_return_code = mutex_lock(g_mutex_info);
            if (RETURN_CODE_MUTEX_LOCK_SUCCESS != mutex_return_code) {
                // Mutex lock failed
            } else {
                if (g_cpu_thermal_zone_name_map->zone_names_ids_length < request->temperatures_length) {
                    request->temperatures_length = g_cpu_thermal_zone_name_map->zone_names_ids_length;
                }
                memcpy_s(request->temperatures, request->temperatures_length * sizeof(double), g_zone_temperatures, g_cpu_thermal_zone_name_map->zone_names_ids_length * sizeof(double));
                mutex_return_code = mutex_unlock(g_mutex_info);
                if (RETURN_CODE_MUTEX_UNLOCK_SUCCESS != mutex_return_code) {
                    // Mutex unlock failed
                }
            }
        }
    } catch (const std::exception &ex) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    } catch (...) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    }

    return status;
}

enum CpuThermalETWReturnCode thermal_sensor_cleanup() {
    enum CpuThermalETWReturnCode status = RETURN_CODE_CPU_THERMAL_SUCCESS;

    try {
        enum QMutexReturnCode mutex_return_code   = RETURN_CODE_MUTEX_CREATE_SUCCESS;
        enum QThreadReturnCode thread_return_code = RET_QTHREAD_CREATE_SUCCESS;

        if (true == g_thermal_initialized) {
            g_thermal_trace.stop();
            if (NULL != g_thermal_thread_info) {
                thread_return_code = thread_join(g_thermal_thread_info);
                if (RET_QTHREAD_JOIN_SUCCESS != thread_return_code) {
                    status = RETURN_CODE_CPU_THERMAL_NATIVE_FUNCTION_FAILED;
                } else {
                    thread_return_code = thread_destroy(g_thermal_thread_info);
                    if (thread_return_code != RET_QTHREAD_DESTROY_SUCCESS) {
                        status = RETURN_CODE_CPU_THERMAL_NATIVE_FUNCTION_FAILED;
                    }
                }

                free(g_thermal_thread_info);
                g_thermal_thread_info = NULL;
            }

            if (NULL != g_mutex_info) {
                mutex_return_code = mutex_destroy(g_mutex_info);
                if (RETURN_CODE_MUTEX_DESTROY_SUCCESS != mutex_return_code) {
                    status = RETURN_CODE_CPU_THERMAL_NATIVE_FUNCTION_FAILED;
                }
                free(g_mutex_info);
                g_mutex_info = NULL;
            }

            if (NULL != g_cpu_thermal_zone_name_map) {
                free(g_cpu_thermal_zone_name_map);
                g_cpu_thermal_zone_name_map = NULL;
            }

            if (NULL != g_zone_temperatures) {
                free(g_zone_temperatures);
                g_zone_temperatures = NULL;
            }

            g_thermal_initialized = false;
        }
    } catch (const std::exception &ex) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    } catch (...) {
        status = RETURN_CODE_CPU_THERMAL_CAUGHT_EXCEPTION;
    }

    return status;
}
