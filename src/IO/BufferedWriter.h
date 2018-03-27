/*
 * BufferedWriter.h
 *
 *  Created on: Dec 21, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_IO_BUFFEREDWRITER_H_
#define __TEST_IO_BUFFEREDWRITER_H_

class BufferedWriter : public TextWriter {
public:
	BufferedWriter(TextWriter* textWriter, int bufferSize = -1);
	virtual ~BufferedWriter();

	// Writes a single character.
	virtual int write(char c);
	// Writes a character sequence from specified offset / length.
	virtual int write(const char* chars, int offset, int length);
	// Flushes the writer.
	virtual bool flush();

protected:
	bool flushBuffer();

private:
	char* _buffer;
	int _bufferSize;
	int _charPos;
	REF<TextWriter> _textWriter;
};

#endif /* __TEST_IO_BUFFEREDWRITER_H_ */
