#include <stdio.h>
#include <sys/types.h>

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
#endif

/** \brief Opens a pipe to a separate process.
 *
 * This function assumes a UTF-8 encoded, narrow-char string.
 */
FILE* sc_popen(const char* command, pid_t* pidp, const char* type);

FILE* sc_popen_argv(const char* filename, char* const argv[], pid_t* pidp, const char* type);
int sc_pclose(FILE* iop, pid_t mPid);
