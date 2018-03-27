/*
 * BufferedWriter.cpp
 *
 *  Created on: Dec 21, 2014
 *      Author: Lucifer
 */

#include "IO.h"

BufferedWriter::BufferedWriter(TextWriter* textWriter, int bufferSize) {
	BFX_ASSERT(textWriter != NULL);
#ifdef	_DEBUG
#define BUFF_SIZE 16
#else
#define BUFF_SIZE 4096
#endif

	if (bufferSize <= 0) {
		bufferSize = BUFF_SIZE;
	}
	_textWriter = textWriter;
	_buffer = new char[bufferSize];
	_bufferSize = bufferSize;
	_charPos = 0;
}

BufferedWriter::~BufferedWriter() {
	close();
	delete[] _buffer;
	_textWriter = NULL;
}

// Writes a single character.
int BufferedWriter::write(char c) {
	if (_charPos >= _bufferSize) {
		if (!flushBuffer())
			return -1;
	}
	_buffer[_charPos++] = c;
	return 1;
}

// Writes a character sequence from specified offset / length.
int BufferedWriter::write(const char* chars, int offset, int length) {
	BFX_ASSERT(offset >= 0 && length >= 0);

	if (offset < 0 || length < 0) {
		return -1;
	}
	int bytesWritten = length;
	if (length >= _bufferSize) {
        // If the request length exceeds the size of the output buffer, flush
		// the buffer and then write the data directly.  In this way buffered
		// streams will cascade harmlessly.
        if (!flushBuffer()) {
        	bytesWritten = -1;
        	goto End;
        }
        bytesWritten = _textWriter->write(chars, offset, length);
	} else {
		int pos = offset;
		int tail = offset + length;
		while (pos < tail) {
			int copy = BFX_MIN(_bufferSize - _charPos, tail - pos);
			memcpy(_buffer + _charPos, chars + pos, copy);
			pos += copy;
			_charPos += copy;
			if (_charPos >= _bufferSize) {
				if (!flushBuffer()) {
		        	bytesWritten = -1;
		        	goto End;
				}
			}
		}
	}
End:
	return bytesWritten;
}

// Flushes the writer.
bool BufferedWriter::flush() {
	if (!flushBuffer())
		return false;
	return !_textWriter->flush();
}

bool BufferedWriter::flushBuffer() {
	if (_charPos == 0)
		return true;
	if (_textWriter->write(_buffer, 0, _charPos) < 0) {
		return false;
	}
	_charPos = 0;
	return true;
}
