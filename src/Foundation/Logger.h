/*
 * Logger.h
 *
 *  Created on: Jan 16, 2015
 *      Author: Lucifer
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum LogLevel {
	LL_TRACE = 0,	///
	LL_INFO = 1,		///
	LL_WARN = 2,		///
	LL_ERROR = 3,	///
};

void log_setfile(FILE* file);

void log_setlevel(LogLevel level);

void log_printf(LogLevel level, const char* channel, const char* func,
		const char* file, int line, const char* format, ...);

#define LOGT LOG_PRINTF(LL_TRACE, LOG_TAG)
#define LOGI LOG_PRINTF(LL_INFO, LOG_TAG)
#define LOGW LOG_PRINTF(LL_WARN, LOG_TAG)
#define LOGE LOG_PRINTF(LL_ERROR, LOG_TAG)

#define LOG_PRINTF(level, channel) {	\
		const char* __channel = channel;	\
		LogLevel __level = level;	\
		LOG_PRINTF_IMPL

#define LOG_PRINTF_IMPL(...)	\
		log_printf(__level, __channel, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);	\
}

#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H_ */
