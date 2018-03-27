/*
 * HttpUtils.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#include "HttpUtils.h"
#include "AWS.h"
#include <openssl/bio.h>
#include <openssl/evp.h>

#define LOG_TAG "HttpUtils"

String HttpUtils::urlEncode(const String& value, bool path) {
	if (value.isEmpty())
		return value;

	String result = urlEncode(value);
	if (path)
		result = result.replace("%2F", "/");
	return result;
}

bool HttpUtils::isAllowedUrlChar(int c) {
	// RFC 2396 states:
	// Data characters that are allowed in a URI but do not have a
	// reserved purpose are called unreserved. These include upper
	// and lower case letters, decimal digits, and a limited set of
	// punctuation marks and symbols.
	//
	// unreserved  = alphanum | mark
	//
	// mark        = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
	//
	// Unreserved characters can be escaped without changing the
	// semantics of the URI, but this should not be done unless the
	// URI is being used in a context that does not allow the
	// unescaped character to appear.
	if (isalnum(c))
		return true;
	else {
		switch (c) {
		case '-':
		case '_':
		case '.':
		case '!':
		case '~':
		case '*':
		case '\'':
		case '(':
		case ')':
			return true;
		}
	}
	return false;
}

String HttpUtils::urlEncode(const String& value) {
	if (value.isEmpty())
		return value;

	String result;
	unsigned char c;
	unsigned char low, high;

	for (int i = 0; i < value.getLength(); i++) {
		c = value[i];
		if (isAllowedUrlChar(c))
			result += c;
		else {
			high = c / 16;
			low = c % 16;
			result += '%';
			result += (high < 10 ? '0' + high : 'A' + high - 10);
			result += (low < 10 ? '0' + low : 'A' + low - 10);
		}
	}
	return result;
}

String HttpUtils::appendUri(const String& baseUri, const String& path,
		bool escapeDoubleSlash) {
	String resultUri = baseUri;
	if (!path.isEmpty()) {
		if (path.startsWith('/')) {
			// trim the trailing slash in baseUri, since the path already
			// starts with a slash
			if (resultUri.endsWith('/')) {
				resultUri = resultUri.substring(0, resultUri.getLength() - 1);
			}
		} else if (!resultUri.endsWith('/')) {
			resultUri += '/';
		}
		String encodedPath = urlEncode(path, true);
		if (escapeDoubleSlash) {
			encodedPath = encodedPath.replace("//", "/%2F");
		}
		resultUri += encodedPath;
	} else if (!resultUri.endsWith('/')) {
		resultUri += '/';
	}

	return resultUri;
}

String HttpUtils::encodeParameters(const AWSStringMap* params) {
	String result;
	if (params != NULL) {
		for (AWSStringMap::PENTRY entry = params->getFirstEntry();
				entry != NULL; entry = params->getNextEntry(entry)) {
			String encodedName = urlEncode(entry->key);
			String encodedValue = urlEncode(entry->value);
			if (!result.isEmpty())
				result.append('&');
			result.append(encodedName);
			result.append('=');
			result.append(encodedValue);
		}
	}
	return result;
}

String HttpUtils::base64Encode(const uint8_t* inBuf, int inBufSize) {
	if (inBuf == NULL || inBufSize == 0)
		return String();

	// initialization for base64 encoding stuff
	BIO* bmem = BIO_new(BIO_s_mem());
	BIO* b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_push(b64, bmem);

	BIO_write(bmem, inBuf, inBufSize);
	inBufSize = BIO_flush(bmem);

	char* outBuf;
	int outBufSize = BIO_get_mem_data(bmem, &outBuf);
	String result(outBuf, outBufSize);

	BIO_free_all(bmem);

	return result;
}

SharedBufferT<uint8_t> HttpUtils::base64Decode(const String& str) {
	if (str.isEmpty())
		return SharedBufferT<uint8_t>();

	BIO *bio, *b64;
	const int inbufsize = 1024;
	uint8_t inbuf[inbufsize];
	int inlen;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bio = BIO_new_mem_buf((void*)str.cstr(), str.getLength());
	bio = BIO_push(b64, bio);

	SharedBufferT<uint8_t> result(str.getLength());
	while((inlen = BIO_read(bio, inbuf, inbufsize)) > 0) {
		result.append(inbuf, inlen);
	}
	BIO_free_all(bio);

	return result;
}

String HttpUtils::toHexString(const uint8_t* inBuf, int inBufSize) {
	String result; // capacity = inBufSize * 2

	for (int i = 0; i < inBufSize; i++) {
		String hex = String::format("%02x", inBuf[i]);
		result.append(hex);
	}

	return result;
}
