/*
 * JSONWriter.cpp
 *
 *   Created on: Dec 14, 2014
 *      Author: Lucifer
 */

#include "JSON.h"

JSONWriter::JSONWriter(TextWriter* textWriter) {
	BFX_ASSERT(textWriter);

	_textWriter = textWriter;
	_lastError = JWE_NoError;
	_indent = 0;
	_compactMode = false;
}

JSONWriter::~JSONWriter() {
}

bool JSONWriter::isCompactMode() const {
	return _compactMode;
}
void JSONWriter::setCompactMode(bool compact) {
	_compactMode = compact;
}

bool JSONWriter::writeObjectBegin() {
	int count = _textWriter->write('{');
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
bool JSONWriter::writeEndObject() {
	int count = _textWriter->write('}');
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
bool JSONWriter::writeStartArray() {
	int count = _textWriter->write('[');
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
bool JSONWriter::writeEndArray() {
	int count = _textWriter->write(']');
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}

bool JSONWriter::writeString(const String& value) {
	int count;
	if (value.isEmpty())
		count = _textWriter->writeString("\"\"");
	else
		count = _textWriter->writeFormat("\"%s\"", (const char*) escapeString(value));
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
bool JSONWriter::writeNumber(const String& rawValue) {
	int count = _textWriter->writeFormat("%s", (const char*) rawValue);
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
bool JSONWriter::writeBool(bool value) {
	int count = _textWriter->writeString(value ? "true" : "false");
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
bool JSONWriter::writeNull() {
	int count = _textWriter->writeString("null");
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}

bool JSONWriter::writeArraySeparator() {
	int count = _textWriter->write(',');
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
bool JSONWriter::writeMemberSeparator() {
	int count = _textWriter->write(':');
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}

JSONWriteError JSONWriter::getLastError() const {
	return _lastError;
}
const char* JSONWriter::getLastErrorMessage() const {
	return _lastErrorMessage;
}

bool JSONWriter::writeSpace() {
	if (_compactMode)
		return true;
	int count = _textWriter->write(' ');
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
bool JSONWriter::writeNewLine() {
	if (_compactMode)
		return true;
	_textWriter->write('\r');
	int count = _textWriter->write('\n');
	if (count <= 0) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
bool JSONWriter::writeIndent() {
	if (_compactMode)
		return true;
	int count = 0;
	for (int i = 0; i < _indent; i++)
		if ((count = _textWriter->write('\t')) == -1)
			break;
	if (count == -1) {
		_lastError = JWE_IOError;
		return false;
	}
	return true;
}
void JSONWriter::pushIndent() {
	_indent++;
	BFX_ASSERT(_indent >= 0);
}
void JSONWriter::popIndent() {
	--_indent;
	BFX_ASSERT(_indent >= 0);
}

String JSONWriter::escapeString(const String& str) {
	String result;
	for (int i = 0; i < str.getLength(); i++) {
		char c = str[i];
		switch (c) {
		case '/':
			result.append("\\/");
			break;
		case '\\':
			result.append("\\\\");
			break;
		case '"':
			result.append("\\\"");
			break;
		default:
			result.append(c);
		}
	}
	return result;
}

const char* JSONWriter::getErrorMessageTemplate(JSONWriteError error) {
	switch (error) {
	case JWE_NoError:
		return "No error occurs";
	case JWE_MissingRoot:
		return "Root element is missing, a JSON value text must begin with '{'";
	case JWE_IOError:
		return "IO error, write operation failed";
	default:
		BFX_ASSERT(false);
		return NULL;
	}
}

void JSONWriter::setLastError(JSONWriteError error, ...) {
	const char* pszFmt = getErrorMessageTemplate(error);
	va_list argList;

	_lastError = error;
	va_start(argList, error);
	_lastErrorMessage = String::formatV(pszFmt, argList);
	va_end(argList);
}
