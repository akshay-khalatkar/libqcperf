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

#ifndef WOS_IOCTL_H
#define WOS_IOCTL_H

#include <stdbool.h>
#include <windows.h>

#define IS_IOCTL_BIT_ENABLED(bitmap, index) (((bitmap) >> (index)) & 1UL)

enum IoctlCommonReturnCode {
    RETURN_CODE_IOCTL_COMMON_SUCCESS = 0,
    RETURN_CODE_IOCTL_COMMON_GET_DEVICE_INTERFACE_LIST_SIZE_FAILED,
    RETURN_CODE_IOCTL_COMMON_DEVICE_NOT_AVAILABLE,
    RETURN_CODE_IOCTL_COMMON_SNPRINTF_INSUFFICIENT_BUFFER_COUNT,
    RETURN_CODE_IOCTL_COMMON_MEMORY_ALLOCATION_FAILED,
    RETURN_CODE_IOCTL_COMMON_CREATE_FILE_FAILED,
    RETURN_CODE_IOCTL_COMMON_NULL_INPUT,
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Populates a deviceHandle given a valid device GUID.
 * @param[in] guid The device GUID to search for
 * @param[out] deviceHandle Pointer to a HANDLE. Pointer must be allocated, but HANDLE
 * will be populated with the actual device handle. If HANDLE is already assigned (not NULL),
 * it will be closed and freed.
 * @return IOCTL_COMMON_RETURN_CODE_SUCCESS on success
 */
enum IoctlCommonReturnCode ioctl_get_device_handle(GUID guid, HANDLE* deviceHandle);

/**
 * @brief Best-effort to close the HANDLE inside the given pointer, and frees any corresponding memory.
 * @param[in,out] deviceHandle Pointer to the HANDLE to close.
 * @return IOCTL_COMMON_RETURN_CODE_SUCCESS on success
 */
enum IoctlCommonReturnCode ioctl_free_device_handle(HANDLE* deviceHandle);

#ifdef __cplusplus
}
#endif

#endif  // WOS_IOCTL_H
