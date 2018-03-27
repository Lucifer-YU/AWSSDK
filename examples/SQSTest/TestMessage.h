/*
 * TestMessage.h
 *
 *  Created on: Dec 19, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_MESSAGES_MESSAGE_H_
#define __TEST_MESSAGES_MESSAGE_H_

#include "JSON/JSON.h"

class JSONSerializable: public REFObject {
public:
	virtual ~JSONSerializable() {
	}

	virtual JSONObject* serialize() const = 0;
	virtual bool deserialize(JSONObject* obj) = 0;

	const char* getLastErrorMessage() const {
		return _lastErrorMessage;
	}
	bool isValid() const {
		return _lastErrorMessage.isEmpty();
	}
protected:
	mutable String _lastErrorMessage;
};

enum MessageError {
	MSGERR_Success = 0,
	MSGERR_SerializationFailed = 1,
	MSGERR_DeSerializationFailed = 2,
	MSGERR_WriteError = 3,
	MSGERR_ReadError = 4,
	MSGERR_BadFormat = 5,
};

class TestMessage: public JSONSerializable {
public:
	enum MessageType {
		MT_ErrorResponse = -1,
		MT_HandshakeRequest = 0,
		MT_HandshakeResponse = 2,
		MT_PayRequest = 3,
		MT_PayResponse = 4,
		MT_AdvertisingRequest = 5,
		MT_AdvertisingResponse = 6
	};
public:
	TestMessage() {
		renewSeq();
	}
	virtual ~TestMessage() {
	}

	void renewSeq() {
		_seq = nextSeq();
	}

	uint64_t getSeq() const {
		return _seq;
	}
	uint64_t getTimestamp() const {
		return (_seq / 1000);	// 1970 UTC
	}
	bool isObsoleted() const {
		// The message only available in 24 hours.
		const int MillisecPerDay = 24 * 60 * 60 * 1000;
		uint64_t msgTS = getTimestamp();
		uint64_t curTS = DateTime::currentMillisecondsSince1970();
		return ((curTS - msgTS) / MillisecPerDay);
	}

	virtual MessageType getType() const = 0;
	virtual JSONObject* serialize() const;
	virtual bool deserialize(JSONObject* root);

	/// Attempt to get message type from given root JSON object.
	/// @return False if message format unexpected, otherwise true.
	static bool tryGetMessageType(JSONObject* root, MessageType& type);

protected:
	virtual JSONObject* serializeHeader() const;
	virtual bool deserializeHeader(JSONObject* header);

	virtual JSONObject* serializeBody() const = 0;
	virtual bool deserializeBody(JSONObject* body) = 0;

	// Generates a non-repetitive (milliseconds since 1970 with three-digit
	// number) sequence.
	static uint64_t nextSeq();

private:
	uint64_t _seq;
};

class TestMessageRequest: public TestMessage {
public:
	TestMessageRequest() {
		renewRequestSerial();
	}
	uint64_t getRequestSerial() const {
		return _requestSerial;
	}
	void renewRequestSerial() {
		_requestSerial = nextRequestSerial();
	}
	String getPrtnerId() const {
		return _partnerId;
	}
	void setPartnerId(const String& partnerId) {
		_partnerId = partnerId;
	}

	//static void resetRequestSerial();

protected:
	virtual JSONObject* serializeHeader() const;
	virtual bool deserializeHeader(JSONObject* header);

	static uint64_t nextRequestSerial();

private:
	static uint64_t __nextSerial;	// Uses to generate serial

	uint64_t _requestSerial;
	String _partnerId;
};

class TestMessageResponse: public TestMessage {
public:
	uint64_t getRequestSerial() const {
		return _requestSerial;
	}
	void setRequestSerial(uint64_t serial) {
		_requestSerial = serial;
	}
protected:
	virtual JSONObject* serializeHeader() const;
	virtual bool deserializeHeader(JSONObject* status);
private:
	uint64_t _requestSerial;
};


#define TESTMSG_FAIL_CAST "Property name:'%s' missing or mismatch"

#endif /* __TEST_MESSAGES_MESSAGE_H_ */
