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
 * @file wos_power_backend_logger.c
 * @brief Logger utility for the WOS Power backend.
 * @author Sourav Sarkar (sousar@qti.qualcomm.com)
 *
 * This header provides a simple logging mechanism for the backend. It allows
 * setting a message callback and provides a macro to conditionally send
 * log messages.
 */

#include <stdarg.h>
#include <stdio.h>

#include "wos_power_backend_logger.h"

/**
 * @brief Set the message callback for the logger.
 *
 * @param[in] message_callback The function pointer to be used for logging.
 */
void wos_power_backend_logger_set_message_callback(QcPerfMessageCallback message_callback) {
    g_message_callback = message_callback;
}

/**
 * @brief Log a message using the registered callback.
 *
 * This function formats a message and sends it to the registered message
 * callback, if one is set.
 *
 * @param[in] level The message level.
 * @param[in] fmt The format string for the message.
 * @param[in] ... Additional arguments for the format string.
 */
void log_message(enum QcPerfMessageLevel level, const char *fmt, ...) {
    char msg[ERROR_STRING_MAX_LEN];
    va_list args;
    int len;

    if (NULL != g_message_callback) {
        va_start(args, fmt);
        len = vsnprintf(msg, sizeof(msg), fmt, args);
        va_end(args);

        if (len > 0) {
            if (len >= ERROR_STRING_MAX_LEN) {
                len = ERROR_STRING_MAX_LEN - 1;
            }

            struct QcPerfMessage msg_struct = {
                .message = msg,
                .message_length = (size_t)len,
                .message_level = level
            };
            g_message_callback(&msg_struct);
        }
    }
}
