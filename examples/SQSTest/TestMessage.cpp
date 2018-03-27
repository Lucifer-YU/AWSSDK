/*
 * TestMessage.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Lucifer
 */

#include "TestMessage.h"

#define LOG_TAG "Message"

JSONObject* TestMessage::serialize() const {
	_lastErrorMessage.setEmpty();

	// message header
	REF<JSONObject> header = serializeHeader();
	if (header == NULL)
		return NULL;
	// message body
	REF<JSONObject> body = serializeBody();
	if (body == NULL)
		return NULL;
	// message root object
	REF<JSONObject> root = new JSONObject();
	root->setProperty("header", header);
	root->setProperty("body", body);
	root->autorelease();
	return root;
}

bool TestMessage::deserialize(JSONObject* root) {
	BFX_ASSERT(root);

	_lastErrorMessage.setEmpty();

	JSONNode* status = root->getProperty("header");
	if (status == NULL || status->getNodeType() != JSONNode::JNT_Object) {
		_lastErrorMessage = String::format(TESTMSG_FAIL_CAST, "header");
		return false;
	}
	JSONNode* body = root->getProperty("body");
	if (body == NULL || body->getNodeType() != JSONNode::JNT_Object) {
		_lastErrorMessage = String::format(TESTMSG_FAIL_CAST, "body");
		return false;
	}

	if (!deserializeHeader((JSONObject*) status))
		return false;
	if (!deserializeBody((JSONObject*) body))
		return false;

	return true;
}

JSONObject* TestMessage::serializeHeader() const {
	REF<JSONObject> status = new JSONObject();

	REF<JSONNumber> node1 = new JSONNumber(_seq);
	status->setProperty("seq", node1);
	REF<JSONNumber> node2 = new JSONNumber((int) getType());
	status->setProperty("type", node2);

	status->autorelease();
	return status;
}

bool TestMessage::deserializeHeader(JSONObject* header) {
	BFX_ASSERT(header != NULL);

	JSONNode* node1 = header->getProperty("seq");
	if (!JSONTryCast<uint64_t>(node1, _seq)) {
		_lastErrorMessage = String::format(TESTMSG_FAIL_CAST, "seq");
		return false;
	}

	JSONNode* node2 = header->getProperty("type");
	int type;
	if (!JSONTryCast<int>(node2, type)) {
		_lastErrorMessage = String::format(TESTMSG_FAIL_CAST, "type");
		return false;
	}
	if (type != getType()) {
		LOGE("Message type mismatch. Expect: %d, but: %d.", getType(), type);
		return false;
	}

	return true;
}

bool TestMessage::tryGetMessageType(JSONObject* root, TestMessage::MessageType& type) {
	BFX_ASSERT(root != NULL && root->getNodeType() == JSONNode::JNT_Object);

	JSONNode* header = root->getProperty("header");
	if (header == NULL || header->getNodeType() != JSONNode::JNT_Object)
		return false;
	JSONNode* headerType = ((JSONObject*) header)->getProperty("type");
	int32_t headerTypeVal;
	if (!JSONTryCast<int32_t>(headerType, headerTypeVal)) {
		return false;
	}
	type = (TestMessage::MessageType) headerTypeVal;
	return true;
}

// Generates a non-repetitive (milliseconds since 1970 with three-digit number) timestamp.
uint64_t TestMessage::nextSeq() {
	static int __seq = 0;
	uint64_t timestamp = DateTime::currentMillisecondsSince1970();
	timestamp *= 1000;
	timestamp += (__seq++) % 1000;
	return timestamp;
}

JSONObject* TestMessageRequest::serializeHeader() const {
	JSONObject* header = TestMessage::serializeHeader();
	if (header != NULL) {
		REF<JSONNumber> node1 = new JSONNumber(_requestSerial);
		header->setProperty("requestSerial", node1);
		header->setProperty("partnerId",
				_partnerId.isEmpty() ?
						(JSONNode*) JSONNull::Instance() :
						(JSONNode*) new JSONString(_partnerId));
	}
	return header;
}
bool TestMessageRequest::deserializeHeader(JSONObject* header) {
	bool result = TestMessage::deserializeHeader(header);
	if (result) {
		JSONNode* node1 = header->getProperty("requestSerial");
		if (!JSONTryCast<uint64_t>(node1, _requestSerial)) {
			_lastErrorMessage = String::format(TESTMSG_FAIL_CAST,
					"requestSerial");
			return false;
		}
		JSONNode* node2 = header->getProperty("partnerId");
		if (!JSONTryCast<String>(node2, _partnerId)) {
			_lastErrorMessage = String::format(TESTMSG_FAIL_CAST, "partnerId");
			return false;
		}
	}
	return result;
}

uint64_t TestMessageRequest::__nextSerial = 0;

uint64_t TestMessageRequest::nextRequestSerial() {
	return (++__nextSerial);
}

JSONObject* TestMessageResponse::serializeHeader() const {
	JSONObject* header = TestMessage::serializeHeader();
	if (header != NULL) {
		REF<JSONNumber> node1 = new JSONNumber(_requestSerial);
		header->setProperty("requestSerial", node1);
	}
	return header;
}
bool TestMessageResponse::deserializeHeader(JSONObject* header) {
	bool success = TestMessage::deserializeHeader(header);
	if (success) {
		JSONNode* node1 = header->getProperty("requestSerial");
		if (!JSONTryCast<uint64_t>(node1, _requestSerial)) {
			_lastErrorMessage = String::format(TESTMSG_FAIL_CAST, "requestSerial");
			return false;
		}
	}
	return success;
}
