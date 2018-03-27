/*
 * StdFileWriter.h
 *
 *  Created on: Dec 15, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_IO_STDFILEWRITER_H_
#define __TEST_IO_STDFILEWRITER_H_

class StdFileWriter: public TextWriter {
public:
	// Creates a new instance from specified file path.
	StdFileWriter(const char* filePath);
	// Creates a new instance from existing file pointer.
	StdFileWriter(FILE* fp, bool closeOnExit = false);
	virtual ~StdFileWriter();

	// Writes a single character.
	virtual int write(char c);
	// Writes a character sequence from specified offset / length.
	virtual int write(const char* pch, int offset, int length);
	// Writes formatted string.
	virtual int writeFormat(const char* psz, ...);

	virtual bool flush();
	virtual void close();

	bool isFileOpen() const {
		return (_fp != NULL);
	}
private:
	FILE* _fp;
	bool _closeOnExit;
};

#endif /* __TEST_IO_STDFILEWRITER_H_ */
