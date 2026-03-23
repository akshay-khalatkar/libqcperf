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
 * @file qcperf_common.h
 * @brief Common definitions and data structures for QcPerf library
 *
 * This header file contains common type definitions, enumerations, and data structures
 * used throughout the QcPerf performance monitoring library. It defines the core data
 * types, return codes, callback function signatures, and structures for handling
 * performance metrics and backend communication.
 */

#ifndef QC_PERF_COMMON_H  // QC_PERF_COMMON_H
#define QC_PERF_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "qcperf_backend_enum.h"
/**
 * @def MAX_METRICS_PER_BACKEND
 * @brief Maximum number of metrics supported per backend
 *
 * Defines the upper limit for the number of performance metrics that can be
 * registered and tracked by a single backend implementation.
 */

#define MAX_METRICS_PER_BACKEND 256
#define ERROR_STRING_MAX_LEN 256

/**
 * @def METRIC_NAME_MAX_LEN
 * @brief Maximum length of metric name string
 *
 * Defines the maximum buffer size for storing metric name strings.
 */
#define METRIC_NAME_MAX_LEN 64

/**
 * @def MAX_SAMPLING_STREAMING_RATES_LEN
 * @brief Maximum number of sampling and streaming rates supported
 *
 * Defines the maximum number of different sampling and streaming rates
 * that can be supported by a capability.
 */
#define MAX_SAMPLING_STREAMING_RATES_LEN 32

/**
 * @def RETURN_CODE_INFO_STRING_MAX_LEN
 * @brief Maximum length of return code information string
 *
 * Specifies the maximum buffer size for storing descriptive information
 * associated with return codes.
 */
#define RETURN_CODE_INFO_STRING_MAX_LEN 256

/**
 * @def CAPABILITY_NAME_MAX_LEN
 * @brief Maximum length of capability name string
 *
 * Defines the maximum buffer size for storing capability name string.
 */
#define CAPABILITY_NAME_MAX_LEN 64

/**
 * @def METRIC_DESC_MAX_LEN
 * @brief Maximum length of metric description string
 *
 * Defines the maximum buffer size for storing metric description strings.
 */
#define MAX_METRIC_DESCRIPTION_LEN 256

/**
 * @def METRIC_UNIT_MAX_LEN
 * @brief Maximum length of metric unit string
 *
 * Defines the maximum buffer size for storing metric unit strings.
 */
#define MAX_METRIC_UNIT_LEN 16

#define ERROR_STRING_MAX_LEN 256

struct QcPerfData;
struct QcPerfMessage;

/**
 * @enum QcPerfReturnCode
 * @brief Return codes for QcPerf API functions
 *
 * Enumeration of all possible return codes that can be returned by QcPerf
 * library functions. These codes indicate the success or failure status of
 * API operations.
 */
enum QcPerfReturnCode {
    QC_PERF_RETURN_CODE_SUCCESS = 0,               /**< Operation completed successfully */
    QC_PERF_RETURN_CODE_FAILED,                    /**< Operation failed with unspecified error */
    QC_PERF_RETURN_CODE_NOT_SUPPORTED,             /**< Requested operation is not supported */
    QC_PERF_RETURN_CODE_ALREADY_INITIALIZED,       /**< Component is already initialized */
    QC_PERF_RETURN_CODE_INVALID_HANDLE,            /**< Invalid handle provided */
    QC_PERF_RETURN_CODE_INVALID_ARGUMENTS,         /**< Invalid arguments passed to function */
    QC_PERF_RETURN_CODE_NOT_INITIALIZED,           /**< Component is not initialized */
    QC_PERF_RETURN_CODE_CAPABILITY_NOT_FOUND,      /**< Requested capability not found */
    QC_PERF_RETURN_CODE_INVALID_BUFFER_SIZE,       /**< Buffer size is invalid or insufficient */
    QC_PERF_RETURN_CODE_NULL_POINTER,              /**< Null pointer encountered */
    QC_PERF_RETURN_CODE_CALLOC_FAILED,             /**< Memory allocation failed */
    QC_PERF_RETURN_CODE_BACKEND_ALREADY_CONNECTED, /**< Backend is already connected */
    QC_PERF_RETURN_CODE_INVALID_BACKEND_ID,        /**< Invalid backend identifier */
    QC_PERF_RETURN_CODE_BACKEND_NOT_CONNECTED,     /**< Backend is not connected */
    QC_PERF_RETURN_CODE_CALLBACK_ALREADY_SET,      /**< Callback already set */
    QC_PERF_RETURN_CODE_NATIVE_LIBRARY_FAILED,     /**< Native library call failed */
};

/**
 * @typedef QcPerfMessageCallback
 * @brief Callback function type for handling message notifications
 *
 * This callback is invoked to deliver informational, warning, or error messages
 * to the application during QcPerf operations.
 *
 * @param message Pointer to the message string
 * @param message_length Length of the message in bytes
 * @return QcPerfReturnCode indicating the callback execution status
 */
typedef enum QcPerfReturnCode (*QcPerfMessageCallback)(struct QcPerfMessage* message);

/**
 * @typedef QcPerfDataCallback
 * @brief Callback function type for handling performance metric results
 *
 * This callback is invoked to deliver performance metric data collected by
 * the backend to the application. It is called periodically based on the
 * configured streaming rate.
 *
 * @param data Pointer to QcPerfData structure containing metric data
 * @return QcPerfReturnCode indicating the callback execution status
 */
typedef enum QcPerfReturnCode (*QcPerfDataCallback)(struct QcPerfData* data);

/**
 * @enum QcPerfMessageLevel
 * @brief Message severity levels for backend communication
 *
 * Defines the severity levels for messages sent from backends to applications.
 * These levels help applications prioritize and filter messages based on
 * their importance.
 */
enum QcPerfMessageLevel {
    QC_PERF_MESSAGE_LEVEL_DEBUG = 0, /**< Debug message */
    QC_PERF_MESSAGE_LEVEL_INFO,      /**< Informational message */
    QC_PERF_MESSAGE_LEVEL_WARNING,   /**< Warning condition */
    QC_PERF_MESSAGE_LEVEL_ERROR,     /**< Error condition */
};

/**
 * @enum QcPerfDataType
 * @brief Data types supported for performance metrics
 *
 * Enumeration of data types that can be used to represent performance metric
 * values. This allows metrics to be expressed in various formats depending on
 * their nature.
 */
enum QcPerfDataType {
    QC_PERF_DATA_TYPE_BOOL = 0, /**< Boolean value (true/false) */
    QC_PERF_DATA_TYPE_UINT64,   /**< Unsigned 64-bit integer */
    QC_PERF_DATA_TYPE_INT64,    /**< Signed 64-bit integer */
    QC_PERF_DATA_TYPE_DOUBLE,   /**< Double-precision floating point */
    QC_PERF_DATA_TYPE_STRING,   /**< String value */
};

/**
 * @struct QcPerfReturnCodeInfo
 * @brief Detailed information about a return code
 *
 * Structure containing comprehensive information about an operation's return
 * status, including the return code, its severity type, and a descriptive message.
 */
struct QcPerfReturnCodeInfo {
    enum QcPerfReturnCode return_code;                 /**< The return code value */
    uint8_t info_str[RETURN_CODE_INFO_STRING_MAX_LEN]; /**< Descriptive information string */
    size_t info_str_len;                               /**< Length of the information string */
};

/**
 * @struct QcPerfRequest
 * @brief Request structure for performance monitoring configuration
 *
 * Defines the parameters for requesting performance data collection from a
 * specific capability, including the desired sampling and streaming rates.
 */
struct QcPerfRequest {
    uint8_t capability_id;   /**< Unique identifier for the capability to monitor */
    uint16_t streaming_rate; /**< Rate at which data is streamed to callback (in milliseconds) */
    uint16_t sampling_rate;  /**< Rate at which backend samples the metric (in milliseconds) */
};

/**
 * @struct QcPerfGenericType
 * @brief Generic container for metric values of different types
 *
 * A union-like structure that can hold metric values of various data types.
 * The data_type field indicates which value field is valid.
 */
struct QcPerfGenericType {
    enum QcPerfDataType data_type; /**< Type of data stored in this structure */
    bool bool_value;               /**< Boolean value (valid when data_type is QC_PERF_DATA_TYPE_BOOL) */
    uint64_t uint64_value;         /**< Unsigned 64-bit integer (valid when data_type is QC_PERF_DATA_TYPE_UINT64) */
    int64_t int64_value;           /**< Signed 64-bit integer (valid when data_type is QC_PERF_DATA_TYPE_INT64) */
    double double_value;           /**< Double-precision float (valid when data_type is QC_PERF_DATA_TYPE_DOUBLE) */
    char* string_value;            /**< String value (valid when data_type is QC_PERF_DATA_TYPE_STRING) */
    size_t string_value_len;
};

/**
 * @struct QcPerfMessage
 * @brief Message structure for communication from backends
 *
 * Contains a message string and metadata about the message, including
 * its length and severity level. Used for delivering informational,
 * warning, or error messages from backends to applications.
 */
struct QcPerfMessage {
    uint8_t backend_id;                    /**< Unique identifier for this backend */
    uint8_t capability_id;                 /**< Unique identifier for the capability message is intented to*/
    const char* message;                   /**< Pointer to the message text */
    size_t message_length;                 /**< Length of the message in bytes */
    enum QcPerfMessageLevel message_level; /**< Severity level of the message */
};

/**
 * @struct QcPerfMetricResponse
 * @brief Response structure for a single performance metric
 *
 * Contains the identifier and value of a single performance metric collected
 * by the backend.
 */
struct QcPerfMetricResponse {
    uint64_t timestamp;                    /**< Timestamp when metrics were collected */
    uint16_t metric_id;                    /**< Unique identifier for the metric */
    struct QcPerfGenericType metric_value; /**< Value of the metric */
};

/**
 * @struct QcPerfData
 * @brief Complete data structure for performance data collection
 *
 * Contains all performance metrics collected for a capability at a specific
 * timestamp, along with status information.
 */
struct QcPerfData {
    uint8_t backend_id;                           /**< Unique identifier for this backend */
    uint8_t capabilityId;                         /**< Capability identifier for this data */
    struct QcPerfMetricResponse* metric_response; /**< Array of metric responses */
    uint32_t metric_response_len;                 /**< Number of metrics in the data array */
};

/**
 * @struct QcPerfCapabilityInfo
 * @brief Information about a performance monitoring capability
 *
 * Describes a single capability provided by a backend, including its identifier,
 * human-readable name, supported metrics, and available sampling/streaming rates.
 */
struct QcPerfCapabilityInfo {
    uint8_t capability_id;                         /**< Unique identifier for the capability */
    char capability_name[CAPABILITY_NAME_MAX_LEN]; /**< Human-readable name of the capability */
    size_t capability_name_len;
    struct QcPerfMetricInfo* metric_ids_list;                  /**< Array of metrics supported by this capability */
    uint8_t metric_ids_list_len;                               /**< Number of metrics in the metric_ids_list array */
    uint16_t streaming_rate[MAX_SAMPLING_STREAMING_RATES_LEN]; /**< Array of supported streaming rates in milliseconds */
    uint8_t streaming_rate_len;                                /**< Number of valid streaming rates in the streaming_rate array */
    uint16_t sampling_rate[MAX_SAMPLING_STREAMING_RATES_LEN];  /**< Array of supported sampling rates in milliseconds */
    uint8_t sampling_rate_len;                                 /**< Number of valid sampling rates in the sampling_rate array */
};

/**
 * @struct QcPerfMetricInfo
 * @brief Detailed information about a performance metric
 *
 * Provides comprehensive metadata about a metric, including its identifier,
 * name, unit of measurement, and description.
 */
struct QcPerfMetricInfo {
    uint16_t metric_id;                    /**< Unique identifier for the metric */
    char metric_name[METRIC_NAME_MAX_LEN]; /**< Human-readable name of the metric */
    size_t metric_name_len;
    char metric_description[MAX_METRIC_DESCRIPTION_LEN]; /**< Detailed description of the metric */
    size_t metric_description_len;
    char metric_unit[MAX_METRIC_UNIT_LEN]; /**< Unit of measurement (e.g., "MHz", "°C", "%") */
    size_t metric_unit_len;
};

/**
 * @struct QcPerfBackendInfo
 * @brief Complete information about a performance monitoring backend
 *
 * Contains all metadata about a backend, including its capabilities, supported
 * metrics, and available sampling/streaming rates.
 */
struct QcPerfBackendInfo {
    uint8_t backend_id;                             /**< Unique identifier for this backend */
    struct QcPerfCapabilityInfo* capabilities_list; /**< Array of capabilities provided by this backend */
    uint8_t capabilities_list_length;               /**< Number of capabilities in the array */
};
#endif  // QC_PERF_COMMON_H
