/*
 * StringTraits.h
 *
 *  Created on: Jan 25, 2013
 *      Author: Lucifer
 */

#ifndef __BFX_STRINGTRAITS_H__
#define __BFX_STRINGTRAITS_H__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <wchar.h>
#include <wctype.h>
#include <stdarg.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <wchar.h>
#ifndef va_copy
#define va_copy(d, s) ((d) = (s))
#endif	// va_copy
#define vscprintf	_vscprintf
#define	vscwprintf	_vscwprintf
#define	strlwr	_strlwr
#define	strupr	_strupr
#define	wcsupr	_wcsupr
#define	wcslwr	_wcslwr
#define	wcsrev	_wcsrev
#else
#ifndef __cdecl
#define	__cdecl
#endif

/*
 * String helpers
 */
extern "C" char * __cdecl strupr(char * string);
extern "C" char * __cdecl strlwr(char * string);
extern "C" char * __cdecl strrev(char * string);
extern "C" wchar_t * __cdecl wcsrev(wchar_t * string);
extern "C" wchar_t * __cdecl wcslwr(wchar_t * wsrc);
extern "C" wchar_t * __cdecl wcsupr(wchar_t * wsrc);

extern "C" int vscprintf(const char *format, va_list argptr);
extern "C" int vscwprintf(const wchar_t *format, va_list argptr);

#endif	//	_WIN32

//////////////////////////////////////////////////////////////////////////////

template<typename T>
class StringTraitsBase {
	typedef T XCHAR;
	typedef T* PXSTR;
	typedef const T* PCXSTR;

public:
	static int __cdecl stringLength(const XCHAR* psz) throw () {
		if (!psz)
			return 0;
		const XCHAR* pchEOS = psz;
		while (*pchEOS++)
			;
		return (pchEOS - psz - 1);
	}
	static void __cdecl fillChars(XCHAR ch, int nLength, XCHAR* pch) throw () {
		if (!pch)
			return;
		for (int i = 0; i < nLength; i++) {
			pch[i] = ch;
		}
	}
	static int __cdecl stringCompare(const XCHAR* pszA, const XCHAR* pszB) throw () {
		int nRet = 0;
		if (!pszA)
			return (!pszB) ? 0 : -1;
		else if (!pszB)
			return 1;

		while (!(nRet = (int) (*pszA - *pszB)) && *pszB)
			++pszA, ++pszB;

		if (nRet < 0)
			nRet = -1;
		else if (nRet > 0)
			nRet = 1;
		return (nRet);
	}
	static int __cdecl stringCompareIgnore(const XCHAR* pszA, const XCHAR* pszB) throw () {
		int chA, chB;
		if (!pszA)
			return (!pszB) ? 0 : -1;
		else if (!pszB)
			return 1;
		do {
			chA = toupper((int) *pszA);
			chB = toupper((int) *pszB);
			pszA++;
			pszB++;
		} while ((chA) && (chA == chB));
		return (int) (chA - chB);
	}
	static const XCHAR* __cdecl stringFindString(const XCHAR* pszBlock,
			const XCHAR* pszMatch) throw () {
		XCHAR *pchCur = (XCHAR *) pszBlock;
		XCHAR *s1, *s2;
		if (!pszBlock || !pszMatch)
			return (NULL);
		if (!*pszMatch)
			return pszBlock;

		while (*pchCur) {
			s1 = pchCur;
			s2 = (XCHAR *) pszMatch;
			while (*s1 && *s2 && !(*s1 - *s2))
				s1++, s2++;
			if (!*s2)
				return (pchCur);
			pchCur++;
		}

		return (NULL);
	}
	static const XCHAR* __cdecl stringFindStringRev(const XCHAR* pszBlock, const XCHAR* pszMatch) {
		// This function finds the last occurrence of the substring in the string.
		// The terminating NULL characters are not compared.
		const XCHAR *pch = NULL;
		while (1) {
			const XCHAR *p = stringFindString(pszBlock, pszMatch);
			if (!p)
				break;
			pch = p;
			pszBlock = p + 1;
		}
		return pch;
	}
	static const XCHAR* __cdecl stringFindChar(const XCHAR* pszBlock, XCHAR chMatch) throw () {
		if (!pszBlock)
			return NULL;
		while (*pszBlock && *pszBlock != chMatch)
			pszBlock++;
		return (*pszBlock == chMatch) ? pszBlock : NULL;
	}
	static const XCHAR* __cdecl stringFindCharRev(const XCHAR* psz, XCHAR ch) throw () {
		const XCHAR* pchStart = psz;
		if (!psz)
			return NULL;
		// find end of string
		while (*psz++)
			;
		// search towards front
		while (--psz != pchStart && *psz != ch)
			;
		// char found ??
		return (*psz == ch) ? psz : NULL;
	}

	static const XCHAR* __cdecl stringScanSet(const XCHAR* pszBlock, const XCHAR* pszMatch) throw () {
		if (!pszBlock || !pszMatch)
			return NULL;
		// 1st char in control string stops search
		while (*pszBlock) {
			for (XCHAR* pch = (XCHAR*) pszMatch; *pch; pch++) {
				if (*pch == *pszBlock)
					return (pszBlock);
			}
			pszBlock++;
		}
		return (NULL);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<typename T = char>
class StringTraitsT: public StringTraitsBase<T> {
public:
	static int __cdecl getFormattedLength(const char* pszFormat,
			va_list args) throw () {
		va_list args1;
		va_copy(args1, args);
		return vscprintf(pszFormat, args1);
	}
	static int __cdecl format(char* pszBuffer, size_t nLength,
			const char* pszFormat, va_list args) throw () {
		va_list args1;
		va_copy(args1, args);
		return vsnprintf(pszBuffer, nLength, pszFormat, args1);
	}
	static bool isSpace(char ch) throw () {
		return (isspace(ch) != 0);
	}
	static char* __cdecl stringUppercase(char* psz) throw () {
		return strupr(psz);
	}
	static char* __cdecl stringLowercase(char* psz) throw () {
		return strlwr(psz);
	}
	static char* __cdecl stringReverse(char* psz) throw () {
		return strrev(psz);
	}
};

//////////////////////////////////////////////////////////////////////////////

template<>
class StringTraitsT<wchar_t> : public StringTraitsBase<wchar_t> {
public:
	static int __cdecl getFormattedLength(const wchar_t* pszFormat,
			va_list args) throw () {
		va_list args1;
		va_copy(args1, args);
		return vscwprintf(pszFormat, args1);
	}
	static int __cdecl format(wchar_t* pszBuffer, size_t nLength,
			const wchar_t* pszFormat, va_list args) throw () {
		va_list args1;
		va_copy(args1, args1);
		return vswprintf(pszBuffer, nLength, pszFormat, args);
	}
	static bool isSpace(wchar_t ch) throw () {
		return (iswspace(ch) != 0);
	}
	static wchar_t* __cdecl stringUppercase(wchar_t* psz) throw () {
		return wcsupr(psz);
	}
	static wchar_t* __cdecl stringLowercase(wchar_t* psz) throw () {
		return wcslwr(psz);
	}
	static wchar_t* __cdecl stringReverse(wchar_t* psz) throw () {
		return wcsrev(psz);
	}
};

#endif /* __BFX_STRINGTRAITS_H__ */
