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
 * @file wos_thermal_logger.c
 * @brief Implementation of the WOS Thermal logger functionality
 * @author Skand Gupta (skangupt@qti.qualcomm.com)
 *
 * This file implements the logging functionality for the WOS Thermal backend.
 * It provides functions for setting a message callback and sending formatted
 * messages to the registered callback. These messages can include informational,
 * warning, or error messages related to the WOS Thermal backend's operation.
 *
 * The implementation uses a simple callback mechanism to deliver messages to
 * the application, allowing for flexible handling of log messages based on
 * their severity level.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "wos_thermal_logger.h"
#include "qcperf_backend_enum.h"

static QcPerfMessageCallback g_message_callback = NULL;

void wos_thermal_logger_set_message_callback(QcPerfMessageCallback message_callback) { g_message_callback = message_callback; }

void wos_thermal_logger_send_message(enum QcPerfMessageLevel level, const char* format, ...) {
    if (NULL != g_message_callback) {
        struct QcPerfMessage message = {0};
        va_list args;
        char buffer[MESSAGE_LEVEL_LENGTH] = {0};

        va_start(args, format);
        vsnprintf(buffer, MESSAGE_LEVEL_LENGTH, format, args);
        va_end(args);

        message.backend_id     = QC_PERF_BACKEND_THERMAL;
        message.message        = buffer;
        message.message_length = strlen(buffer);
        message.message_level  = level;

        g_message_callback(&message);
    }
}
