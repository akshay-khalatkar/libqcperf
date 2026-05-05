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
 * @file dummy_info.c
 * @brief Metric initialization implementation for dummy backend
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 * This file implements the initialization functions that populate metric
 * information structures with static definitions for the dummy backend's
 * two capabilities.
 */

#include <stdio.h>
#include <string.h>
#include "dummy_info.h"

/**
 * @brief Initialize Capability 0 metrics data
 *
 * This function populates the metric information for Capability 0
 * using the macro definitions from dummy_info.h
 */
void dummy_capability_0_init_metrics(struct QcPerfMetricInfo *metrics_data) {
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_id = DUMMY_METRIC_0_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_0_NAME);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_0_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_0_UNIT);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_0].metric_unit);

    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_id = DUMMY_METRIC_1_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_1_NAME);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_1_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_1_UNIT);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_1].metric_unit);

    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_id = DUMMY_METRIC_2_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_2_NAME);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_2_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_2_UNIT);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_2].metric_unit);

    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_id = DUMMY_METRIC_3_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_3_NAME);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_3_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_3_UNIT);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_3].metric_unit);

    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_id = DUMMY_METRIC_4_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_4_NAME);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_4_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_4_UNIT);
    metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_0_METRIC_INDEX_4].metric_unit);
}

/**
 * @brief Initialize Capability 1 metrics data
 *
 * This function populates the metric information for Capability 1
 * using the macro definitions from dummy_info.h
 */
void dummy_capability_1_init_metrics(struct QcPerfMetricInfo *metrics_data) {
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_id = DUMMY_METRIC_0_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_0_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_0_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_0_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_0].metric_unit);

    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_id = DUMMY_METRIC_1_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_1_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_1_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_1_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_1].metric_unit);

    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_id = DUMMY_METRIC_2_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_2_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_2_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_2_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_2].metric_unit);

    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_id = DUMMY_METRIC_3_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_3_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_3_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_3_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_3].metric_unit);

    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_id = DUMMY_METRIC_4_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_4_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_4_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_4_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_4].metric_unit);

    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_id = DUMMY_METRIC_5_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_5_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_5_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_5_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_5].metric_unit);

    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_id = DUMMY_METRIC_6_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_6_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_6_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_6_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_6].metric_unit);

    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_id = DUMMY_METRIC_7_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_7_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_7_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_7_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_7].metric_unit);

    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_id = DUMMY_METRIC_8_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_8_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_8_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_8_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_8].metric_unit);

    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_id = DUMMY_METRIC_9_ID;
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_name, METRIC_NAME_MAX_LEN, "%s", DUMMY_METRIC_9_NAME);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_name_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_name);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_description, MAX_METRIC_DESCRIPTION_LEN, "%s", DUMMY_METRIC_9_DESCRIPTION);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_description_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_description);
    snprintf(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_unit, MAX_METRIC_UNIT_LEN, "%s", DUMMY_METRIC_9_UNIT);
    metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_unit_len = strlen(metrics_data[DUMMY_CAPABILITY_1_METRIC_INDEX_9].metric_unit);
}
