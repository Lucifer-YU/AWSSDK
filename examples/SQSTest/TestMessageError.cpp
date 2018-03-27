/*
 * TestMessageError.cpp
 *
 *  Created on: Dec 23, 2014
 *      Author: Lucifer
 */

#include "TestMessageError.h"

JSONObject* TestMessageErrorResponse::serializeBody() const {
	REF<JSONObject> body = new JSONObject();

	REF<JSONNumber> node1 = new JSONNumber(_errorCode);
	body->setProperty("errorCode", node1);
	REF<JSONNode> node2 =
			_errorMessage.isEmpty() ?
					(JSONNode*) JSONNull::Instance() :
					(JSONNode*) new JSONString(_errorMessage);
	body->setProperty("errorMessage", node2);

	body->autorelease();
	return body;
}

bool TestMessageErrorResponse::deserializeBody(JSONObject* body) {
	BFX_ASSERT(body != NULL);

	JSONNode* node1 = body->getProperty("errorCode");
	if (JSONTryCast<int32_t>(node1, _errorCode)) {
		_lastErrorMessage = String::format(TESTMSG_FAIL_CAST, "errorCode");
		return false;
	}

	JSONNode* node2 = body->getProperty("errorMessage");
	_errorMessage = JSONCast<String>(node2);

	return true;
}
