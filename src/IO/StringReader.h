/*
 * StringReader.h
 *
 *  Created on: Dec 16, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_IO_STRINGREADER_H_
#define __TEST_IO_STRINGREADER_H_

class StringReader: public TextReader {
public:
	StringReader(const String& str);
	virtual ~StringReader();

	virtual int read(char* chars, int offset, int length);
	// Reads a single character.
	virtual int read();

private:
	String _chars;
	int _charPos;
};

#endif /* __TEST_IO_STRINGREADER_H_ */
