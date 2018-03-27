/*
 * StringWriter.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: Lucifer
 */

#include "IO.h"

StringWriter::StringWriter() {
}

StringWriter::~StringWriter() {
}

// Writes a single character.
int StringWriter::write(char c) {
	_chars.append(c);
	return 1;
}

// Writes a character sequence from specified offset / length.
int StringWriter::write(const char* chars, int offset, int length) {
	_chars.append(chars + offset, length);
	return length;
}

// Flushes the writer.
bool StringWriter::flush() {
	return true;
}
