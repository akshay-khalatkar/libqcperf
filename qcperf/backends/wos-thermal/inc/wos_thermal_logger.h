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
 * @file wos_thermal_logger.h
 * @brief Logging utilities for WOS Thermal backend
 * @author Skand Gupta (skangupt@qti.qualcomm.com)
 *
 * This header provides logging utilities for the WOS (Windows on Snapdragon)
 * Thermal backend. It defines functions and macros for sending log messages
 * with different severity levels to the application through a callback mechanism.
 */

#ifndef WOS_THERMAL_LOGGER_H
#define WOS_THERMAL_LOGGER_H

#include "qcperf_common.h"

/**
 * @def MESSAGE_LEVEL_LENGTH
 * @brief Maximum length of message buffer
 *
 * Defines the maximum buffer size for storing log messages.
 */
#define MESSAGE_LEVEL_LENGTH 256

/**
 * @def SEND_MESSAGE
 * @brief Macro for sending log messages
 *
 * Convenience macro for calling wos_thermal_logger_send_message with the
 * specified level and format string.
 */
#define SEND_MESSAGE(level, format, ...) wos_thermal_logger_send_message(level, format, ##__VA_ARGS__)

/**
 * @brief Set the message callback function
 *
 * This function sets the callback function that will be used to deliver
 * log messages to the application.
 *
 * @param message_callback Pointer to the message callback function
 */
void wos_thermal_logger_set_message_callback(QcPerfMessageCallback message_callback);

/**
 * @brief Send a log message
 *
 * This function formats and sends a log message using the registered
 * message callback function.
 *
 * @param level Severity level of the message
 * @param format Format string for the message
 * @param ... Variable arguments for the format string
 */
void wos_thermal_logger_send_message(enum QcPerfMessageLevel level, const char* format, ...);

#endif /* WOS_THERMAL_LOGGER_H */
