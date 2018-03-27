/*
 * Logger.cpp
 *
 *  Created on: Jan 16, 2015
 *      Author: Lucifer
 */

#include "Foundation.h"
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

static const char* __levelNames[] = {
		"TRACE",	///
		"INFO",	///
		"WARN",		///
		"ERROR"	///
		};

/* size of output buffer (arbitrary) */
#define BUFFER_SIZE 20000

static Mutex __mutex;
static FILE* __output_file = stdout;
static LogLevel __threshold_level = LL_TRACE;

#ifdef	WIN32
#define	snprintf	_snprintf
#endif

void log_setfile(FILE* file) {
	MutexHolder locker(&__mutex);
	if (file == NULL)
		__output_file = stdout;
	else
		__output_file = file;
}

void log_setlevel(LogLevel level) {
	if (level > LL_ERROR)
		level = LL_ERROR;
	else if (level < LL_TRACE)
		level = LL_TRACE;
	__threshold_level = level;
}

void log_printf(LogLevel level, const char* channel, const char* func,
		const char* file, int line, const char* format, ...) {

	char buffer[BUFFER_SIZE];
	char* buffer_ptr;
	size_t output_size;
	va_list args;
	int thread_id;
	int errno_reserved = 0;

	if (level < __threshold_level)
		return;

	errno_reserved = errno;
	thread_id =
#ifdef	WIN32
			(int)::GetCurrentThreadId();
#else
			(int) (long) pthread_self();
#endif

	if (LL_TRACE == level) {
		output_size = snprintf(buffer, sizeof(buffer),
			"{%08X} %-5s [%-7s] at %s.%d: ", thread_id, __levelNames[level],
			channel, func, line);
	} else {
		output_size = snprintf(buffer, sizeof(buffer),
			"{%08X} %-5s [%-7s]: ", thread_id, __levelNames[level],
			channel);
	}

	if (output_size + 1 > sizeof(buffer)) {
		fprintf(stderr, "ERROR : buffer overflow in log_printf");
		return;
	}

	buffer_ptr = buffer + output_size;

	va_start(args, format);
	output_size += vsnprintf(buffer_ptr, BUFFER_SIZE - output_size, format,
			args);
	va_end(args);

	if (output_size > BUFFER_SIZE) {
		fprintf(stderr, "ERROR : buffer overflow in log_printf");
	}

	MutexHolder locker(&__mutex);
	fprintf(__output_file, "%s\n", buffer);
	locker.release();

	// flush the output to file
	fflush(__output_file);

	errno = errno_reserved;
}
