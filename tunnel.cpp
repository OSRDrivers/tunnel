//
// Copyright 2019 OSR Open Systems Resources, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE
// 
#include "stdafx.h"


int wmain(int argc, wchar_t *argv[])
{

    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD shortNameChars;
    PWCHAR shortName = NULL;

    if (argc != 2) {
        printf("Usage - %ls <path to long name>\n", argv[0]);
        goto Exit;
    }

    //
    // Create a file with a long name
    //
    printf("Creating long name %ls\n", argv[1]);

    hFile = CreateFile(argv[1],
                       DELETE, 
                       FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       NULL,
                       CREATE_NEW,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Create failed %d\n", GetLastError());
        goto Exit;
    }

    //
    // Don't need this anymore
    //
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    //
    // Get the short name
    //
    shortNameChars = GetShortPathNameW(argv[1],
                                       NULL,
                                       0);

    if (shortNameChars == 0) {
        printf("GetShortPathNameW failed %d\n", GetLastError());
        goto Exit;
    }

    shortName = (PWCHAR)malloc(shortNameChars * sizeof(WCHAR));

    if (shortName == NULL) {
        printf("malloc failed for %d chars\n", shortNameChars);
        goto Exit;
    }

    shortNameChars = GetShortPathNameW(argv[1],
                                       shortName,
                                       shortNameChars);

    if (shortNameChars == 0) {
        printf("GetShortPathNameW (2) failed %d\n", GetLastError());
        goto Exit;
    }

    printf("Deleting short name %ls\n", shortName);

    if (!DeleteFileW(shortName)) {
        printf("DeleteFileW failed %d\n", GetLastError());
        goto Exit;
    }

    //
    // Make sure the long name is gone
    //
    if (GetFileAttributesW(argv[1]) != INVALID_FILE_ATTRIBUTES) {
        printf("Opened long name - Delete didn't work??\n");
        goto Exit;
    }

    printf("Recreating short name %ls\n", shortName);

    //
    // Now recreate using the short name
    //
    hFile = CreateFile(shortName,
                       DELETE, 
                       FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                       NULL,
                       CREATE_NEW,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Create failed %d\n", GetLastError());
        goto Exit;
    }

    //
    // Don't need this anymore
    //
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    //
    // Make sure the long name is back
    //
    if (GetFileAttributesW(argv[1]) == INVALID_FILE_ATTRIBUTES) {
        printf("Failed to open long name - Not tunneled! %d\n", GetLastError());
        goto Exit;
    }

    printf("Long name is back! Name was tunneled!\n");


Exit:
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
    }
    if (shortName != NULL) {
        free(shortName);
    }
    return 0;
}

