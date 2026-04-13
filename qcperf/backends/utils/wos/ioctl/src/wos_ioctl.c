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

#include "wos_ioctl.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <float.h>
#include <cfgmgr32.h>

#define MAX_SYMBOLIC_LINK_LENGTH 256

struct SymbolicLinkRequest {
    GUID guid;
    uint8_t* symbolic_link;
    uint16_t symbolic_link_memory;
};

/**
 * @brief Helper function to get device symbolic link and populate input with it based on provided memory length.
 * It is assumed that both inputs are valid and non-NULL/non-zero. Callers of this function are responsible for checking.
 * @param[in,out] symbolic_link_request Pointer to symbolic link request structure
 * @return IOCTL_COMMON_RETURN_CODE_SUCCESS on success
 */
static enum IoctlCommonReturnCode ioctl_get_device_symbolic_link(struct SymbolicLinkRequest* symbolic_link_request) {
    enum IoctlCommonReturnCode return_code;
    CONFIGRET cr_status;
    uint8_t* device_interface_list;
    uint8_t* device_interface_list_copy;
    ULONG device_interface_list_length;
    ULONG snprintf_length;

    return_code                  = RETURN_CODE_IOCTL_COMMON_SUCCESS;
    cr_status                    = CR_SUCCESS;
    device_interface_list        = NULL;
    device_interface_list_copy   = NULL;
    device_interface_list_length = 0;
    snprintf_length              = 0;

    // CM_Get_Device_Interface_List_Size could be outdated once CM_Get_Device_Interface_List is called, retry as needed
    do {
        cr_status = CM_Get_Device_Interface_List_Size(&device_interface_list_length, (LPGUID) & (symbolic_link_request->guid), NULL, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

        if (cr_status != CR_SUCCESS) {
            return_code = RETURN_CODE_IOCTL_COMMON_GET_DEVICE_INTERFACE_LIST_SIZE_FAILED;
        } else {
            if (device_interface_list_length <= 1) {
                cr_status = CR_DEVICE_NOT_THERE;
            } else {
                device_interface_list_copy = device_interface_list;
                // Only reallocates memory when existing is not enough
                device_interface_list = (uint8_t*)realloc(device_interface_list, device_interface_list_length * sizeof(uint8_t));
                if (NULL == device_interface_list) {
                    if (NULL != device_interface_list_copy) {
                        free(device_interface_list_copy);
                        device_interface_list_copy = NULL;
                    }
                    cr_status   = CR_OUT_OF_MEMORY;
                    return_code = RETURN_CODE_IOCTL_COMMON_MEMORY_ALLOCATION_FAILED;
                } else {
                    // Ensure this doesn't point to garbage (no longer needed)
                    device_interface_list_copy = NULL;

                    cr_status =
                        CM_Get_Device_Interface_List((LPGUID) & (symbolic_link_request->guid), NULL, (PZZSTR)device_interface_list, device_interface_list_length, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

                    if (cr_status != CR_SUCCESS && cr_status != CR_BUFFER_SMALL) {
                        return_code = RETURN_CODE_IOCTL_COMMON_GET_DEVICE_INTERFACE_LIST_SIZE_FAILED;
                    }
                }
            }
        }
    } while (cr_status == CR_BUFFER_SMALL);

    if (cr_status == CR_DEVICE_NOT_THERE) {
        return_code = RETURN_CODE_IOCTL_COMMON_DEVICE_NOT_AVAILABLE;
    } else if (RETURN_CODE_IOCTL_COMMON_SUCCESS == return_code) {
        snprintf_length = snprintf((char*)symbolic_link_request->symbolic_link, symbolic_link_request->symbolic_link_memory, "%s", device_interface_list);
        if (snprintf_length >= symbolic_link_request->symbolic_link_memory) {
            return_code = RETURN_CODE_IOCTL_COMMON_SNPRINTF_INSUFFICIENT_BUFFER_COUNT;
        }
    }

    if (NULL != device_interface_list) {
        free(device_interface_list);
        device_interface_list = NULL;
    }

    return return_code;
}

enum IoctlCommonReturnCode ioctl_get_device_handle(GUID guid, HANDLE* device_handle) {
    enum IoctlCommonReturnCode return_code;
    DWORD error_number;
    struct SymbolicLinkRequest* symbolic_link_request;

    return_code           = RETURN_CODE_IOCTL_COMMON_SUCCESS;
    error_number          = ERROR_SUCCESS;
    symbolic_link_request = NULL;

    if (NULL == device_handle) {
        return_code = RETURN_CODE_IOCTL_COMMON_NULL_INPUT;
    } else {
        if (NULL != *device_handle) {
            ioctl_free_device_handle(device_handle);
        }

        symbolic_link_request = (struct SymbolicLinkRequest*)calloc(1, sizeof(struct SymbolicLinkRequest));
        if (NULL == symbolic_link_request) {
            return_code = RETURN_CODE_IOCTL_COMMON_MEMORY_ALLOCATION_FAILED;
        } else {
            symbolic_link_request->guid                 = guid;
            symbolic_link_request->symbolic_link_memory = MAX_SYMBOLIC_LINK_LENGTH;
            symbolic_link_request->symbolic_link        = (uint8_t*)calloc(symbolic_link_request->symbolic_link_memory, sizeof(uint8_t));
            if (NULL == symbolic_link_request->symbolic_link) {
                return_code = RETURN_CODE_IOCTL_COMMON_MEMORY_ALLOCATION_FAILED;
            } else {
                return_code = ioctl_get_device_symbolic_link(symbolic_link_request);

                if (RETURN_CODE_IOCTL_COMMON_SUCCESS == return_code) {
                    *device_handle = CreateFile((LPCSTR)symbolic_link_request->symbolic_link, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                    error_number   = GetLastError();
                    if (*device_handle == INVALID_HANDLE_VALUE) {
                        return_code = RETURN_CODE_IOCTL_COMMON_CREATE_FILE_FAILED;
                    }
                }
            }
        }
    }

    if (RETURN_CODE_IOCTL_COMMON_SUCCESS != return_code) {
        ioctl_free_device_handle(device_handle);
    }

    if (NULL != symbolic_link_request) {
        if (NULL != symbolic_link_request->symbolic_link) {
            free(symbolic_link_request->symbolic_link);
            symbolic_link_request->symbolic_link = NULL;
        }
        free(symbolic_link_request);
        symbolic_link_request = NULL;
    }

    return return_code;
}

enum IoctlCommonReturnCode ioctl_free_device_handle(HANDLE* device_handle) {
    enum IoctlCommonReturnCode return_code;
    bool close_handle_return_code;
    DWORD error_number;

    return_code              = RETURN_CODE_IOCTL_COMMON_SUCCESS;
    close_handle_return_code = true;
    error_number             = ERROR_SUCCESS;

    if (NULL == device_handle || NULL == *device_handle) {
        // No action needed
    } else {
        close_handle_return_code = CloseHandle(*device_handle);
        if (false == close_handle_return_code) {
            error_number = GetLastError();
        }
        *device_handle = NULL;
    }

    return return_code;
}
