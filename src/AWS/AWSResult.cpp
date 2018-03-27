/*
 * AWSResultParser.cpp
 *
 *  Created on: Jan 8, 2015
 *      Author: Lucifer
 */

#include <string.h>

#include "AWSResult.h"

////////////////////////////////////////////////////////////////////////////////

AWSResultUnmarshaller::AWSResultUnmarshaller() {
	// Initializes SAX handler.
	memset(&_xmlSAXHandler, 0, sizeof(_xmlSAXHandler));
	_xmlSAXHandler.initialized = XML_SAX2_MAGIC;
	_xmlSAXHandler.startElementNs = AWSResultUnmarshaller::startElementNsCallback;
	_xmlSAXHandler.characters = AWSResultUnmarshaller::charactersCallback;
	_xmlSAXHandler.endElementNs = AWSResultUnmarshaller::endElementNsCallback;

	// Creates XML parser context.
	_xmlParserCtx = xmlCreatePushParserCtxt(&_xmlSAXHandler, this, NULL, 0, 0);
}

AWSResultUnmarshaller::~AWSResultUnmarshaller() {
	xmlFreeParserCtxt(_xmlParserCtx);
}

bool AWSResultUnmarshaller::parse(TextReader* reader) {
	BFX_ASSERT(reader);

	const int bufSize = 1024;
	char buf[bufSize];
	int charsRead;
	int ret = XML_ERR_OK;
	while ((charsRead = reader->read(buf, 0, bufSize)) > 0) {
		ret = xmlParseChunk(_xmlParserCtx, buf, charsRead,
				(charsRead < bufSize) ? 1 : 0);
		if (ret != XML_ERR_OK)
			break;
	}

	return (ret == XML_ERR_OK) ? true : false;
}

void AWSResultUnmarshaller::startElementNsCallback(void* ctx,
		const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
		int nb_namespaces, const xmlChar** namespaces, int nb_attributes,
		int nb_defaulted, const xmlChar** attributes) {
	BFX_ASSERT(ctx);

	AWSResultUnmarshaller* handler = static_cast<AWSResultUnmarshaller*>(ctx);
	handler->onStartElement((const char*) localname, (const char*) prefix,
			(const char*) URI, nb_namespaces, (const char**) namespaces,
			nb_attributes, nb_defaulted, (const char**) attributes);
}

void AWSResultUnmarshaller::charactersCallback(void* ctx, const xmlChar* value,
		int len) {
	BFX_ASSERT(ctx);

	AWSResultUnmarshaller* handler = static_cast<AWSResultUnmarshaller*>(ctx);
	handler->onCharacters((const char*) value, len);
}

void AWSResultUnmarshaller::endElementNsCallback(void* ctx, const xmlChar* localname,
		const xmlChar* prefix, const xmlChar* URI) {
	BFX_ASSERT(ctx);

	AWSResultUnmarshaller* handler = static_cast<AWSResultUnmarshaller*>(ctx);
	handler->onEndElement((const char*) localname, (const char*) prefix,
			(const char*) URI);
}
