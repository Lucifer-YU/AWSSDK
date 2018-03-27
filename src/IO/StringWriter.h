/*
 * StringWriter.h
 *
 *  Created on: Dec 27, 2014
 *      Author: Lucifer
 */

#ifndef TestTest1_IO_STRINGWRITER_H_
#define TestTest1_IO_STRINGWRITER_H_

class StringWriter : public TextWriter {
public:
	StringWriter();
	virtual ~StringWriter();

	// Writes a single character.
	virtual int write(char c);
	// Writes a character sequence from specified offset / length.
	virtual int write(const char* chars, int offset, int length);

	// Flushes the writer.
	virtual bool flush();

	String getString() const {
		return _chars;
	}
private:
	String _chars;
};

#endif /* TestTest1_IO_STRINGWRITER_H_ */
