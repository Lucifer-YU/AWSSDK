/*
 * HttpUtils.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_HTTPUTILS_H_
#define TestTest1_AWS_HTTPUTILS_H_

#include "AWS.h"

class AWSHttpRequest;

class HttpUtils {
private:
	HttpUtils();
	virtual ~HttpUtils();

public:
	static String urlEncode(const String& value, bool path);
	static String urlEncode(const String& value);

	static String appendUri(const String& baseUri, const String& path,
			bool escapeDoubleSlash);

	static String encodeParameters(const AWSStringMap* params);

	static String base64Encode(const uint8_t* inBuf, int inBufSize);
	static SharedBufferT<uint8_t> base64Decode(const String& str);

	static String toHexString(const uint8_t* inBuf, int inBufSize);


private:
	static bool isAllowedUrlChar(int c);
};

#endif /* TestTest1_AWS_HTTPUTILS_H_ */
