#include <stdio.h>
#include <sys/types.h>

#include <vector>
#include <string>

#ifndef _WIN32
#    include <unistd.h>
#    include <sys/wait.h>
#endif

#ifdef _WIN32
#    include "SC_Win32Utils.h"
/*
 *	Signal stuff
 *	For WIN32, there is no wait() call so there are no wait() macros
 *	to interpret the return value of system().  Instead, system()
 *	return values < 0x100 are used for exit() termination, and higher
 *	values are used to indicated non-exit() termination, which is
 *	similar to a unix-style signal exit (think SIGSEGV ==
 *	STATUS_ACCESS_VIOLATION).  See this URL for a list of WIN32
 *	STATUS_* values from Wine:
 *
 *		http://source.winehq.org/source/include/ntstatus.h
 */
#    define WIFEXITED(w) (((w)&0xffffff00) == 0)
#    define WIFSIGNALED(w) (!WIFEXITED(w))
#    define WEXITSTATUS(w) (w)
#    define WTERMSIG(w) (w)

std::tuple<pid_t, FILE*> sc_popen_c(const char* utf8_cmd, const char* mode);
#else
std::tuple<pid_t, FILE*> sc_popen_c_argv(const char* filename, char* const argv[], const char* type);
#endif

/** \brief Opens a pipe to a separate process.
 *
 * This function assumes a UTF-8 encoded, narrow-char string.
 */
std::tuple<pid_t, FILE*> sc_popen(std::string&& command, const std::string& type);
std::tuple<pid_t, FILE*> sc_popen_argv(const std::vector<std::string>& strings, const std::string& type);
int sc_pclose(FILE* iop, pid_t mPid);
