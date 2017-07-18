#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sc_popen.h"
#include <cerrno>

#ifndef _WIN32

/*	This code is based on popen() and pclose() from Darwin's
	libc. The only functional difference is that sc_popen()
	places the new process' pid at the pidp address. sc_pclose()
	takes this pid which allows the code to be simplified
	(maintaining a global linked list of fds to pids and locking
	it is no longer necessary). */

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <paths.h>
#include <vector>
#include <array>
#include <string>

// allows for linking into a dylib on darwin
#if defined(__APPLE__) && !defined(SC_IPHONE)
	#include <crt_externs.h>
	#define environ (*_NSGetEnviron())
#else
	extern char **environ;
#endif

FILE *
sc_popen(const char *command, pid_t *pidp, const char *type)
{
	std::array<char*, 4> argv;
	std::string str0 = "/bin/sh";
	std::vector<char> v0( str0.begin(), str0.end() );
	v0.push_back('\0');
	argv[0] = v0.data();
	std::string str1 = "-c";
	std::vector<char> v1( str1.begin(), str1.end() );
	v1.push_back('\0');
	argv[1] = v1.data();
	std::string str2 = command;
	std::vector<char> v2( str2.begin(), str2.end() );
	v2.push_back('\0');
	argv[2] = v2.data();
	argv[3] = nullptr;

	return sc_popen_argv(v0.data(), argv.data(), pidp, type);
}

FILE *
sc_popen_argv(const char *filename, char *const argv[], pid_t *pidp, const char *type)
{
	FILE *iop;
	int pdes[2], pid, twoway;
	/*
	 * Lite2 introduced two-way popen() pipes using _socketpair().
	 * FreeBSD's pipe() is bidirectional, so we use that.
	 */
	if (strchr(type, '+')) {
		twoway = 1;
		type = "r+";
	} else  {
		twoway = 0;
		if ((*type != 'r' && *type != 'w') || type[1])
			return (nullptr);
	}
	if (pipe(pdes) < 0)
		return (nullptr);

	switch (pid = fork()) {
	case -1:			/* Error. */
		(void)close(pdes[0]);
		(void)close(pdes[1]);
		return (nullptr);
		/* NOTREACHED */
	case 0:				/* Child. */
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

		execve(filename, argv, environ);
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

    *pidp = pid;
	return iop;
}

/*
 * pclose --
 *	Pclose returns -1 if stream is not associated with a `popened' command,
 *	if already `pclosed', or waitpid returns an error.
 */
int
sc_pclose(FILE *iop, pid_t mPid)
{
	int pstat = 0;
	pid_t pid;

	(void)fclose(iop);

	do {
		pid = wait4(mPid, &pstat, 0, (struct rusage *)0);
	} while (pid == -1 && errno == EINTR);

	return (pid == -1 ? -1 : pstat);
}

#else

#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include "SC_Lock.h"
#include "SC_Codecvt.hpp"

/*	The process handle allows us to get the exit code after
	the process has died. It must be closed in sc_pclose;
	just having the pid as in the unix version is not enough.
	Thus this global linked list needs to be maintained and
	access to it needs to be locked. */

static struct process {
	struct process *next;
	FILE *fp;
	HANDLE handle;
} *processlist;

static SC_Lock processlist_mutex;

#define	THREAD_LOCK()	processlist_mutex.lock()
#define	THREAD_UNLOCK()	processlist_mutex.unlock()

FILE *
sc_popen(const char *utf8_cmd, pid_t *pid, const char *mode)
{
	PROCESS_INFORMATION pi;
	FILE *f = NULL;
	int fno;
	HANDLE child_in, child_out;
	HANDLE father_in, father_out;
	HANDLE father_in_dup, father_out_dup;
	HANDLE current_pid;
	int binary_mode;
	struct process *cur;
	BOOL read_mode, write_mode;

	if (utf8_cmd == NULL) {
		return NULL;
	}

	std::wstring cmd = L"cmd /c " + SC_Codecvt::utf8_cstr_to_utf16_wstring(utf8_cmd);

	current_pid = GetCurrentProcess();

	STARTUPINFOW si;
	ZeroMemory( &si, sizeof(STARTUPINFOW) );
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
			return NULL;
		}

		if (DuplicateHandle(current_pid, child_in,
							current_pid, &father_in_dup,
							0, TRUE, DUPLICATE_SAME_ACCESS) == FALSE) {
			fprintf(stderr, "popen: error DuplicateHandle father_out\n");
			return NULL;
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
			return NULL;
		}
		if (DuplicateHandle(current_pid, child_out,
							current_pid, &father_out_dup,
							0, TRUE, DUPLICATE_SAME_ACCESS) == FALSE) {
			fprintf(stderr, "popen: error DuplicateHandle father_in\n");
			return NULL;
		}
		CloseHandle(child_out);
		si.hStdOutput = father_out_dup;
		fno = _open_osfhandle((size_t)father_in, binary_mode);
		f = _fdopen(fno, mode);
	}
	else {
		fprintf(stderr, "popen: invalid mode %s\n", mode);
		return NULL;
	}

	// creating child process
	if (CreateProcessW(NULL,	/* pointer to name of executable module */
					  &cmd[0],	/* pointer to command line string */
					  NULL,	/* pointer to process security attributes */
					  NULL,	/* pointer to thread security attributes */
					  TRUE,	/* handle inheritance flag */
					  0,		/* creation flags */
					  NULL,	/* pointer to environment */
					  NULL,	/* pointer to current directory */
					  &si,	/* pointer to STARTUPINFOW */
					  &pi		/* pointer to PROCESS_INFORMATION */
					  ) == FALSE) {
		fprintf(stderr, "popen: CreateProcess %x\n", GetLastError());
		fclose(f);
		return NULL;
	}

	// Only the process handle is needed, ignore errors
	CloseHandle(pi.hThread);

	// Closing the unnecessary part of the pipe
	if (read_mode)
		CloseHandle(father_out_dup);
	else if (write_mode)
		CloseHandle(father_in_dup);

	if ((cur = (struct process *)malloc(sizeof(struct process))) == NULL) {
		fclose(f);
		CloseHandle(pi.hProcess);
		return NULL;
	}

	cur->fp = f;
	cur->handle = pi.hProcess;
	THREAD_LOCK();
	cur->next = processlist;
	processlist = cur;
	THREAD_UNLOCK();

	*pid = pi.dwProcessId;

	return f;
}

int
sc_pclose(FILE *f, pid_t pid)
{
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

FILE *
sc_popen_argv(const char *filename, char *const argv[], pid_t *pidp, const char *type)
{
    printf("sc_popen_argv: not implemented\n");
    return nullptr;
}

#endif
