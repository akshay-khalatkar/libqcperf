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
 * @file qcpep_power_ioctl.h
 * @brief IOCTL interface definitions for the qcpep8380 power driver (V1 hardware).
 *
 * Qualcomm Power Engine Plug-in Device (ACPI\VEN_QCOM&DEV_0C17, qcpep8380.sys)
 * on Snapdragon 8380 (V1). The standard GUID_DEVICE_ENERGY_METER path returns
 * GLE=1 on V1 firmware, so these proprietary IOCTLs are used instead.
 *
 * Protocol (all IOCTLs: FILE_DEVICE_UNKNOWN, METHOD_BUFFERED, FILE_READ_DATA):
 *   func=0x000  IOCTL_QCPEP_GET_VERSION       -> uint16 driver API version (= 2)
 *   func=0x001  IOCTL_QCPEP_GET_CAPS_SIZE     -> uint32 byte size of caps buffer
 *   func=0x002  IOCTL_QCPEP_GET_CAPABILITIES  -> QcPepCapabilities (variable length)
 *   func=0x003  IOCTL_QCPEP_GET_MEASUREMENTS  -> QcPepRailMeasurement[numRails]
 *
 * Power calculation (two consecutive GET_MEASUREMENTS calls):
 *   power_W = (energy2 - energy1) / (filetimeTick2 - filetimeTick1)
 *   (energy in 100 nJ units, FILETIME in 100 ns units, factors cancel to Watts)
 */

#ifndef QCPEP_POWER_IOCTL_H
#define QCPEP_POWER_IOCTL_H

#include <windows.h>
#include <initguid.h>
#include <stdint.h>

DEFINE_GUID(GUID_QCPEP_POWER_DEVICE_INTERFACE,
    0x09195dae, 0x6bd7, 0x4a9c, 0x8f, 0x3b, 0xc8, 0x35, 0x87, 0x3d, 0x4d, 0xae);

#define IOCTL_QCPEP_GET_VERSION      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x000, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_QCPEP_GET_CAPS_SIZE    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x001, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_QCPEP_GET_CAPABILITIES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x002, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_QCPEP_GET_MEASUREMENTS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x003, METHOD_BUFFERED, FILE_READ_DATA)

#define QCPEP_CAPS_BUFFER_MAX      512
#define QCPEP_RAIL_INDEX_UNMATCHED 0xFF

/*
 * Fixed header of IOCTL_QCPEP_GET_CAPABILITIES response.
 * Followed by numRails variable-length entries:
 *   { uint16_t nameBytes; WCHAR name[nameBytes/2]; <padding> }
 * Use IOCTL_QCPEP_GET_CAPS_SIZE to allocate the correct buffer size.
 */
struct QcPepCapabilities {
    WCHAR    vendor[16];
    WCHAR    model[16];
    uint16_t version;
    uint16_t numRails;
    uint32_t reserved;
};

struct QcPepRailMeasurement {
    uint64_t energy100nJ;
    uint64_t filetimeTick;
};

#endif  // QCPEP_POWER_IOCTL_H
