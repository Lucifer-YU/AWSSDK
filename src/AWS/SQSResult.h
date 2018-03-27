/*
 * SQSResult.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_SQSRESULT_H_
#define TestTest1_AWS_SQSRESULT_H_

#include "AWSResult.h"

class AWSHttpResponse;

class SQSResult: public AWSResult {
public:
	SQSResult() {
	}
	virtual ~SQSResult() {
	}

	const String& getErrorCode() const {
		return _errorCode;
	}
	const String& getErrorMessage() const {
		return _errorMessage;
	}
	const String& getRequestId() const {
		return _requestId;
	}
	void setErrorCode(const String& errorCode) {
		_errorCode = errorCode;
	}
	void setErrorMessage(const String& errorMessage) {
		_errorMessage = errorMessage;
	}
	void setRequestId(const String& requestId) {
		_requestId = requestId;
	}

protected:
	String _errorCode;
	String _errorMessage;
	String _requestId;
};

/// The result contains QueueUrl element on CreateQueueResult.
class SQSCreateQueueResult: public SQSResult {
	SQSCreateQueueResult() {
	}
	virtual ~SQSCreateQueueResult() {
	}
	const String& getQueueUrl() const {
		return _queueUrl;
	}
	void setQueueUrl(const String& queueUrl) {
		_queueUrl = queueUrl;
	}
private:
	String _queueUrl;
};

class SQSDeleteQueueResult: public SQSResult {
	// TODO
};

/// The result contains QueueUrls list on ListQueues.
class SQSListQueuesResult: public SQSResult {
public:
	SQSListQueuesResult() {
	}
	AWSStringList* getQueueUrls() const {
		if (_queueUrls == NULL)
			const_cast<SQSListQueuesResult*>(this)->_queueUrls =
					new AWSStringList();
		return _queueUrls;
	}
	bool hasQueueUrls() const {
		return ((_queueUrls != NULL) && (_queueUrls->getSize() > 0));
	}
private:
	REF<AWSStringList> _queueUrls;
};

/// The result contains MessageId/MD5OfMessageBody/MD5OfMessageAttributes on
/// SendMessage.
class SQSSendMessageResult: public SQSResult {
public:
	SQSSendMessageResult() {
	}
	virtual ~SQSSendMessageResult() {
	}
	const String& getMessageId() const {
		return _messageId;
	}
	void setMessageId(const String& messageId) {
		_messageId = messageId;
	}
	const String& getMD5OfMessageBody() const {
		return _MD5OfMessageBody;
	}
	void setMD5OfMessageBody(const String& MD5OfMessageBody) {
		_MD5OfMessageBody = MD5OfMessageBody;
	}
	const String& getMD5OfMessageAttributes() const {
		return _MD5OfMessageAttributes;
	}
	void setMD5OfMessageAttributes(const String& MD5OfMessageAttributes) {
		_MD5OfMessageAttributes = MD5OfMessageAttributes;
	}
private:
	String _messageId;
	String _MD5OfMessageBody;
	String _MD5OfMessageAttributes;
};

/// The result contains Messages tags on ReceiveMessage.
class SQSReceiveMessageResult: public SQSResult {
public:
	SQSReceiveMessageResult() {
	}
	virtual ~SQSReceiveMessageResult() {
	}
	/// Gets a value indicating message list not empty.
	bool hasMessages() const {
		return ((_messages != NULL) && (_messages->getSize() > 0));
	}
	/// Gets a list of messages.
	SQSMessageList* getMessages() const {
		if (_messages == NULL)
			const_cast<SQSReceiveMessageResult*>(this)->_messages =
					new SQSMessageList();
		return _messages;
	}

private:
	REF<SQSMessageList> _messages;
};

class SQSDeleteMessageResult: public SQSResult {

};

/// The result contains both successful and failed tags on DeleteMessageBatch.
class SQSDeleteMessageBatchResult: public SQSResult {
public:
	SQSDeleteMessageBatchResult() {
	}
	virtual ~SQSDeleteMessageBatchResult() {
	}
	SQSDeleteMessageBatchResultEntryList* getSuccessful() const {
		if (_successful == NULL)
			const_cast<SQSDeleteMessageBatchResult*>(this)->_successful =
					new SQSDeleteMessageBatchResultEntryList();
		return _successful;
	}
	SQSBatchResultErrorEntryList* getFailed() const {
		if (_failed == NULL)
			const_cast<SQSDeleteMessageBatchResult*>(this)->_failed =
					new SQSBatchResultErrorEntryList();
		return _failed;
	}
private:
	REF<SQSDeleteMessageBatchResultEntryList> _successful;
	REF<SQSBatchResultErrorEntryList> _failed;
};


class SQSResultUnmarshaller: public AWSResultUnmarshaller {
public:
	SQSResultUnmarshaller() :
			_state(0), _successful(true), _result(NULL) {
	}
	virtual ~SQSResultUnmarshaller() {
	}

protected:
	bool unmarshaller(AWSHttpResponse* response, SQSResult* result);

	void setState(uint64_t state) {
		_state |= state;
	}
	bool hasState(uint64_t state) {
		return (_state & state) == state;
	}
	void unsetState(uint64_t state) {
		_state ^= state;
	}
	bool isSuccessful() const {
		return _successful;
	}

	virtual void handleStartElement(const char* localname,
			const char** attributes, int numAttributes) = 0;
	virtual void handleCharacters(const char* value, int len) = 0;
	virtual void handleEndElement(const char* localname) = 0;

	virtual void onStartElement(const char* localname, const char* prefix,
			const char* URI, int numNamespaces, const char** namespaces,
			int numAttributes, int numDefaulted, const char** attributes);
	virtual void onCharacters(const char* value, int len);
	virtual void onEndElement(const char*localname, const char* prefix,
			const char* URI);

protected:
	enum State {
		S_ErrorCode = 1, //
		S_ErrorMessage = 2, //
		S_RequestId = 4, //
		S_HostId = 8, //
		S_QueueUrl = 16, //
		S_MessageId = 32, //
		S_MD5OfMessageBody = 64, //
		S_ReceiptHandle = 128, //
		S_Body = 256, //
		S_MetaData = 512, //
		S_BatchResultEntry = 1024, //
		S_BatchResultErrorEntry = 2048, //
		S_BatchResultErrorCode = 4096, //
		S_BatchResultErrorMessage = 8192, //
		S_BatchResultErrorSenderFault = 16384, //
	};

	uint64_t _state;
	bool _successful;

	SQSResult* _result;
};

class SQSReceiveMessageResultUnmarshaller : public SQSResultUnmarshaller {
public:
	SQSReceiveMessageResult* unmarshall(AWSHttpResponse* response);

protected:
	virtual void handleStartElement(const char* localname,
			const char** attributes, int numAttributes);
	virtual void handleCharacters(const char* chars, int numChars);
	virtual void handleEndElement(const char* localname);

protected:
	REF<SQSMessage> _curMessage;
};

class SQSListQueuesResultUnmarshaller : public SQSResultUnmarshaller {
public:
	SQSListQueuesResult* unmarshall(AWSHttpResponse* response);

protected:
	virtual void handleStartElement(const char* localname,
			const char** attributes, int numAttributes);
	virtual void handleCharacters(const char* chars, int numChars);
	virtual void handleEndElement(const char* localname);
};

class SQSSendMessageResultUnmarshaller : public SQSResultUnmarshaller {
public:
	SQSSendMessageResult* unmarshall(AWSHttpResponse* response);

protected:
	virtual void handleStartElement(const char* localname,
			const char** attributes, int numAttributes);
	virtual void handleCharacters(const char* chars, int numChars);
	virtual void handleEndElement(const char* localname);
};

class SQSDeleteMessageBatchResultUnmarshaller : public SQSResultUnmarshaller {
public:
	SQSDeleteMessageBatchResult* unmarshall(AWSHttpResponse* response);

protected:
	virtual void handleStartElement(const char* localname,
			const char** attributes, int numAttributes);
	virtual void handleCharacters(const char* chars, int numChars);
	virtual void handleEndElement(const char* localname);
protected:
	REF<SQSDeleteMessageBatchResultEntry> _curResultEntry;
	REF<SQSBatchResultErrorEntry> _curErrorResultEntry;
};

#endif /* TestTest1_AWS_SQSRESULT_H_ */
