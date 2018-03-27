/*
 * JSONReader.cpp
 *
 *   Created on: Dec 13, 2014
 *      Author: Lucifer
 */

#include "JSON.h"

#undef LOGI
#define LOGI(...)
#define LOG_TAG "JSONReader"

JSONReader::JSONReader(const char* psz, bool retain) {
	LOGT("Creating JSON reader (psz:%p, retain:%s)", psz, retain ? "true" : "false");

	BFX_ASSERT(psz);

	_retain = retain;
	_charsUsed = strlen(psz);
	_charsLength = _charsUsed + 1;
	if (_retain) {
		_chars = new char[_charsLength];
		memcpy(_chars, psz, _charsLength);
	} else {
		_chars = const_cast<char*>(psz);
	}
	_charPos = 0;
	_isEof = true;

	_lastError = JRE_NoError;
	_lineNo = 1;
	_lineStartPos = -1;
}
JSONReader::JSONReader(TextReader* textReader) {
	LOGI("JSONReader::JSONReader(textReader:%p)", textReader);
#ifdef	_DEBUG
#define BUFF_SIZE 16
#else
#define BUFF_SIZE 4096
#endif

	BFX_ASSERT(textReader);

	_textReader = textReader;

	_retain = true;
	_charsLength = BUFF_SIZE;
	_chars = new char[_charsLength];
	_charPos = 0;
	_charsUsed = 0;
	_isEof = false;

	_lastError = JRE_NoError;
	_lineNo = 1;
	_lineStartPos = -1;

	readData();
}

JSONReader::~JSONReader() {
	LOGT("Releasing JSON reader...");
	if (_retain) {
		delete[] _chars;
	}
}

JSONReader::JSONTokenType JSONReader::next() {
	LOGT("Attempt to forward to next JSON element");

	// Reset
	_value.setEmpty();

	JSONTokenType type = nextToken();
	switch (type) {
	case JT_String:
		if (!parseString())
			type = JT_Error;
		break;
	case JT_Number:
		if (!parseNumber())
			type = JT_Error;
		break;
	case JT_True:
	case JT_False:
	case JT_Null:
		break;
	case JT_Error:
		break;
	default:
		break;
	}
	return type;
}

String JSONReader::getRawValue() const {
	return _value;
}

bool JSONReader::parseString() {
	LOGT("Attempt to parse string value");

	int pos = _charPos;
	const char* chars = _chars;
	char c;

	if (chars[pos] != '"') {
		setLastError(JRE_UnexpectedToken2, chars[pos], '"');
		LOGE(_lastErrorMessage);
		return false;
	}
	pos++;
	_charPos = pos;

	for (;;) {
		if (_charsUsed <= pos)
			goto readData;

		c = chars[pos];
		switch (c) {
		case 0x09:
			pos++;
			continue;
			// EOL
		case 0x0A:
			pos++;
			onNewLine(pos);
			continue;
		case 0x0D:
			if (pos + 1 >= _charsUsed)
				goto readData;
			if (chars[pos + 1] == 0x0A) {
				pos += 2;
				onNewLine(pos);
			} else {
				/// const_cast<char*>(chars)[pos] = 0x0A;	// EOL normalization of 0xD
				pos++;
			}
			continue;
			// escape character
		case '\\':
			if (pos + 1 >= _charsUsed)
				goto readData;
			pos += 2;
			continue;
		case '"':
			// end of the string.
			_value = unescapeString(_chars, _charPos, pos - _charPos);
			LOGI("_value=%s", (const char*)_value);
			pos++;
			_charPos = pos;
			return true;
		case '\0':
			setLastError(JRE_UnexpectedEOF);
			LOGE(_lastErrorMessage);
			return false;
		default:
			pos++;
			continue;
		}
readData:
		// read new characters into the buffer
		if (readData(true) == 0) {
			// TODO onEof();
			return false;
		}
		// pos = _charPos;
		chars = _chars;
	}
	BFX_ASSERT(false);
	return false;
}

bool JSONReader::parseNumber() {
	LOGT("Attempt to parse numeric value");

	int pos = _charPos;
	const char* chars = _chars;
	char c;

	for (;;) {
		if (_charsUsed <= pos)
			goto readData;

		c = chars[pos];
		switch (c) {
		case '-':
			if (pos > _charPos) {
				// The '-'Not the first character
				setLastError(JRE_UnexpectedCharacter2, c, c);
				LOGE(_lastErrorMessage);
				return false;
			}
			continue;
			// XXX Need to validate the number format?
		case '.':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			pos++;
			continue;
		default:
			if (c == ',' || c == '}' || isWhitespaceChar(c)) {
				_value = String(_chars + _charPos, pos - _charPos);
				LOGI("_value=%s", (const char*)_value);
				_charPos = pos;
				return true;
			}
			setLastError(JRE_UnexpectedCharacter2, c, c);
			LOGE(_lastErrorMessage);
			return false;
		}
readData:
		// read new characters into the buffer
		if (readData(true) == 0) {
			// TODO onEof();
			return false;
		}
		// pos = _charPos;
		chars = _chars;
	}
	BFX_ASSERT(false);
	return false;
}

int JSONReader::eatWhitespaces() {
	LOGI("JSONReader::eatWhitespaces()");

	int pos = _charPos;
	int wsCount = 0;
	const char* chars = _chars;

	for (;;) {
		for (;;) {
			switch (chars[pos]) {
			case 0x0A:
				pos++;
				onNewLine(pos);
				continue;
			case 0x0D:
				if (pos + 1 == _charsUsed)
					goto readData;
				if (chars[pos + 1] == 0x0A) {
#ifdef EOL_NORMALIZED
					int nChar = pos - _charPos;
					if (nChar > 0) {
						wsCount += nChar;
					}
#endif
					pos += 2;
				} else {
					// EOL normalization of 0xD
					pos++;
				}
				onNewLine(pos);
				continue;
			case 0x09:
			case 0x20:
				pos++;
				continue;
			default:
				if (pos == _charsUsed) {
					goto readData;
				} else {
					int nChar = pos - _charPos;
					if (nChar > 0) {
						_charPos = pos;
						wsCount += nChar;
					} LOGI("return wsCount = %d", wsCount);
					return wsCount;
				}
			}
		}
readData:
		// read new characters into the buffer
		int nChar = pos - _charPos;
		if (nChar > 0) {
			_charPos = pos;
			wsCount += nChar;
		}

		if (readData() == 0) {
			if (_charsUsed - _charPos == 0) {
				LOGI("return wsCount = %d", wsCount);
				return wsCount;
			}
			if (_chars[_charPos] != 0x0D) {
				LOGI("We should never get to this point.");
				BFX_ASSERT(false);
			}
			BFX_ASSERT(_isEof);
		}
		pos = _charPos;
		chars = _chars;
	}
	BFX_ASSERT(false);
	return 0;
}

JSONReader::JSONTokenType JSONReader::nextToken() {
	LOGT("Attempt forward to next token.");

	// skip leading whitespace(s)
	eatWhitespaces();

	JSONTokenType type;
	// parse token
	for (;;) {
		int pos = _charPos;
		const char* chars = _chars;

		switch (chars[pos]) {
		case '{':
			type = JT_ObjectBegin;
			pos++;
			break;
		case '}':
			type = JT_ObjectEnd;
			pos++;
			break;
		case '[':
			type = JT_ArrayBegin;
			pos++;
			break;
		case ']':
			type = JT_ArrayEnd;
			pos++;
			break;
		case '"':
			type = JT_String;
			break;
		case '/':
			type = JT_Comment;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
			type = JT_Number;
			break;
		case 't':
			if (_charsUsed - pos < 3)
				goto readData;
			if (stringEqualN(chars + pos, "true", 4)) {
				type = JT_True;
				pos += 4;
			}
			break;
		case 'f':
			if (_charsUsed - pos < 5)
				goto readData;
			if (stringEqualN(chars + pos, "false", 5)) {
				type = JT_False;
				pos += 5;
			}
			break;
		case 'n':
			if (_charsUsed - pos < 4)
				goto readData;
			if (stringEqualN(chars + pos, "null", 4)) {
				type = JT_Null;
				pos += 4;
			}
			break;
		case ',':
			type = JT_ArraySeparator;
			pos++;
			break;
		case ':':
			type = JT_MemberSeparator;
			pos++;
			break;
		case 0:
			type = JT_EOF;
			break;
		default:
			setLastError(JRE_UnexpectedCharacter2, chars[pos], chars[pos]);
			LOGE(_lastErrorMessage);
			type = JT_Error;
			break;
		}

		LOGI("Next token type: %d", type);
		_charPos = pos;
		return type;

readData:
		// read new characters into the buffer
		if (readData() == 0) {
			// TODO onEof();
			return JT_Error;
		}
		pos = _charPos;
		chars = _chars;
	}
	return JT_Error;	//  should never touches at this point, just to avoid eclipse warnings.
}

// Reads more data to the character buffer, discarding already parsed chars.
int JSONReader::readData(bool appendMode) {
	// Append Mode:  Append new bytes and characters to the buffers, do not rewrite them. Allocate new buffers
	//               if the current ones are full
	// Rewrite Mode: Reuse the buffers. If there is less than half of the char buffer left for new data, move
	//               the characters that has not been parsed yet to the front of the buffer. Same for bytes.

	LOGT("appendMode: %s", appendMode ? "true" : "false");

	int chars = 0;

	if (_isEof)
		return 0;

	if (appendMode) {
		// reallocation
		if (_charsUsed == (_charsLength - 1)) {
			char* dest = new char[_charsLength * 2];
			memcpy(dest, _chars, _charsLength);
			delete[] _chars;
			_chars = dest;
			_charsLength *= 2;
		}
		// calculate free space
		chars = (_charsLength - _charsUsed) - 1;
	} else {
		int length = _charsLength;
		if ((length - _charsUsed) <= (length / 2)) {
			int count = _charsUsed - _charPos;
			if (count < (length - 1)) {
				// eat used characters in buffer
				_lineStartPos -= _charPos;
				if (count > 0) {
					memcpy(_chars, _chars + _charPos, count);
				}
				_charPos = 0;
				_charsUsed = count;
			} else {
				// however, we have no enough space.
				char* dest = new char[_charsLength * 2];
				memcpy(dest, _chars, _charsLength);
				delete[] _chars;
				_chars = dest;
				_charsLength *= 2;
			}
		}
		chars = (_charsLength - _charsUsed) - 1;
	}

	if (_textReader != NULL) {
		chars = _textReader->read(_chars, _charsUsed, chars);
		// TODO in case of return -1
		_charsUsed += chars;
	} else {
		chars = 0;
	}
	if (chars == 0)
		_isEof = true;

	_chars[_charsUsed] = '\0';
	return chars;
}

bool JSONReader::stringEqualN(const char* psz, const char* psz2, int length) {
	int i = 0;
	while (i < length && psz[i] == psz2[i])
		i++;
	return (i == length);
}

bool JSONReader::isWhitespaceChar(char c) {
	switch (c) {
	case 0x0A:
	case 0x0D:
	case 0x09:
	case 0x20:
		return true;
	default:
		return false;
	}
}

String JSONReader::unescapeString(const char* chars, int offset, int length) {
	String result;
	for (int i = 0; i < length; i++) {
		char c = chars[offset + i];
		if (c == '\\') {
			BFX_ASSERT((i + 1) < length);
			result.append(chars[offset + (++i)]);
		} else
			result.append(c);
	}
	return result;
}

const char* JSONReader::getErrorMessageTemplate(JSONReadError error) {
	switch (error) {
	case JRE_NoError:
		return "No error occurs";
	case JRE_UnexpectedEOF:
		return "Unexpected end of file has occurred.";
	case JRE_UnexpectedToken:
		return "This is an unexpected token.";
	case JRE_UnexpectedToken2:
		return "'%c' is an unexpected token. The expected token is '%c'.";
	case JRE_UnexpectedCharacter2:
		return "'%c', hexadecimal value %02x, is an unexpected character.";
	case JRE_MissingRoot:
		return "Root element is missing, a JSON value text must begin with '{'";
	default:
		BFX_ASSERT(false);
		return NULL;
	}
}

void JSONReader::setLastError(JSONReadError error, ...) {
#define BUF_SIZE	512

	const char* pszFmt = getErrorMessageTemplate(error);
	va_list argList;
	String message;

	_lastError = error;
	va_start(argList, error);
	message = String::format(pszFmt, argList);
	va_end(argList);

	_lastErrorMessage = String::format("Line %d, position %d: %s", _lineNo,
			getLinePos(), (const char*) message);
}

JSONReadError JSONReader::getLastError() const {
	return _lastError;
}

const char* JSONReader::getLastErrorMessage() const {
	return _lastErrorMessage;
}
