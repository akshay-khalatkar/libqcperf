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
 * @file cdsprpc_stub.c
 * @brief Build-time stub for libcdsprpc.so — satisfies the linker; real library loaded at runtime
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 */
#include <stdio.h>
#include "remote.h"
#include "rpcmem.h"

/* remote_handle64 API ---------------------------------------------------- */

int remote_handle64_open(const char *name, remote_handle64 *ph)
{
    (void)name; (void)ph;
    return -1;
}

int remote_handle64_invoke(remote_handle64 h, uint32_t dwScalars, remote_arg *pra)
{
    (void)h; (void)dwScalars; (void)pra;
    return -1;
}

int remote_handle64_close(remote_handle64 h)
{
    (void)h;
    return -1;
}

// /* rpcmem API ------------------------------------------------------------- */

void *rpcmem_alloc(int heapid, uint32 flags, int size)
{
    (void)heapid; (void)flags; (void)size;
    return NULL;
}

void rpcmem_free(void *po)
{
    (void)po;
}
