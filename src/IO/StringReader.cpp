/*
 * StringReader.cpp
 *
 *  Created on: Dec 16, 2014
 *      Author: Lucifer
 */

#include "IO.h"

StringReader::StringReader(const String& str) {
	_charPos = 0;
	_chars = str;
}

StringReader::~StringReader() {
}

int StringReader::read(char* chars, int offset, int length) {
	BFX_ASSERT(chars != NULL);
	BFX_ASSERT(offset >= 0);
	BFX_ASSERT(length >= 0);
	BFX_ASSERT(_charPos <= _chars.getLength());

	int charsUnread = (_chars.getLength() - _charPos);
	if (length > charsUnread)
		length = charsUnread;
	memcpy(chars + offset, _chars.cstr() + _charPos, length);
	_charPos += length;
	return length;
}

int StringReader::read() {
	BFX_ASSERT(_charPos <= _chars.getLength());

	if (_charPos == _chars.getLength())
		return EOF;
	return _chars[_charPos++];
}
