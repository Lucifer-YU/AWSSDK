/*
 * TestMessageHandshake.cpp
 *
 *  Created on: Dec 23, 2014
 *      Author: Lucifer
 */

#include "TestMessageHandshake.h"

JSONObject* TestMessageHandshakeRequest::serializeBody() const {
	JSONObject* body = new JSONObject();
	body->autorelease();
	return body;
}

bool TestMessageHandshakeRequest::deserializeBody(JSONObject* body) {
	BFX_ASSERT(body);
	return true;
}

JSONObject* TestMessageHandshakeResponse::serializeBody() const {
	JSONObject* body = new JSONObject();
	JSONNode* partnerNameNode =
			_partnerName.isEmpty() ?
					(JSONNode*) JSONNull::Instance() :
					(JSONNode*) new JSONString(_partnerName);
	body->setProperty("partnerName", partnerNameNode);
	body->autorelease();
	return body;
}

bool TestMessageHandshakeResponse::deserializeBody(JSONObject* body) {
	BFX_ASSERT(body);
	JSONNode* partnerNameNode = body->getProperty("partnerName");
	if (!JSONTryCast<String>(partnerNameNode, _partnerName)) {
		_lastErrorMessage = String::format(TESTMSG_FAIL_CAST, "partnerName");
		return false;
	}
	return true;
}
