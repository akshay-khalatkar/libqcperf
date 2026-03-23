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
 * @file wos_power_backend_logger.h
 * @brief Logger utility for the WOS Power backend.
 * @author Sourav Sarkar (sousar@qti.qualcomm.com)
 *
 * This header provides the interface for a simple logging mechanism for the backend.
 */

#ifndef WOS_POWER_BACKEND_LOGGER_H
#define WOS_POWER_BACKEND_LOGGER_H

#include "qcperf_common.h"

QcPerfMessageCallback g_message_callback;

/**
 * @brief Set the message callback for the logger.
 *
 * @param[in] message_callback The function pointer to be used for logging.
 */
void wos_power_backend_logger_set_message_callback(QcPerfMessageCallback message_callback);

/**
 * @brief Macro to conditionally log messages if a message callback is set.
 *
 * This macro checks if a message callback is available before calling the
 * log_message function. If the callback is not set, the call is skipped.
 *
 * @param level The message level (e.g., QC_PERF_MESSAGE_LEVEL_INFO).
 * @param ... The format string and arguments.
 */
#define SEND_MESSAGE(level, ...) \
    if (g_message_callback != NULL) { \
            log_message(level, __VA_ARGS__); \
    }

#endif // WOS_POWER_BACKEND_LOGGER_H
