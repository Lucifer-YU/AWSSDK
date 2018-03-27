/*
 * StdFileReader.cpp
 *
 *  Created on: Jan 12, 2015
 *      Author: Lucifer
 */

#include "IO.h"

StdFileReader::StdFileReader(const char* filePath) {
	_fp = fopen(filePath, "r");
	_closeOnExit = true;
}

StdFileReader::StdFileReader(FILE* fp, bool closeOnExit) {
	BFX_ASSERT(fp);
	_fp = fp;
	_closeOnExit = closeOnExit;
}

StdFileReader::~StdFileReader() {
	close();
}

int StdFileReader::read(char* pch, int offset, int length) {
	BFX_ASSERT(pch || length == 0);
	BFX_ASSERT(_fp);
	return (int) fread(pch + offset, sizeof(char), length, _fp);
}

int StdFileReader::read() {
	BFX_ASSERT(_fp);
	return fgetc(_fp);
}

void StdFileReader::close() {
	TextReader::close();
	if (_fp && _closeOnExit) {
		fclose(_fp);
		_fp = NULL;
	}
}
