/*
 * TextReader.h
 *
 *  Created on: Dec 15, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_IO_TEXTREADER_H_
#define __TEST_IO_TEXTREADER_H_

class TextReader: public REFObject {
public:
	TextReader();

    // Reads a block of characters. This method will read up to count
	// characters from this TextReader into the buffer starting at position
    // offset.
	// Returns the actual number of characters read. returns 0 (zero) if it is
	// called when no more characters are left to read. Or returns -1 if error
	// occurs, and getLastError() returns a not zero value.
	virtual int read(char* chars, int offset, int length) = 0;
    // Reads the next character from the input source.
	// Returns the next character from the text reader, Or -1 if no further
	// characters are available. If returns -1 and getLastError() returns a not
	// zero value means an error occurs.
	virtual int read() = 0;

	virtual void close();
};

#endif /* __TEST_IO_TEXTREADER_H_ */
