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
 * @file wos_power_backend.h
 * @brief This header defines the create function for the WOS Power backend which is the entry point.
 * @author Sourav Sarkar (sousar@qti.qualcomm.com)
 * 
 * This header provides the public interface for the WoS (Windows on Snapdragon)
 * Power backend, which monitors power metrics. This backend interfaces with the 
 * Windows power telemetry to collect real-time power metrics.
 */

#ifndef WOS_POWER_BACKEND_H
#define WOS_POWER_BACKEND_H

#include "qcperf_common.h"
#include "qcperf_backend_interface.h"

/**
 * @brief Create a WOS Power backend instance.
 *
 * This function initializes the function pointers of the backend private
 * structure with the WOS Power backend's implementation.
 *
 * @param[out] backend A pointer to the backend private structure to be
 *                     populated.
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on success, or an error code on failure.
 */
enum QcPerfReturnCode wos_power_backend_create(struct QcPerfBackendPrivate *backend);

#endif // WOS_POWER_BACKEND_H
