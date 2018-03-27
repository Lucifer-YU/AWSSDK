/*
 * TestMessageReader.cpp
 *
 *  Created on: Dec 26, 2014
 *      Author: Lucifer
 */

#include "TestMessageReader.h"

#define LOG_TAG "MessageReader"

TestMessageReader::TestMessageReader(TextReader* textReader) {
	BFX_ASSERT(textReader);
	_textReader = textReader;
	_lastError = MSGERR_Success;
}

TestMessageReader::~TestMessageReader() {
	_textReader->close();
}

TestMessage* TestMessageReader::read() {
	_lastError = MSGERR_Success;
	_lastErrorMessage.setEmpty();

	REF<JSONReader> jr = new JSONReader(_textReader);
	JSONNode* root = JSONNode::fromJSONReader(jr);
	if (root == NULL || root->getNodeType() != JSONNode::JNT_Object) {
		_lastError = MSGERR_ReadError;
		_lastErrorMessage = String::format(
				"%s, failed to read JSON object from reader.",
				(const char*) jr->getLastErrorMessage());
		LOGE(_lastErrorMessage);
		return NULL;
	}

	TestMessage::MessageType msgType;
	if (!TestMessage::tryGetMessageType((JSONObject*) root, msgType)) {
		_lastError = MSGERR_BadFormat;
		_lastErrorMessage = String::format(
				"Bad message format, unable to resolve the message type.");
		LOGE(_lastErrorMessage);
		return NULL;
	}
	REF<TestMessage> message;
	switch (msgType) {
	case TestMessage::MT_ErrorResponse:
		// message = new TestMessageErrorResponse();
		assert(0);
		break;
	case TestMessage::MT_HandshakeResponse:
		//  message = new TestMessageHandshakeResponse();
		assert(0);
		break;
	case TestMessage::MT_PayResponse:
		// message = new TestMessagePaymentResponse();
		assert(0);
		break;
	case TestMessage::MT_AdvertisingResponse:
		//message = new TestMessageAdvertisingResponse();
		break;
	default:
		return NULL;
	}
	BFX_ASSERT(message);

	message->deserialize((JSONObject*) root);
	if (!message->isValid()) {
		_lastError = MSGERR_DeSerializationFailed;
		_lastErrorMessage = String::format(
				"%s, Failed to de-serialize message.",
				(const char*) message->getLastErrorMessage());
		LOGE(_lastErrorMessage);
		return NULL;
	}
	message->autorelease();

	return message;
}
