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
 * @file qcom_linux_cpu.h
 * @brief Qualcomm Linux CPU backend interface for libqcperf
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 *
 * This header provides the public interface for the qcom-linux-cpu backend,
 * which monitors CPU performance metrics on Qualcomm Linux platforms.
 * It also defines capability configuration and streaming/sampling rates.
 */

#ifndef QCOM_LINUX_CPU_H
#define QCOM_LINUX_CPU_H

#include "qcom_linux_cpu_info.h"

struct QcPerfBackendPrivate;

/**
 * @brief Create and initialize the qcom-linux-cpu backend
 *
 * This function configures the qcom-linux-cpu backend by populating the backend
 * private structure with function pointers for all required backend
 * operations including initialization, start, stop, deinitialization,
 * and callback registration.
 *
 * @param[in,out] backend Pointer to backend private structure to be configured
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on successful creation
 * @return QC_PERF_RETURN_CODE_NULL_POINTER if backend pointer is NULL
 */
enum QcPerfReturnCode qcperf_qcom_linux_cpu_create(struct QcPerfBackendPrivate* backend);

#endif /* QCOM_LINUX_CPU_H */
