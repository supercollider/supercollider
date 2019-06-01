#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sc_popen.h"

#include <cerrno>
#include <tuple>

#ifndef _WIN32

/*	This code is based on popen() and pclose() from Darwin's
    libc. The only functional difference is that sc_popen()
    places the new process' pid at the pidp address. sc_pclose()
    takes this pid which allows the code to be simplified
    (maintaining a global linked list of fds to pids and locking
    it is no longer necessary). */

#    include <sys/cdefs.h>
#    include <sys/param.h>
#    include <sys/wait.h>
#    include <signal.h>
#    include <unistd.h>
#    include <paths.h>
#    include <vector>
#    include <array>
#    include <string>

std::tuple<pid_t, FILE*> sc_popen(std::string&& command, const std::string& type) {
    std::vector<std::string> argv;
    argv.emplace_back("/bin/sh");
    argv.emplace_back("-c");
    argv.push_back(std::move(command));
    return sc_popen_argv(argv, type);
}

std::tuple<pid_t, FILE*> sc_popen_argv(const std::vector<std::string>& strings, const std::string& type) {
    std::vector<char*> argv(strings.size() + 1);
    for (int i = 0; i < strings.size(); ++i) {
        // execv wants argv as char *const argv[]
        // need to make the pointer const
        argv[i] = const_cast<char*>(strings[i].data());
    }
    argv[strings.size()] = nullptr;
    return sc_popen_c_argv(argv[0], argv.data(), type.c_str());
}

std::tuple<pid_t, FILE*> sc_popen_c_argv(const char* filename, char* const argv[], const char* type) {
    const std::tuple<int, FILE*> error_result = std::make_tuple(-1, nullptr);
    FILE* iop;
    pid_t pid;
    int pdes[2], twoway;
    /*
     * Lite2 introduced two-way popen() pipes using _socketpair().
     * FreeBSD's pipe() is bidirectional, so we use that.
     */
    if (strchr(type, '+')) {
        twoway = 1;
        type = "r+";
    } else {
        twoway = 0;
        if ((*type != 'r' && *type != 'w') || type[1])
            return error_result;
    }
    if (pipe(pdes) < 0)
        return error_result;

    switch (pid = fork()) {
    case -1: /* Error. */
        (void)close(pdes[0]);
        (void)close(pdes[1]);
        return error_result;
        /* NOTREACHED */
    case 0: /* Child. */
        if (*type == 'r') {
            /*
             * The _dup2() to STDIN_FILENO is repeated to avoid
             * writing to pdes[1], which might corrupt the
             * parent's copy.  This isn't good enough in
             * general, since the _exit() is no return, so
             * the compiler is free to corrupt all the local
             * variables.
             */
            (void)close(pdes[0]);
            if (pdes[1] != STDOUT_FILENO) {
                (void)dup2(pdes[1], STDOUT_FILENO);
                (void)close(pdes[1]);
                if (twoway)
                    (void)dup2(STDOUT_FILENO, STDIN_FILENO);
            } else if (twoway && (pdes[1] != STDIN_FILENO))
                (void)dup2(pdes[1], STDIN_FILENO);
        } else {
            if (pdes[0] != STDIN_FILENO) {
                (void)dup2(pdes[0], STDIN_FILENO);
                (void)close(pdes[0]);
            }
            (void)close(pdes[1]);
        }

        execvp(filename, argv);
        exit(127);
        /* NOTREACHED */
    }

    /* Parent; assume fdopen can't fail. */
    if (*type == 'r') {
        iop = fdopen(pdes[0], type);
        (void)close(pdes[1]);
    } else {
        iop = fdopen(pdes[1], type);
        (void)close(pdes[0]);
    }

    return std::make_tuple(pid, iop);
}

/*
 * pclose --
 *	Pclose returns -1 if stream is not associated with a `popened' command,
 *	if already `pclosed', or waitpid returns an error.
 */
int sc_pclose(FILE* iop, pid_t mPid) {
    int pstat = 0;
    pid_t pid;

    (void)fclose(iop);

    do {
        pid = wait4(mPid, &pstat, 0, (struct rusage*)0);
    } while (pid == -1 && errno == EINTR);

    return (pid == -1 ? -1 : pstat);
}

#else
#    include <numeric>

#    include <windows.h>
#    include <fcntl.h>
#    include <io.h>
#    include "SC_Lock.h"
#    include "SC_Codecvt.hpp"

/*	The process handle allows us to get the exit code after
    the process has died. It must be closed in sc_pclose;
    just having the pid as in the unix version is not enough.
    Thus this global linked list needs to be maintained and
    access to it needs to be locked. */

static struct process {
    struct process* next;
    FILE* fp;
    HANDLE handle;
} * processlist;

static SC_Lock processlist_mutex;

#    define THREAD_LOCK() processlist_mutex.lock()
#    define THREAD_UNLOCK() processlist_mutex.unlock()


std::tuple<pid_t, FILE*> sc_popen_argv(const std::vector<std::string>& strings, const std::string& type) {
    // joins strings using space as delimeter
    std::string commandLine = std::accumulate(
        strings.begin(), strings.end(), std::string(),
        [](const std::string& a, const std::string& b) -> std::string { return a + (a.length() > 0 ? " " : "") + b; });

    return sc_popen(std::move(commandLine), type);
}

std::tuple<pid_t, FILE*> sc_popen(std::string&& command, const std::string& type) {
    return sc_popen_c(command.data(), type.data());
}

std::tuple<pid_t, FILE*> sc_popen_c(const char* utf8_cmd, const char* mode) {
    PROCESS_INFORMATION pi;
    FILE* f = NULL;
    int fno;
    HANDLE child_in, child_out;
    HANDLE father_in, father_out;
    HANDLE father_in_dup, father_out_dup;
    HANDLE current_pid;
    int binary_mode;
    struct process* cur;
    BOOL read_mode, write_mode;
    const std::tuple<int, FILE*> error_result = std::make_tuple(-1, nullptr);

    if (utf8_cmd == NULL) {
        return error_result;
    }

    std::wstring cmd = L"cmd /c " + SC_Codecvt::utf8_cstr_to_utf16_wstring(utf8_cmd);

    current_pid = GetCurrentProcess();

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(STARTUPINFOW));
    si.cb = sizeof(STARTUPINFOW);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    // if the mode contains 'b', use binary mode
    binary_mode = (strchr(mode, 'b') ? _O_BINARY : _O_TEXT);

    // check whether the mode includes reading or writing
    read_mode = (strchr(mode, 'r') != 0);
    write_mode = (strchr(mode, 'w') != 0);

    // Opening the pipe for writing
    if (write_mode) {
        binary_mode |= _O_WRONLY;
        if (CreatePipe(&child_in, &father_out, NULL, 0) == FALSE) {
            fprintf(stderr, "popen: error CreatePipe\n");
            return error_result;
        }

        if (DuplicateHandle(current_pid, child_in, current_pid, &father_in_dup, 0, TRUE, DUPLICATE_SAME_ACCESS)
            == FALSE) {
            fprintf(stderr, "popen: error DuplicateHandle father_out\n");
            return error_result;
        }
        si.hStdInput = father_in_dup;

        CloseHandle(child_in);
        fno = _open_osfhandle((size_t)father_out, binary_mode);
        f = _fdopen(fno, mode);
    }
    // Opening the pipe for reading
    else if (read_mode) {
        binary_mode |= _O_RDONLY;
        if (CreatePipe(&father_in, &child_out, NULL, 0) == FALSE) {
            fprintf(stderr, "popen: error CreatePipe\n");
            return error_result;
        }
        if (DuplicateHandle(current_pid, child_out, current_pid, &father_out_dup, 0, TRUE, DUPLICATE_SAME_ACCESS)
            == FALSE) {
            fprintf(stderr, "popen: error DuplicateHandle father_in\n");
            return error_result;
        }
        CloseHandle(child_out);
        si.hStdOutput = father_out_dup;
        fno = _open_osfhandle((size_t)father_in, binary_mode);
        f = _fdopen(fno, mode);
    } else {
        fprintf(stderr, "popen: invalid mode %s\n", mode);
        return error_result;
    }

    // creating child process
    if (CreateProcessW(NULL, /* pointer to name of executable module */
                       &cmd[0], /* pointer to command line string */
                       NULL, /* pointer to process security attributes */
                       NULL, /* pointer to thread security attributes */
                       TRUE, /* handle inheritance flag */
                       0, /* creation flags */
                       NULL, /* pointer to environment */
                       NULL, /* pointer to current directory */
                       &si, /* pointer to STARTUPINFOW */
                       &pi /* pointer to PROCESS_INFORMATION */
                       )
        == FALSE) {
        fprintf(stderr, "popen: CreateProcess %x\n", GetLastError());
        fclose(f);
        return error_result;
    }

    // Only the process handle is needed, ignore errors
    CloseHandle(pi.hThread);

    // Closing the unnecessary part of the pipe
    if (read_mode)
        CloseHandle(father_out_dup);
    else if (write_mode)
        CloseHandle(father_in_dup);

    if ((cur = (struct process*)malloc(sizeof(struct process))) == NULL) {
        fclose(f);
        CloseHandle(pi.hProcess);
        return error_result;
    }

    cur->fp = f;
    cur->handle = pi.hProcess;
    THREAD_LOCK();
    cur->next = processlist;
    processlist = cur;
    THREAD_UNLOCK();

    return std::make_tuple(pi.dwProcessId, f);
}

int sc_pclose(FILE* f, pid_t pid) {
    struct process *cur, *last;
    int exit_code;

    THREAD_LOCK();
    for (last = NULL, cur = processlist; cur; last = cur, cur = cur->next)
        if (cur->fp == f)
            break;
    if (cur == NULL) {
        THREAD_UNLOCK();
        return (-1);
    }
    if (last == NULL)
        processlist = cur->next;
    else
        last->next = cur->next;
    THREAD_UNLOCK();

    if (WaitForSingleObject(cur->handle, INFINITE) != WAIT_OBJECT_0) {
        fprintf(stderr, "sc_pclose: error, process still active\n");
        free(cur);
        return -1;
    }

    if (GetExitCodeProcess(cur->handle, (LPDWORD)&exit_code) == 0) {
        fprintf(stderr, "sc_pclose: can't get process exit code\n");
        free(cur);
        return -1;
    }

    fclose(cur->fp);

    if (CloseHandle(cur->handle) == FALSE) {
        fprintf(stderr, "sc_pclose: error closing process handle\n");
        free(cur);
        return -1;
    }

    free(cur);

    return exit_code;
}
#endif
