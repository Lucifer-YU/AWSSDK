/*
 * TextWriter.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: Lucifer
 */

#include "IO.h"

TextWriter::TextWriter() {
}

int TextWriter::writeString(const char* psz) {
	if (psz == NULL)
		return 0;
	return write(psz, 0, strlen(psz));
}

int TextWriter::writeFormat(const char* pszFmt, ...) {
	va_list argList;

	va_start(argList, pszFmt);
	int len = vscprintf(pszFmt, argList) + 1;	// including zero end character
	va_end(argList);

	BufferT<char> chars(len);
	char* pszBuffer = chars.getBuffer(len);

	va_start(argList, pszFmt);
	int len2 = vsnprintf(pszBuffer, len, pszFmt, argList);
	va_end(argList);

	BFX_ASSERT((len - 1) == len2);

	return writeString(pszBuffer);
}

void TextWriter::close() {
	flush();
}
