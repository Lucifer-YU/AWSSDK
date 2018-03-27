/*
 * String.h
 *
 *  Created on: Jan 23, 2013
 *      Author: Lucifer
 */

#ifndef __BFX_STRING_H__
#define __BFX_STRING_H__

#include "StringTraits.h"

//////////////////////////////////////////////////////////////////////////////
/**
 * A string class designed to use referenceable string data.
 */
template<typename T, class StrTraits = StringTraitsT<T> >
class StringT {
	typedef T XCHAR;
	typedef T* PXSTR;
	typedef const T* PCXSTR;

private:
	struct StringData {
		StringData(int capacitySize = 0) :
				_buffer(NULL), _length(0), _allocLength(0), _refCount(0) {
			if (capacitySize > 0)
				capacity(capacitySize);
		}
		virtual ~StringData() {
			if (_buffer != NULL)
				delete[] _buffer;
		}
		long addRef() const {
			return (++_refCount);
		}
		long release() const {
			if ((--_refCount) == 0) {
				delete this;
				return 0;
			}
			return _refCount;
		}
		long getRefCount() const {
			return _refCount;
		}
		bool isShared() const {
			return (getRefCount() > 1);
		}
		void capacity(int capacitySize) {
			BFX_ASSERT(capacitySize >= 0);

			// calculate required buffer length, align by m_nAllocLength.
			int allocSize = BFX_MAX(_allocLength + 1, 16);
			while (allocSize < (capacitySize + 1)) {
				allocSize *= 2;
			}
			// if overflow then allocate new.
			if (allocSize > (_allocLength + 1)) {
				XCHAR* newBuffer = new XCHAR[allocSize];
				if (_length > 0)
					memcpy(newBuffer, _buffer, _length * sizeof(XCHAR));

				if (_buffer != NULL)
					delete[] _buffer;

				_buffer = newBuffer;
				_allocLength = (allocSize - 1);
			}
		}
		PXSTR _buffer;
		int _length;
		int _allocLength;
		mutable long _refCount;
	};
public:
	/**
	 * Creates an empty string.
	 */
	StringT() :
			_data(new StringData()) {
	}
	/**
	 * Creates a string with a character array with specified length.
	 * @param pchSrc Pointer to a character array.
	 * @param length Number of characters to use.
	 */
	StringT(PCXSTR pchSrc, int length = -1) {
		BFX_ASSERT(pchSrc != NULL || length == 0);
		if (length == -1) {
			length = StrTraits::stringLength(pchSrc);
		}

		BFX_ASSERT(length >= 0);
		_data = new StringData(length);
		memcpy(_data->_buffer, pchSrc, length * sizeof(XCHAR));
		setLength(length);
	}
	/**
	 * Creates a string with value of specified character repeated a specified number of times.
	 * @param ch A character
	 * @param nLength The number of times ch occurs.
	 */
	StringT(XCHAR ch, int nLength = 1) {
		BFX_ASSERT(nLength >= 0);
		_data = new StringData(nLength);
		memset(_data->_buffer, ch, nLength * sizeof(XCHAR));
		setLength(nLength);
	}
	/**
	 * Creates a string with another string.
	 * @param strSrc The another string.
	 */
	StringT(const StringT& strSrc) :
			_data(strSrc._data) {
	}
	virtual ~StringT() {
	}

	operator PCXSTR() const {
		return cstr();
	}
	StringT& operator=(const StringT& strSrc) {
		_data = strSrc._data;
		return (*this);
	}

	XCHAR operator[](int iChar) const {
		BFX_ASSERT(iChar >= 0 && iChar < getLength());

		return cstr()[iChar];
	}

	int getLength() const {
		return (_data->_length);
	}
	PCXSTR cstr() const {
		return (_data->_buffer);
	}
	bool isEmpty() const {
		return (getLength() == 0);
	}
	void setEmpty() {
		if (isEmpty())
			return;
		if (!_data->isShared())
			// Don't reallocate a locked buffer that's shrinking
			setLength(0);
		else
			_data = new StringData();
	}

	void append(XCHAR ch) {
		int newLength = getLength() + 1;
		PXSTR pszBuffer = getBuffer(newLength);
		pszBuffer[getLength()] = ch;
		setLength(newLength);
	}
	void append(PCXSTR pszSrc) {
		append(pszSrc, StrTraits::stringLength(pszSrc));
	}
	void append(PCXSTR pszSrc, int nLength) {
#ifdef	__MFCSTYLE__
		int nOffset = pszSrc - cstr();
#endif
		int nOldLength = getLength();
		if (nOldLength < 0) {
			// protects from underflow
			nOldLength = 0;
		}

		// Make sure the nLength is greater than zero
		BFX_ASSERT(nLength >= 0);

		//Make sure we don't read pass end of the terminating NULL
		BFX_ASSERT(StrTraits::stringLength(pszSrc) >= nLength);

		int nNewLength = nOldLength + nLength;
		PXSTR pszBuffer = getBuffer(nNewLength);
#ifdef	__MFCSTYLE__
		if (nOffset <= nOldLength) {
			pszSrc = pszBuffer + nOffset;
		}
#endif
		memcpy(pszBuffer + nOldLength, pszSrc, nLength * sizeof(XCHAR));
		setLength(nNewLength);
	}

	StringT insert(int iIndex, XCHAR ch) const {
		BFX_ASSERT(iIndex >= 0);

		StringT strResult(*this);
		BFX_ASSERT(iIndex < strResult.getLength());

		int nNewLength = strResult.getLength() + 1;
		PXSTR pszBuffer = strResult.getBuffer(nNewLength);
		// move existing bytes down
		memmove(pszBuffer + iIndex + 1, pszBuffer + iIndex, (nNewLength - iIndex) * sizeof(XCHAR));
		pszBuffer[iIndex] = ch;

		strResult.setLength(nNewLength);

		return (strResult);
	}
	StringT insert(int iIndex, PCXSTR psz) const {
		BFX_ASSERT(iIndex >= 0);

		StringT strResult(*this);
		BFX_ASSERT(iIndex < strResult.getLength());

		// nInsertLength and nNewLength are in XCHARs
		int nInsertLength = StrTraits::stringLength(psz);
		int nNewLength = strResult.getLength();
		if (nInsertLength > 0) {
			nNewLength += nInsertLength;

			PXSTR pszBuffer = strResult.getBuffer(nNewLength);
			// move existing bytes down
			memmove(pszBuffer + iIndex + nInsertLength, pszBuffer + iIndex,
					(nNewLength - iIndex - nInsertLength + 1) * sizeof(XCHAR));
			memcpy(pszBuffer + iIndex, psz, nInsertLength * sizeof(XCHAR));
			strResult.setLength(nNewLength);
		}

		return (strResult);
	}
	StringT remove(int iIndex, int nCount = 1) const {
		BFX_ASSERT(iIndex >= 0);
		BFX_ASSERT(nCount >= 0);

		StringT strResult(*this);
		int nLength = getLength();
		BFX_ASSERT((iIndex + nCount) <= nLength);

		if (nCount > 0) {
			int nNewLength = nLength - nCount;
			int nCharsToCopy = nLength - (iIndex + nCount) + 1;
			PXSTR pszBuffer = strResult.getBuffer();
			memmove(pszBuffer + iIndex, pszBuffer + iIndex + nCount, nCharsToCopy * sizeof(XCHAR));
			strResult.setLength(nNewLength);
		}

		return (strResult);
	}
	StringT replace(XCHAR chOld, XCHAR chNew) const {
		StringT strResult(*this);

		// short-circuit the nop case
		if (chOld != chNew) {
			// otherwise modify each character that matches in the string
			PXSTR pszBuffer = strResult.getBuffer();
			int nLength = strResult.getLength();
			for (int iChar = 0; iChar < nLength; iChar++) {
				// replace instances of the specified character only
				if (pszBuffer[iChar] == chOld) {
					pszBuffer[iChar] = chNew;
				}
			}
			strResult.setLength(nLength);
		}
		return (strResult);
	}
	StringT replace(PCXSTR pszOld, PCXSTR pszNew) const {
		// can't have empty or NULL lpszOld
		// nSourceLen is in XCHARs
		int nSourceLen = StrTraits::stringLength(pszOld);
		BFX_ASSERT(nSourceLen > 0);

		// nReplacementLen is in XCHARs
		int nReplacementLen = StrTraits::stringLength(pszNew);

		// loop once to figure out the size of the result string
		StringT<XCHAR> strResult(*this);
		int nCount = 0;
		PCXSTR pszStart = strResult.getBuffer();
		PCXSTR pszEnd = pszStart + getLength();
		while (pszStart < pszEnd) {
			PCXSTR pszTarget;
			while ((pszTarget = StrTraits::stringFindString(pszStart, pszOld)) != NULL) {
				nCount++;
				pszStart = pszTarget + nSourceLen;
			}
			pszStart += StrTraits::stringLength(pszStart) + 1;
		}

		// if any changes were made, make them
		if (nCount > 0) {
			// if the buffer is too small, just allocate a new buffer (slow but sure)
			int nOldLength = strResult.getLength();
			int nNewLength = nOldLength + (nReplacementLen - nSourceLen) * nCount;

			PXSTR pszBuffer = strResult.getBuffer(BFX_MAX(nNewLength, nOldLength));
			pszStart = pszBuffer;
			pszEnd = pszStart + nOldLength;

			// loop again to actually do the work
			while (pszStart < pszEnd) {
				PXSTR pszTarget;
				while ((pszTarget = (PXSTR) StrTraits::stringFindString(pszStart, pszOld)) != NULL) {
					int nBalance = nOldLength - int(pszTarget - pszBuffer + nSourceLen);
					memmove(pszTarget + nReplacementLen, pszTarget + nSourceLen,
							nBalance * sizeof(XCHAR));
					memcpy(pszTarget, pszNew, nReplacementLen * sizeof(XCHAR));
					pszStart = pszTarget + nReplacementLen;
					pszTarget[nReplacementLen + nBalance] = 0;
					nOldLength += (nReplacementLen - nSourceLen);
				}
				pszStart += StrTraits::stringLength(pszStart) + 1;
			}
			BFX_ASSERT(pszBuffer[nNewLength] == 0);
			strResult.setLength(nNewLength);
		}

		return (strResult);
	}
	int compareTo(PCXSTR psz, bool ignoreCase = false) const {
		PCXSTR pszStr = cstr();
		if (!ignoreCase) {
			return (StrTraits::stringCompare(pszStr, psz));
		} else {
			return (StrTraits::stringCompareIgnore(pszStr, psz));
		}
	}
	bool startsWith(XCHAR ch, int iStart = 0) const {
		// nLength is in XCHARs
		int nLength = getLength();
		if (iStart < 0 || iStart > nLength)
			return false;

		PCXSTR pszStr = cstr();

		return (pszStr[iStart] == ch);
	}
	bool startsWith(PCXSTR psz, int iStart = 0) const {
		// nLength is in XCHARs
		int nLength = getLength();
		if (iStart < 0 || iStart > nLength)
			return false;

		PCXSTR pszStr = cstr() + iStart;
		nLength -= iStart;
		for (int i = 0; (i < nLength) && (psz[i] == 0); i ++) {
			if (pszStr[i] != psz[i])
				return false;
		}
		return true;
	}
	bool endsWith(XCHAR ch) const {
		// nLength is in XCHARs
		int nLength = getLength();
		PCXSTR pszStr = cstr();

		return (pszStr[nLength-1] == ch);
	}
	bool endsWith(PCXSTR psz) const {
		int nLength = StrTraits::stringLength(psz);
		return startsWith(psz, getLength() - nLength);
	}
	int indexOf(XCHAR ch, int iStart = 0) const {
		// iStart is in XCHARs
		BFX_ASSERT(iStart >= 0);

		// nLength is in XCHARs
		int nLength = getLength();
		if (iStart < 0 || iStart > nLength) {
			return (-1);
		}

		PCXSTR pszStr = cstr();

		// find first single character
		PCXSTR psz = StrTraits::stringFindChar(pszStr + iStart, ch);

		// return -1 if not found and index otherwise
		return ((psz == NULL) ? -1 : int(psz - pszStr));
	}
	int indexOf(PCXSTR pszSub, int iStart = 0) const {
		// iStart is in XCHARs
		BFX_ASSERT(iStart >= 0);
		if (pszSub == NULL) {
			return (-1);
		}
		// nLength is in XCHARs
		int nLength = getLength();
		if (iStart < 0 || iStart > nLength) {
			return (-1);
		}
		PCXSTR pszStr = cstr();

		// find first matching substring
		PCXSTR psz = StrTraits::stringFindString(pszStr + iStart, pszSub);

		// return -1 for not found, distance from beginning otherwise
		return ((psz == NULL) ? -1 : int(psz - pszStr));
	}
	int indexOfAny(PCXSTR pszCharSet, int iStart = 0) const {
		// iStart is in XCHARs
		BFX_ASSERT(iStart >= 0);
		if (pszCharSet == NULL) {
			return (-1);
		}
		// nLength is in XCHARs
		int nLength = getLength();
		if (iStart < 0 || iStart > nLength) {
			return (-1);
		}

		PCXSTR psz = cstr();

		PCXSTR pszResult = StrTraits::stringScanSet(psz + iStart, pszCharSet);
		return ((pszResult == NULL) ? -1 : int(pszResult - psz));
	}
	int lastIndexOf(XCHAR ch) const {
		PCXSTR psz = cstr();

		// find last single character
		PCXSTR pszResult = StrTraits::stringFindCharRev(psz, ch);

		// return -1 if not found, distance from beginning otherwise
		return (pszResult == NULL) ? -1 : int(pszResult - psz);
	}

	StringT substring(int iFirst) const {
		return substring(iFirst, getLength() - iFirst);
	}
	StringT substring(int iFirst, int nCount) const {
		BFX_ASSERT(iFirst >= 0 && nCount >= 0);
		BFX_ASSERT((iFirst + nCount) <= getLength());

		// optimize case of returning entire string
		if (iFirst == 0 && nCount == getLength()) {
			return (*this);
		}

		return StringT(cstr() + iFirst, nCount);
	}
	StringT toUpper() const {
		StringT strResult(*this);
		int nLength = strResult.getLength();

		PXSTR pszBuffer = strResult.getBuffer(nLength);
		StrTraits::stringUppercase(pszBuffer);
		strResult.setLength(nLength);

		return strResult;
	}
	StringT toLower() const {
		StringT strResult(*this);
		int nLength = strResult.getLength();

		PXSTR pszBuffer = strResult.getBuffer(nLength);
		StrTraits::stringLowercase(pszBuffer);
		strResult.setLength(nLength);

		return strResult;
	}
	StringT reverse() const {
		StringT strResult(*this);
		int nLength = strResult.getLength();

		PXSTR pszBuffer = strResult.getBuffer(nLength);
		StrTraits::stringReverse(pszBuffer);
		strResult.setLength(nLength);

		return strResult;
	}

	StringT trim() const {
		return (trimRight().trimLeft());
	}
	StringT trimRight() const {
		if (isEmpty())
			return (*this);

		// find beginning of trailing spaces by starting at beginning
		PCXSTR pchStart = cstr();
		PCXSTR pchLast = NULL;
		for (PCXSTR pch = pchStart; *pch != 0; pch++) {
			if (StrTraits::isSpace(*pch)) {
				if (pchLast == NULL)
					pchLast = pch;
			} else {
				pchLast = NULL;
			}
		}

		if (pchLast != NULL) {
			// truncate at trailing space start
			int iLast = (int) (pchLast - pchStart);

			return substring(0, iLast);
		}
		return (*this);
	}
	StringT trimLeft() const {
		if (isEmpty())
			return (*this);

		// find first non-space character
		PCXSTR pchStart = cstr();
		PCXSTR pch = pchStart;

		while (StrTraits::isSpace(*pch)) {
			pch++;
		}

		if (pch != pchStart) {
			// fixup data and length
			int iFirst = (int) (pch - pchStart);
			int nLength = getLength() - iFirst;
			return substring(iFirst, nLength);
		}
		return (*this);
	}
	StringT trim(XCHAR chTarget) const {
		return (trimRight(chTarget).trimLeft(chTarget));
	}
	StringT trimLeft(XCHAR chTarget) const {
		if (isEmpty())
			return (*this);

		// find first non-space character
		PCXSTR pchStart = cstr();
		PCXSTR pch = pchStart;

		while (pch != pchStart) {
			pch++;
		}

		if (pch != pchStart) {
			// fix-up data and length
			int iFirst = (int) (pch - pchStart);
			int nLength = getLength() - iFirst;
			return substring(iFirst, nLength);
		}
		return (*this);
	}
	StringT trimRight(XCHAR chTarget) const {
		if (isEmpty())
			return (*this);

		// find beginning of trailing matches by starting at beginning
		PCXSTR pchStart = cstr();
		PCXSTR pchLast = NULL;
		for (PCXSTR pch = pchStart; *pch != 0/*NULL*/; pch++) {
			if (*pch == chTarget) {
				if (pchLast == NULL)
					pchLast = pch;
			} else {
				pchLast = NULL;
			}
		}

		if (pchLast != NULL) {
			// truncate at trailing space start
			int iLast = (int) (pchLast - pchStart);

			return substring(0, iLast);
		}
		return (*this);
	}
	StringT trim(PCXSTR pszTargets) const {
		return (trimRight(pszTargets).trimLeft(pszTargets));
	}
	StringT trimLeft(PCXSTR pszTargets) const {
		if (isEmpty())
			return (*this);

		// if we're not trimming anything, we're not doing any work
		if ((pszTargets == NULL) || (*pszTargets == 0)) {
			return (*this);
		}

		PCXSTR pchStart = cstr();
		PCXSTR pch = pchStart;
		while (*pch != 0 && StrTraits::stringFindChar(pszTargets, *pch) != NULL) {
			pch++;
		}

		if (pch != pchStart) {
			// fix-up data and length
			int iFirst = (int) (pch - pchStart);
			int nLength = getLength() - iFirst;
			return substring(iFirst, nLength);
		}
		return (*this);
	}
	StringT trimRight(PCXSTR pszTargets) const {
		if (isEmpty())
			return (*this);

		// if we're not trimming anything, we're not doing any work
		if ((pszTargets == NULL) || (*pszTargets == 0)) {
			return (*this);
		}

		// find beginning of trailing matches by starting at beginning
		PCXSTR pchStart = cstr();
		PCXSTR pchLast = NULL;
		for (PCXSTR pch = pchStart; *pch != 0; pch++) {
			if (StrTraits::stringFindChar(pszTargets, *pch) != NULL) {
				if (pchLast == NULL) {
					pchLast = pch;
				}
			} else {
				pchLast = NULL;
			}
		}

		if (pchLast != NULL) {
			// truncate at trailing space start
			int iLast = (int) (pchLast - pchStart);

			return substring(0, iLast);
		}
		return (*this);
	}

	static StringT __cdecl format(PCXSTR pszFormat, ...) {
		va_list argList;
		StringT strResult;

		va_start(argList, pszFormat);
		int nLength = StrTraits::getFormattedLength(pszFormat, argList);
		PXSTR pszBuffer = strResult.getBuffer(nLength);
		StrTraits::format(pszBuffer, nLength + 1, pszFormat, argList);
		va_end(argList);
		strResult.setLength(nLength);

		return strResult;
	}

	static StringT __cdecl formatV(PCXSTR pszFormat, va_list argList) {
		BFX_ASSERT(pszFormat);

		int nLength = StrTraits::getFormattedLength(pszFormat, argList);
		BFX_ASSERT(nLength >= 0);

		StringT strResult;
		PXSTR pszBuffer = strResult.getBuffer(nLength);
		StrTraits::format(pszBuffer, nLength + 1, pszFormat, argList);
		strResult.setLength(nLength);

		return strResult;
	}

	StringT& operator+=(const StringT& strSrc) {
		append(strSrc, strSrc.getLength());
		return (*this);
	}
	StringT& operator+=(PCXSTR pszSrc) {
		append(pszSrc);
		return (*this);
	}
	StringT& operator+=(XCHAR chSrc) {
		append(chSrc);
		return (*this);
	}

	friend StringT operator+(const StringT& str1, const StringT& str2) {
		StringT strResult(str1);
		strResult += str2;
		return (strResult);
	}
	friend StringT operator+(const StringT& str1, PCXSTR psz2) {
		StringT strResult(str1);
		strResult += psz2;
		return (strResult);
	}
	friend StringT operator+(PCXSTR psz1, const StringT& str2) {
		StringT strResult(psz1);
		strResult += str2;
		return (strResult);
	}
	friend StringT operator+(const StringT& str1, XCHAR ch2) {
		StringT strResult(str1);
		strResult += ch2;
		return (strResult);
	}
	friend StringT operator+(XCHAR ch1, const StringT& str2) {
		StringT strResult(ch1);
		strResult += str2;
		return (strResult);
	}
	friend bool operator==(const StringT& str1, const StringT& str2) throw () {
		return (str1.compareTo(str2) == 0);
	}

	friend bool operator==(const StringT& str1, PCXSTR psz2) throw () {
		return (str1.compareTo(psz2) == 0);
	}

	friend bool operator==(PCXSTR psz1, const StringT& str2) throw () {
		return (str2.compareTo(psz1) == 0);
	}

	friend bool operator!=(const StringT& str1, const StringT& str2) throw () {
		return (str1.compareTo(str2) != 0);
	}

	friend bool operator!=(const StringT& str1, PCXSTR psz2) throw () {
		return (str1.compareTo(psz2) != 0);
	}

	friend bool operator!=(PCXSTR psz1, const StringT& str2) throw () {
		return (str2.compareTo(psz1) != 0);
	}

private:
	PXSTR getBuffer() {
		return getBuffer(getLength());
	}
	PXSTR getBuffer(int nLength) {
		BFX_ASSERT(nLength >= 0);

		if (_data->isShared()) {
			// clone it.
			int nOldLength = getLength();
			int nAllocLength = BFX_MAX(nOldLength, nLength);
			StringData* pNewData = new StringData(nAllocLength);
			if (nOldLength > 0) {
				memcpy(pNewData->_buffer, cstr(), nOldLength * sizeof(XCHAR));
				pNewData->_buffer[nOldLength] = 0;
				pNewData->_length = nOldLength;
			}
			_data = pNewData;
		} else {
			// grow it up if necessary.
			_data->capacity(nLength);
		}

		return _data->_buffer;
	}
	void setLength(int nLength) {
		BFX_ASSERT(nLength >= 0);
		BFX_ASSERT(nLength <= _data->_allocLength);
		BFX_ASSERT(!_data->isShared());

		_data->_length = nLength;
		if (_data->_buffer == NULL)
			return;
		_data->_buffer[nLength] = 0;
	}

private:
	REF<StringData> _data;
};

typedef StringT<char> String;
typedef StringT<wchar_t> StringW;

#endif /* __BFX_STRING_H__ */
