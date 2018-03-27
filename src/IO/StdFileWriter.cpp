/*
 * StdFileWriter.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: Lucifer
 */

#include "IO.h"
#include <errno.h>

StdFileWriter::StdFileWriter(const char* filePath) {
	_fp = fopen(filePath, "w");
	_closeOnExit = true;
}

StdFileWriter::StdFileWriter(FILE* fp, bool closeOnExit) {
	BFX_ASSERT(fp);
	_fp = fp;
	_closeOnExit = closeOnExit;
}

StdFileWriter::~StdFileWriter() {
	close();
}

int StdFileWriter::write(char c) {
	BFX_ASSERT(_fp);
	return fputc(c, _fp);
}

int StdFileWriter::write(const char* pch, int offset, int length) {
	BFX_ASSERT(pch || length == 0);
	BFX_ASSERT(_fp);
	return (int) fwrite(pch + offset, sizeof(char), length, _fp);
}

int StdFileWriter::writeFormat(const char* pszFmt, ...) {
	BFX_ASSERT(pszFmt);
	BFX_ASSERT(_fp);

	va_list argList;
	va_start(argList, pszFmt);
	int retval = vfprintf(_fp, pszFmt, argList);
	va_end(argList);

	return retval;
}

bool StdFileWriter::flush() {
	BFX_ASSERT(_fp);

	if (EOF == fflush(_fp))
		return false;
	return true;
}

void StdFileWriter::close() {
	TextWriter::close();
	if (_fp && _closeOnExit) {
		fclose(_fp);
		_fp = NULL;
	}
}
