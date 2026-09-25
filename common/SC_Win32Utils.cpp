/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifdef _WIN32

#    include "SC_Win32Utils.h"

#    include <cstdio>
#    include <cstring>
#    include <cstdlib>
#    include <sys/timeb.h>
#    include <time.h>
#    include <windows.h>
#    include <shlobj.h>

void win32_ReplaceCharInString(char* string, int len, char src, char dst) {
    for (int i = 0; i < len; i++)
        if (string[i] == src)
            string[i] = dst;
}

void win32_ExtractContainingFolder(char* folder, const char* pattern, int maxChars) {
    strcpy(folder, pattern);
    bool backSlashFound = false;
    int pathLen = strlen(pattern);
    for (int i = pathLen - 2; i >= 0; --i) {
        if (pattern[i] == '\\') {
            folder[i + 1] = 0;
            backSlashFound = true;
            break;
        }
    }
    if (!backSlashFound)
        folder[0] = 0;
}

char* win32_basename(char* path) {
    int pathLen = strlen(path);
    int lastPathSepFoundPos = -1;
    int pos = 0;
    while (path[pathLen - 1] == '\\' || path[pathLen - 1] == '/') {
        path[pathLen - 1] = 0;
        pathLen--;
    };
    while (path[pos] != 0) {
        if (path[pos] == '\\' || path[pos] == '/') {
            lastPathSepFoundPos = pos;
        }
        pos++;
    }
    if (lastPathSepFoundPos == -1)
        return path;
    else
        return path + lastPathSepFoundPos + 1;
}

char* win32_dirname(char* path) {
    int pathLen = strlen(path);
    int lastPathSepFoundPos = -1;
    int pos = 0;
    while (path[pathLen - 1] == '\\' || path[pathLen - 1] == '/') {
        path[pathLen - 1] = 0;
        pathLen--;
    };
    while (path[pos] != 0) {
        if (path[pos] == '\\' || path[pos] == '/') {
            lastPathSepFoundPos = pos;
        }
        pos++;
    }
    if (lastPathSepFoundPos != -1)
        path[lastPathSepFoundPos] = 0;

    return path;
}

/* Based on code from PostgreSQL (pgsql/src/port/pipe.c)
 *  This is a replacement version of pipe for Win32 which allows
 *  returned handles to be used in select(). Note that read/write calls
 *  must be replaced with recv/send.
 */
int win32_pipe(int handles[2]) {
    SOCKET s;
    struct sockaddr_in serv_addr;
    int len = sizeof(serv_addr);
    handles[0] = handles[1] = INVALID_SOCKET;

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("win32_pipe failed to create socket: %ui", WSAGetLastError());
        return -1;
    }
    memset((void*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(0);
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(s, (SOCKADDR*)&serv_addr, len) == SOCKET_ERROR) {
        printf("win32_pipe failed to bind: %ui", WSAGetLastError());
        return -1;
    }
    if (listen(s, 1) == SOCKET_ERROR) {
        printf("win32_pipe failed to listen to socket: %ui", WSAGetLastError());
        closesocket(s);
        return -1;
    }
    if (getsockname(s, (SOCKADDR*)&serv_addr, &len) == SOCKET_ERROR) {
        printf("win32_pipe failed to getsockname: %ui", WSAGetLastError());
        closesocket(s);
        return -1;
    }
    if ((handles[1] = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("win32_pipe failed to create socket 2: %ui", WSAGetLastError());
        closesocket(s);
        return -1;
    }
    if (connect(handles[1], (SOCKADDR*)&serv_addr, len) == SOCKET_ERROR) {
        printf("win32_pipe failed to connect to socket: %ui", WSAGetLastError());
        closesocket(s);
        return -1;
    }
    if ((handles[0] = accept(s, (SOCKADDR*)&serv_addr, &len)) == INVALID_SOCKET) {
        printf("win32_pipe failed to accept socket: %ui", WSAGetLastError());
        closesocket(handles[1]);
        handles[1] = INVALID_SOCKET;
        closesocket(s);
        return -1;
    }
    closesocket(s);
    return 0;
}

int win32_piperead(int s, char* buf, int len) {
    int ret = recv(s, buf, len, 0);

    if (ret < 0 && WSAGetLastError() == WSAECONNRESET)
        /* EOF on the pipe! (win32 socket based implementation) */
        ret = 0;
    return ret;
}

int win32_pipewrite(int s, char* buf, int len) {
    int ret = send(s, buf, len, 0);

    if (ret < 0 && WSAGetLastError() == WSAECONNRESET)
        /* EOF on the pipe! (win32 socket based implementation) */
        ret = 0;
    return ret;
}

// Microsoft doesn't recommend the use of SetThreadAffinityMask - except for testing
// individual processors - because it can interfere with the OS scheduler and decrease
// multi-processing performance.
// https://docs.microsoft.com/en-us/windows/win32/procthread/multiple-processors
//
// In my tests SetThreadIdealProcessor() doesn't seem to improve performance at all.
// It is only a hint after all, and the OS is free to ignore it - which it will!
//
// SetThreadAffinityMask(), on the other hand, really makes a big difference when using
// all hardware threads on an Intel SMT machine:
// * without thread pinning performance can be *worse* than single thread performance (!)
// * with SetThreadAffinityMask() performance can be better than with the default number
//   of threads (= physical concurrency).
//
// If someone decides to use thread pinning, they probably are serious about it, so we
// use SetThreadAffinity() by default (= STRICT_THREAD_AFFINITY 1)

#    define STRICT_THREAD_AFFINITY 1
#    define DEBUG_THREAD_AFFINITY 0

bool win32_thread_set_affinity(int i) {
#    if STRICT_THREAD_AFFINITY
    // SetThreadAffinityMask forces a thread to run only on the specified core(s)
    DWORD_PTR mask = 1 << i;
    DWORD_PTR prev = SetThreadAffinityMask(GetCurrentThread(), mask);
#        if DEBUG_THREAD_AFFINITY
    fprintf(stdout, "set thread %d affinity mask: previous: %lx, new: %lx\n", i, prev, mask);
    fflush(stdout);
#        endif
    return prev != 0;
#    else
    // SetThreadIdealProcessor is merely a hint to the OS scheduler
    DWORD prev = SetThreadIdealProcessor(GetCurrentThread(), i);
#        if DEBUG_THREAD_AFFINITY
    fprintf(stdout, "set thread %d ideal processor: previous: %d, new: %d\n", i, prev, i);
    fflush(stdout);
#        endif
    return prev >= 0;
#    endif
}

using SetThreadDescriptionType = HRESULT(WINAPI*)(HANDLE, PCWSTR);

int win32_name_thread(const char* name) {
    // SetThreadDescription is only available on Windows 10 and above.
#    if _WIN32_WINNT >= 0x0A00
    auto fn = &SetThreadDescription;
#    else
    // To support older systems, we try to lazily load the function at
    // runtime from Kernel32.dll.
    static auto fn = []() -> SetThreadDescriptionType {
        auto lib = GetModuleHandleA("kernel32");
        if (lib) {
            auto fn = (void*)GetProcAddress(lib, "SetThreadDescription");
            if (fn) {
                return reinterpret_cast<SetThreadDescriptionType>(fn);
            }
        }
        return nullptr;
    }();
#    endif
    if (fn) {
        auto size = MultiByteToWideChar(CP_UTF8, 0, name, -1, nullptr, 0);
        if (size == 0) {
            return GetLastError();
        }
        auto buffer = (WCHAR*)alloca(size * sizeof(WCHAR));
        auto count = MultiByteToWideChar(CP_UTF8, 0, name, -1, buffer, size);
        if (count == 0) {
            return GetLastError();
        }

        HRESULT result = fn(GetCurrentThread(), buffer);
        if (result == S_OK) {
            return ERROR_SUCCESS;
        } else {
            return GetLastError();
        }
    } else {
        return ERROR_CALL_NOT_IMPLEMENTED;
    }
}

#endif
// _WIN32
