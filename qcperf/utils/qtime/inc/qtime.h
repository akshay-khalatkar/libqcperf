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
 * @file qtime.h
 * @brief Time utility to get time for cross-platform
 * @author Skand Gupta (skangupt@qti.qualcomm.com)
 */

#include <stdint.h>

 enum QTimeReturnCode {
    RETURN_CODE_TIME_SUCCESS = 0,
    RETURN_CODE_TIME_FAILED,
    RETURN_CODE_TIME_NULL_POINTER,
    RETURN_CODE_TIME_INVALID_VALUE,
 };

/**
 * @brief Get current time in nanoseconds
 *
 * This function uses QueryPerformanceCounter and QueryPerformanceFrequency
 * to get the current time in nanoseconds. It provides high-precision timing
 * for performance measurements with nanosecond resolution.
 *
 * @param[out] time_ns Pointer to store the current time in nanoseconds
 *
 * @return RETURN_CODE_TIME_SUCCESS on success
 * @return RETURN_CODE_TIME_NULL_POINTER if time_ns is NULL
 * @return RETURN_CODE_TIME_INVALID_VALUE if counter or frequency values are invalid
 * @return RETURN_CODE_TIME_FAILED if the system's high-resolution timer fails
 */
enum QTimeReturnCode get_time_ns(uint64_t* time_ns);
