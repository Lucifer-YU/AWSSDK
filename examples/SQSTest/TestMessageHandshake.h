/*
 * TestMessageHandshake.h
 *
 *  Created on: Dec 23, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_MESSAGES_MESSAGETEST_H_
#define __TEST_MESSAGES_MESSAGETEST_H_

#include "TestMessage.h"

/// Handshake request
class TestMessageHandshakeRequest: public TestMessageRequest {
public:
	TestMessageHandshakeRequest() {
	}
	virtual ~TestMessageHandshakeRequest() {
	}

	virtual MessageType getType() const {
		return MT_HandshakeRequest;
	};

protected:
	virtual JSONObject* serializeBody() const;
	virtual bool deserializeBody(JSONObject* body);

private:
	String _partnerId;
};

/// Handshake response
class TestMessageHandshakeResponse: public TestMessageResponse {
public:
	TestMessageHandshakeResponse() {
	}
	virtual ~TestMessageHandshakeResponse() {
	}

	virtual MessageType getType() const {
		return MT_HandshakeResponse;
	}
	const String getPartnerName() const {
		return _partnerName;
	}

protected:
	virtual JSONObject* serializeBody() const;
	virtual bool deserializeBody(JSONObject* body);

private:
	String _partnerName;
};

#endif /* __TEST_MESSAGES_MESSAGETEST_H_ */
