/*
 * AWSResultParser.h
 *
 *  Created on: Jan 8, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_AWSRESULTPARSER_H_
#define TestTest1_AWS_AWSRESULTPARSER_H_

#include "../Foundation/Foundation.h"
#include "../IO/IO.h"
#include <libxml/globals.h>

class AWSResultParser;

class AWSResult: public REFObject {
public:
	AWSResult() {
	}
	virtual ~AWSResult() {
	}
};

class AWSResultUnmarshaller {
public:
	AWSResultUnmarshaller();
	virtual ~AWSResultUnmarshaller();

protected:
	bool parse(TextReader* reader);

	virtual void onStartElement(const char* localname, const char* prefix,
			const char* URI, int nb_namespaces, const char** namespaces,
			int nb_attributes, int nb_defaulted, const char** attributes) =0;
	virtual void onCharacters(const char* value, int len) = 0;
	virtual void onEndElement(const char*localname, const char* prefix,
			const char* URI) =0;

	static bool stringEquals(const char* s1, const char* s2) {
		return (0 == StringTraitsT<char>::stringCompare(s1, s2));
	}

	// SAX Callback functions we will uses.
	static void startElementNsCallback(void* ctx, const xmlChar* localname,
			const xmlChar* prefix, const xmlChar* URI, int nb_namespaces,
			const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
			const xmlChar** attributes);
	static void charactersCallback(void* ctx, const xmlChar* value, int len);
	static void endElementNsCallback(void* ctx, const xmlChar* localname,
			const xmlChar* prefix, const xmlChar* URI);

protected:
	xmlParserCtxtPtr _xmlParserCtx;
	xmlSAXHandler _xmlSAXHandler;
};

#endif /* TestTest1_AWS_AWSRESULTPARSER_H_ */
