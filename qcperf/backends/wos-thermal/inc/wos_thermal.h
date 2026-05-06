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
 * @file wos_thermal.h
 * @brief WOS Thermal backend interface for libqcperf
 * @author Skand Gupta (skangupt@qti.qualcomm.com)
 *
 * This header provides the public interface for the WOS (Windows on Snapdragon)
 * Thermal backend, which monitors temperature and passive cooling metrics for
 * various thermal zones. This backend interfaces with the Windows thermal
 * monitoring subsystem to collect real-time temperature data and passive
 * cooling states.
 */

#ifndef WOS_THERMAL_H
#define WOS_THERMAL_H

#include "wos_thermal_info.h"

struct QcPerfBackendPrivate;

/**
 * @brief Create and initialize the WOS Thermal backend
 *
 * This function configures the WOS Thermal backend by populating the backend
 * private structure with function pointers for all required backend
 * operations including initialization, start, stop, deinitialization,
 * and callback registration. The backend provides temperature and passive cooling
 * monitoring for various thermal zones.
 *
 * @param[in,out] backend Pointer to backend private structure to be configured
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on successful creation
 * @return QC_PERF_RETURN_CODE_NULL_POINTER if backend pointer is NULL
 */
enum QcPerfReturnCode qcperf_wos_thermal_backend_create(struct QcPerfBackendPrivate* backend);

#endif  // WOS_THERMAL_H
