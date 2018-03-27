/*
 * TextWriter.h
 *
 *  Created on: Dec 15, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_IO_TEXTWRITER_H_
#define __TEST_IO_TEXTWRITER_H_

class TextWriter: public REFObject {
public:
	// Creates a new TextWriter.
	TextWriter();

	// Writes a single character.
	virtual int write(char c) = 0;
	// Writes a character sequence from specified offset / length.
	virtual int write(const char* chars, int offset, int length) = 0;
	// Writes a c-string (zero end).
	virtual int writeString(const char* psz);
	// Writes formatted string.
	virtual int writeFormat(const char* pszFormat, ...);

	// Flushes the writer.
	virtual bool flush() = 0;
	// Closes the writer.
	virtual void close();
};

#endif /* __TEST_IO_TEXTWRITER_H_ */
