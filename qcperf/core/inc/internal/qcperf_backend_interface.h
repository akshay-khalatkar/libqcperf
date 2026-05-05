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
 * @file qcperf_backend_interface.h
 * @brief Backend interface definition for QcPerf library
 * @author Skand Gupta (skangupt@qti.qualcomm.com)
 *
 * This file defines the private backend interface structure that all QcPerf
 * backends must implement. It provides function pointers for backend lifecycle
 * management, performance request handling, and callback registration.
 */

#ifndef QC_PERF_BACKEND_INTERFACE_H
#define QC_PERF_BACKEND_INTERFACE_H

#include "qcperf_common.h"

/**
 * @struct QcPerfBackendPrivate
 * @brief Private backend interface structure containing function pointers
 *
 * This structure defines the interface that all QcPerf backends must implement.
 * Each backend provides implementations for initialization, deinitialization,
 * performance request handling, and callback management.
 */
struct QcPerfBackendPrivate {
    /**
     * @brief Set the message callback function
     * @param message_callback Function pointer to the message callback
     * @return QcPerfReturnCode indicating success or failure
     */
    enum QcPerfReturnCode (*set_message_callback)(QcPerfMessageCallback message_callback);

    /**
     * @brief Initialize the backend
     * @return QcPerfReturnCode indicating success or failure
     */
    enum QcPerfReturnCode (*qcperf_backend_init)(void);

    /**
     * @brief Get backend information
     * @param backend_info Pointer to structure to be filled with backend information
     * @return QcPerfReturnCode indicating success or failure
     */
    enum QcPerfReturnCode (*qcperf_backend_info)(struct QcPerfBackendInfo* backend_info);

    /**
     * @brief Set the result callback function
     * @param data_callback Function pointer to the result callback
     * @return QcPerfReturnCode indicating success or failure
     */
    enum QcPerfReturnCode (*set_data_callback)(QcPerfDataCallback data_callback);

    /**
     * @brief Start a performance request
     * @param request Pointer to the performance request structure
     * @return QcPerfReturnCode indicating success or failure
     */
    enum QcPerfReturnCode (*qcperf_backend_start)(struct QcPerfRequest* request);

    /**
     * @brief Stop a performance request
     * @param request Pointer to the performance request structure
     * @return QcPerfReturnCode indicating success or failure
     */
    enum QcPerfReturnCode (*qcperf_backend_stop)(struct QcPerfRequest* request);

    /**
     * @brief Deinitialize the backend
     * @return QcPerfReturnCode indicating success or failure
     */
    enum QcPerfReturnCode (*qcperf_backend_deinit)(void);
};

#endif  // QC_PERF_BACKEND_INTERFACE_H
