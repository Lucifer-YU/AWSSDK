/*
 * StringTraits.cpp
 *
 *  Created on: Apr 14, 2013
 *      Author: Lucifer
 */
#include "Foundation.h"
#include <errno.h>

#ifndef	_WIN32

extern "C" char * __cdecl strupr(char * string) {
	char *cp;

	/* validation section */
	if (string == NULL) {
		errno = EINVAL;
		return NULL;
	}

	for (cp = string; *cp; ++cp)
		if (('a' <= *cp) && (*cp <= 'z'))
			*cp -= 'a' - 'A';

	return (string);
}

extern "C" char * __cdecl strlwr(char * string) {
	char * cp;

	/* validation section */
	if (string == NULL) {
		errno = EINVAL;
		return NULL;
	}

	for (cp = string; *cp; ++cp) {
		if ('A' <= *cp && *cp <= 'Z')
			*cp += 'a' - 'A';
	}

	return (string);
}

extern "C" char * __cdecl strrev(char * string) {
	char *start = string;
	char *left = string;
	char ch;

	/* validation section */
	if (string == NULL) {
		errno = EINVAL;
		return NULL;
	}

	while (*string++)
		/* find end of string */
		;
	string -= 2;

	while (left < string) {
		ch = *left;
		*left++ = *string;
		*string-- = ch;
	}

	return (start);
}

extern "C" wchar_t * __cdecl wcsrev(wchar_t * string) {
	wchar_t *start = string;
	wchar_t *left = string;
	wchar_t ch;

	/* validation section */
	if (string == NULL) {
		errno = EINVAL;
		return NULL;
	}

	while (*string++)
		/* find end of string */
		;
	string -= 2;

	while (left < string) {
		ch = *left;
		*left++ = *string;
		*string-- = ch;
	}

	return (start);
}

extern "C" wchar_t * __cdecl wcsupr(wchar_t * wsrc) {
	wchar_t * p;

	/* validation section */
	if (wsrc == NULL) {
		errno = EINVAL;
		return NULL;
	}

	for (p = wsrc; *p; ++p) {
		if (L'a' <= *p && *p <= L'z')
			*p += (wchar_t) (L'A' - L'a');
	}

	return wsrc;
}

extern "C" wchar_t * __cdecl wcslwr(wchar_t * wsrc) {
	wchar_t * p;

	/* validation section */
	if (wsrc == NULL) {
		errno = EINVAL;
		return NULL;
	}

	for (p = wsrc; *p; ++p) {
		if (L'A' <= *p && *p <= L'Z')
			*p += (wchar_t) L'a' - (wchar_t) L'A';
	}

	return (wsrc);
}

extern "C" int vscprintf(const char *format, va_list argptr) {
	return vsnprintf(NULL, 0, format, argptr);
}

extern "C" int vscwprintf(const wchar_t *format, va_list argptr) {
	/*
	 * Unlike vsnprintf(), the vswprintf() routine does not tell you how many
	 * characters would have been written if there was space enough in the
	 * buffer - it just reports an error when there is not enough space.
	 * Assume a moderately large machine so kilobytes of wchar_t on the stack
	 * is not a problem.
	 */
	int bufsize = 1024;
	BufferT<wchar_t> buf(1024);
	// limit to 1M characters
	while (bufsize < 1024 * 1024) {
		va_list args;
		va_copy(args, argptr);
		wchar_t* buffer = buf.getBuffer(bufsize);
		int fmtsize = vswprintf(buffer, bufsize, format, args);
		// NOTE No necessary to call buf.releaseBuffer(...).
		if (fmtsize >= 0)
			return fmtsize;
		bufsize *= 2;
	}
	return -1;
}

#endif	//	!_WIN32
