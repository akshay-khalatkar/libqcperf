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
 * @file qtime.c
 * @brief Windows implementation of time utility functions
 * @author Skand Gupta (skangupt@qti.qualcomm.com)
 *
 * This file implements the time utility functions defined in qtime.h
 * for Windows platforms. It provides a cross-platform abstraction layer over
 * the Windows high-resolution timing APIs.
 *
 * The implementation uses Windows API functions like QueryPerformanceCounter and
 * QueryPerformanceFrequency to provide high-precision timing with nanosecond resolution
 * for performance measurements and benchmarking.
 */

#include <stdint.h>
#include <windows.h>
#include "qtime.h"

enum QTimeReturnCode get_time_ns(uint64_t* time_ns) {
    enum QTimeReturnCode return_code = RETURN_CODE_TIME_FAILED;
    LARGE_INTEGER counter, frequency;
    if (time_ns == NULL) {
        return_code = RETURN_CODE_TIME_NULL_POINTER;
    } else {
        QueryPerformanceCounter(&counter);
        QueryPerformanceFrequency(&frequency);

        if (counter.QuadPart < 0 || frequency.QuadPart <= 0) {
            return_code = RETURN_CODE_TIME_INVALID_VALUE;
        } else {
            // Convert to nanoseconds
            // (counter * 1,000,000,000) / frequency
            *time_ns    = (uint64_t)((counter.QuadPart * 1000000000ULL) / frequency.QuadPart);
            return_code = RETURN_CODE_TIME_SUCCESS;
        }
    }
    return return_code;
}
