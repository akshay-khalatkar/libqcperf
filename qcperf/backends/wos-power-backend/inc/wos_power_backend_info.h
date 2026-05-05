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
 * @file wos_power_backend_info.h
 * @brief Static metric definitions for the WoS Power backend.
 * @author Sourav Sarkar (sousar@qti.qualcomm.com)
 *
 * This header defines all metric IDs, names, descriptions, and units for the
 * WoS Power backend's capabilities. It also provides initialization functions
 * to populate metric information structures with these static definitions.
 */

#ifndef WOS_POWER_BACKEND_INFO_H
#define WOS_POWER_BACKEND_INFO_H

#include "qcperf_common.h"

#define WOS_POWER_CAPABILITY_1_ID 0x0
#define WOS_POWER_CAPABILITY_1_INDEX 0

#define WOS_POWER_METRIC_0_ID 0
#define WOS_POWER_METRIC_0_NAME "CPU Cluster 0 Power"
#define WOS_POWER_METRIC_0_DESCRIPTION "CPU Cluster 0 power average over sampling period"
#define WOS_POWER_METRIC_0_UNIT "mW"

#define WOS_POWER_METRIC_1_ID 1
#define WOS_POWER_METRIC_1_NAME "CPU Cluster 1 Power"
#define WOS_POWER_METRIC_1_DESCRIPTION "CPU Cluster 1 power average over sampling period"
#define WOS_POWER_METRIC_1_UNIT "mW"

#define WOS_POWER_METRIC_2_ID 2
#define WOS_POWER_METRIC_2_NAME "CPU Cluster 2 Power"
#define WOS_POWER_METRIC_2_DESCRIPTION "CPU Cluster 2 power average over sampling period"
#define WOS_POWER_METRIC_2_UNIT "mW"

#define WOS_POWER_METRIC_3_ID 3
#define WOS_POWER_METRIC_3_NAME "GPU Power"
#define WOS_POWER_METRIC_3_DESCRIPTION "GPU power average over sampling period"
#define WOS_POWER_METRIC_3_UNIT "mW"

#define WOS_POWER_METRIC_4_ID 4
#define WOS_POWER_METRIC_4_NAME "PSU USB Power"
#define WOS_POWER_METRIC_4_DESCRIPTION "PSU USB power average over sampling period"
#define WOS_POWER_METRIC_4_UNIT "mW"

/* Same metric info for POWER_TELEMETRY_RAIL_SYS and POWER_TELEMETRY_RAIL_SYSTEM*/
#define WOS_POWER_METRIC_5_ID 5
#define WOS_POWER_METRIC_5_NAME "System Power"
#define WOS_POWER_METRIC_5_DESCRIPTION "System power average over sampling period"
#define WOS_POWER_METRIC_5_UNIT "mW"

#define WOS_POWER_METRIC_6_ID 6
#define WOS_POWER_METRIC_6_NAME "USBC Total Power"
#define WOS_POWER_METRIC_6_DESCRIPTION "USBC Total power average over sampling period"
#define WOS_POWER_METRIC_6_UNIT "mW"

#define WOS_POWER_METRIC_7_ID 7
#define WOS_POWER_METRIC_7_NAME "ROP Power"
#define WOS_POWER_METRIC_7_DESCRIPTION "Rest of Platform power average over sampling period"
#define WOS_POWER_METRIC_7_UNIT "mW"

#define WOS_POWER_METRIC_8_ID 8
#define WOS_POWER_METRIC_8_NAME "NSP Power"
#define WOS_POWER_METRIC_8_DESCRIPTION "NSP Total power average over sampling period"
#define WOS_POWER_METRIC_8_UNIT "mW"

#define WOS_POWER_METRIC_9_ID 9
#define WOS_POWER_METRIC_9_NAME "Multimedia Power"
#define WOS_POWER_METRIC_9_DESCRIPTION "Multimedia Total power average over sampling period"
#define WOS_POWER_METRIC_9_UNIT "mW"

#define WOS_POWER_METRIC_10_ID 10
#define WOS_POWER_METRIC_10_NAME "INFRA Power"
#define WOS_POWER_METRIC_10_DESCRIPTION "INFRA Total power average over sampling period"
#define WOS_POWER_METRIC_10_UNIT "mW"

#define WOS_POWER_METRIC_11_ID 11
#define WOS_POWER_METRIC_11_NAME "DRAM/DDR Power"
#define WOS_POWER_METRIC_11_DESCRIPTION "DRAM/DDR Total power average over sampling period"
#define WOS_POWER_METRIC_11_UNIT "mW"

#define WOS_POWER_METRIC_12_ID 12
#define WOS_POWER_METRIC_12_NAME "SOC Power"
#define WOS_POWER_METRIC_12_DESCRIPTION "SOC Total power average over sampling period"
#define WOS_POWER_METRIC_12_UNIT "mW"


#endif /* WOS_POWER_BACKEND_INFO_H */
