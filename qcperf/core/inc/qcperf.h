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
 * @file qcperf.h
 * @brief QCPerf Library Main API Header
 *
 * This header file defines the main API for the QCPerf (Qualcomm Performance) library.
 * The library provides a unified interface for performance monitoring and profiling
 * across different backend implementations.
 *
 */

#ifndef QC_PERF_H  // QC_PERF_H
#define QC_PERF_H

#include "qcperf_common.h"
#include "version_info.h"


/**
 * @brief Initialize the QCPerf library
 *
 * This function must be called before any other QCPerf API functions.
 * It initializes internal data structures and prepares the library for use.
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on successful initialization
 * @return QC_PERF_RETURN_CODE_ALREADY_INITIALIZED if already initialized
 * @return QC_PERF_RETURN_CODE_FAILED on initialization failure
 *
 * @note This function should only be called once during the application lifecycle
 * @see qcperf_deinit()
 */
enum QcPerfReturnCode qcperf_init(void);

/**
 * @brief Get the version information of the QCPerf library
 *
 * Retrieves the current version information of the QCPerf library.
 *
 * @param[out] version_info Pointer to a QcPerfVersionInfo structure that will be populated
 *                          with the version information
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_NULL_POINTER if version_info is NULL
 *
 * @see struct QcPerfVersionInfo
 */
enum QcPerfReturnCode qcperf_version(struct QcPerfVersionInfo* version_info);

/**
 * @brief Connect to a specific performance monitoring backend
 *
 * Establishes a connection to the specified backend and optionally registers
 * a message callback function for receiving backend messages.
 *
 * @param[in] backend_id The identifier of the backend to connect to (e.g., QC_PERF_BACKEND_DUMMY)
 * @param[in] message_callback Optional callback function for receiving messages from the backend.
 *                             Can be NULL if message callbacks are not needed.
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on successful connection
 * @return QC_PERF_RETURN_CODE_NOT_INITIALIZED if library not initialized
 * @return QC_PERF_RETURN_CODE_INVALID_ARGUMENTS if backend_id is invalid
 * @return QC_PERF_RETURN_CODE_FAILED on connection failure
 *
 * @note The backend must be disconnected using qcperf_disconnect_backend() when no longer needed
 * @see qcperf_disconnect_backend()
 */
enum QcPerfReturnCode qcperf_connect_backend(enum QcPerfBackendId backend_id, QcPerfMessageCallback message_callback);

/**
 * @brief Get capabilities information from a connected backend
 *
 * Retrieves detailed information about the capabilities, metrics, and supported
 * sampling/streaming rates of the specified backend.
 *
 * @param[in] backend_id The identifier of the backend to query
 * @param[out] backend_info Pointer to a QcPerfBackendInfo structure that will be populated
 *                          with the backend's capabilities information
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_NOT_INITIALIZED if library not initialized
 * @return QC_PERF_RETURN_CODE_INVALID_ARGUMENTS if backend_id is invalid
 * @return QC_PERF_RETURN_CODE_NULL_POINTER if backend_info is NULL
 * @return QC_PERF_RETURN_CODE_FAILED on failure
 *
 * @note The backend must be connected before calling this function
 * @see qcperf_connect_backend(), struct QcPerfBackendInfo
 */
enum QcPerfReturnCode qcperf_get_capabilities_info(enum QcPerfBackendId backend_id, struct QcPerfBackendInfo* backend_info);

/**
 * @brief Set the result callback for a connected backend
 *
 * Registers a callback function that will be invoked when the backend produces
 * performance monitoring results. This is the primary mechanism for receiving
 * metric data from the backend.
 *
 * @param[in] backend_id The identifier of the backend
 * @param[in] data_callback The callback function to register for receiving results.
 *                            Must not be NULL.
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_NOT_INITIALIZED if library not initialized
 * @return QC_PERF_RETURN_CODE_INVALID_ARGUMENTS if backend_id is invalid
 * @return QC_PERF_RETURN_CODE_NULL_POINTER if data_callback is NULL
 * @return QC_PERF_RETURN_CODE_FAILED on failure
 *
 * @note The backend must be connected before calling this function
 * @note This callback must be set before starting performance monitoring
 * @see qcperf_start(), QcPerfDataCallback, struct QcPerfData
 */
enum QcPerfReturnCode qcperf_set_data_callback(enum QcPerfBackendId backend_id, QcPerfDataCallback data_callback);

/**
 * @brief Start performance monitoring on a backend
 *
 * Initiates performance monitoring for the specified capability with the given
 * sampling and streaming rates. Results will be delivered via the registered
 * result callback.
 *
 * @param[in] backend_id The identifier of the backend
 * @param[in] request Pointer to a QcPerfRequest structure containing:
 *                    - capability_id: The capability to monitor
 *                    - streaming_rate: Rate at which data is streamed (in milliseconds)
 *                    - sampling_rate: Rate at which data is sampled (in milliseconds)
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_NOT_INITIALIZED if library not initialized
 * @return QC_PERF_RETURN_CODE_INVALID_ARGUMENTS if backend_id or request parameters are invalid
 * @return QC_PERF_RETURN_CODE_NULL_POINTER if request is NULL
 * @return QC_PERF_RETURN_CODE_CAPABILITY_NOT_FOUND if the capability is not supported
 * @return QC_PERF_RETURN_CODE_FAILED on failure
 *
 * @note A result callback must be set before calling this function
 * @note Use qcperf_stop() to stop the monitoring session
 * @see qcperf_stop(), qcperf_set_data_callback(), struct QcPerfRequest
 */
enum QcPerfReturnCode qcperf_start(enum QcPerfBackendId backend_id, struct QcPerfRequest* request);

/**
 * @brief Stop performance monitoring on a backend
 *
 * Stops an active performance monitoring session for the specified capability.
 *
 * @param[in] backend_id The identifier of the backend
 * @param[in] request Pointer to a QcPerfRequest structure identifying the monitoring
 *                    session to stop (must match the request used in qcperf_start())
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_NOT_INITIALIZED if library not initialized
 * @return QC_PERF_RETURN_CODE_INVALID_ARGUMENTS if backend_id or request parameters are invalid
 * @return QC_PERF_RETURN_CODE_NULL_POINTER if request is NULL
 * @return QC_PERF_RETURN_CODE_FAILED on failure
 *
 * @note This function should be called for each active monitoring session started with qcperf_start()
 * @see qcperf_start(), struct QcPerfRequest
 */
enum QcPerfReturnCode qcperf_stop(enum QcPerfBackendId backend_id, struct QcPerfRequest* request);

/**
 * @brief Disconnect from a backend
 *
 * Closes the connection to the specified backend and releases associated resources.
 * All active monitoring sessions on this backend will be stopped.
 *
 * @param[in] backend_id The identifier of the backend to disconnect from
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_NOT_INITIALIZED if library not initialized
 * @return QC_PERF_RETURN_CODE_INVALID_ARGUMENTS if backend_id is invalid
 * @return QC_PERF_RETURN_CODE_FAILED on failure
 *
 * @note All monitoring sessions should be stopped before disconnecting
 * @see qcperf_connect_backend(), qcperf_stop()
 */
enum QcPerfReturnCode qcperf_disconnect_backend(enum QcPerfBackendId backend_id);

/**
 * @brief Deinitialize the QCPerf library
 *
 * Cleans up all resources used by the QCPerf library. This function should be
 * called when the library is no longer needed, typically at application shutdown.
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_NOT_INITIALIZED if library not initialized
 * @return QC_PERF_RETURN_CODE_FAILED on failure
 *
 * @note All backends should be disconnected before calling this function
 * @note After calling this function, qcperf_init() must be called again to use the library
 * @see qcperf_init(), qcperf_disconnect_backend()
 */
enum QcPerfReturnCode qcperf_deinit(void);

/**
 * @brief Get detailed information about an error code
 *
 * Retrieves human-readable information about a specific error code, including
 * the error type (info/warning/error) and a descriptive message.
 *
 * @param[in] error_code The error code to query
 * @param[out] error_info Pointer to a QcPerfReturnCodeInfo structure that will be populated
 *                        with detailed error information
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_NULL_POINTER if error_info is NULL
 * @return QC_PERF_RETURN_CODE_INVALID_ARGUMENTS if error_code is invalid
 * @return QC_PERF_RETURN_CODE_FAILED on failure
 *
 * @note This function can be called even if the library is not initialized
 * @see enum QcPerfReturnCode, struct QcPerfReturnCodeInfo
 */
enum QcPerfReturnCode qcperf_get_error_info(enum QcPerfReturnCode error_code, struct QcPerfReturnCodeInfo* error_info);

#endif  // QC_PERF_H
