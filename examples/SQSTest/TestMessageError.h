/*
 * TestMessageError.h
 *
 *  Created on: Dec 23, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_MESSAGES_MESSAGEERROR_H_
#define __TEST_MESSAGES_MESSAGEERROR_H_

#include "TestMessage.h"

class TestMessageErrorResponse: public TestMessageResponse {
public:
	TestMessageErrorResponse() {
		_errorCode = 0;
	}
	virtual ~TestMessageErrorResponse() {
	}
	virtual MessageType getType() const {
		return MT_ErrorResponse;
	}

	// Gets the error code / message from the remote peer.
	int getErrorCode() const {
		return _errorCode;
	}
	const char* getErrorMessage() const {
		return _errorMessage;
	}

protected:
	virtual JSONObject* serializeBody() const;
	virtual bool deserializeBody(JSONObject* body);

private:
	int _errorCode;
	String _errorMessage;
};

#endif /* __TEST_MESSAGES_MESSAGEERROR_H_ */
