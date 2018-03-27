/*
 * StdFileReader.h
 *
 *  Created on: Jan 12, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_IO_STDFILEREADER_H_
#define TestTest1_IO_STDFILEREADER_H_

class StdFileReader: public TextReader {
public:
	StdFileReader(const char* filePath);
	StdFileReader(FILE* fp, bool closeOnExit = false);
	virtual ~StdFileReader();

	bool isOpen() const {
		return (_fp != NULL);
	}

	/// Reads a block of characters. This method will read up to count
	/// characters from this TextReader into the buffer starting at position
	/// offset.
	/// @returns The actual number of characters read. returns 0 (zero) if it
	///		is called when no more characters are left to read. Or returns -1
	///		if error occurs, and getLastError() returns a not zero value.
	virtual int read(char* chars, int offset, int length);
	/// Reads the next character from the input source.
	/// @returns the next character from the text reader, Or -1 if no further
	///		characters are available. If returns -1 and getLastError() returns
	///		a not zero value means an error occurs.
	virtual int read();

	virtual void close();

private:
	FILE* _fp;
	bool _closeOnExit;
};

#endif /* TestTest1_IO_STDFILEREADER_H_ */
