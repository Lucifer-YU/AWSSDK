/*
 * SQSResult.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#include "AWS.h"

#undef LOGT
#define LOGT(...)
#define LOG_TAG "SQSResult"

////////////////////////////////////////////////////////////////////////////////

bool SQSResultUnmarshaller::unmarshaller(AWSHttpResponse* response,
		SQSResult* result) {
	BFX_ASSERT(_result == NULL);

	_result = result;
	REF<TextReader> reader = new StringReader(response->getContent());
	bool retval = parse(reader);
	_result = NULL;

	return retval;
}

void SQSResultUnmarshaller::onStartElement(const char* localname,
		const char* prefix, const char* URI, int numNamespaces,
		const char** namespaces, int numAttributes, int numDefaulted,
		const char** attributes) {
	LOGT("onStartElement: %s", localname);

	if (stringEquals(localname, "ErrorResponse")) {
		_successful = false;
	} else if (_successful) {
		handleStartElement(localname, attributes, numAttributes);
	} else if (stringEquals(localname, "Code")) {
		setState(S_ErrorCode);
	} else if (stringEquals(localname, "Message")) {
		setState(S_ErrorMessage);
	} else if (stringEquals(localname, "RequestID")) {
		setState(S_RequestId);
	}
}
void SQSResultUnmarshaller::onCharacters(const char* chars, int numChars) {
	LOGT("onCharacters: %s", String(chars, numChars).cstr())
	;
	BFX_ASSERT(_result);

	if (_successful) {
		handleCharacters(chars, numChars);
	} else {
		String value(chars, numChars);
		if (hasState(S_ErrorCode)) {
			_result->setErrorCode(value);
		} else if (hasState(S_ErrorMessage)) {
			_result->setErrorMessage(value);
		} else if (hasState(S_RequestId)) {
			_result->setRequestId(value);
		}
	}
}
void SQSResultUnmarshaller::onEndElement(const char*localname,
		const char* prefix, const char* URI) {
	BFX_ASSERT(_result);
	LOGT("onEndElement: %s", localname);

	if (_successful) {
		handleEndElement(localname);
	} else {
		if (stringEquals(localname, "Code")) {
			unsetState(S_ErrorCode);
		} else if (stringEquals(localname, "Message")) {
			unsetState(S_ErrorMessage);
		} else if (stringEquals(localname, "RequestID")) {
			unsetState(S_RequestId);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

SQSReceiveMessageResult* SQSReceiveMessageResultUnmarshaller::unmarshall(
		AWSHttpResponse* response) {
	REF<SQSReceiveMessageResult> result = new SQSReceiveMessageResult();
	if (unmarshaller(response, result))
		result->autorelease();
	else
		result = NULL;

	return result;
}

void SQSReceiveMessageResultUnmarshaller::handleStartElement(
		const char* localname, const char** attributes, int numAttributes) {
	SQSReceiveMessageResult* result = (SQSReceiveMessageResult*) _result;
	if (stringEquals(localname, "ReceiveMessageResponse")) {
		// XXX Nothing to do
	} else if (stringEquals(localname, "Message")) {
		_curMessage = new SQSMessage();
		result->getMessages()->addLast(_curMessage);
	} else if (stringEquals(localname, "MessageId")) {
		setState(S_MessageId);
	} else if (stringEquals(localname, "ReceiptHandle")) {
		setState(S_ReceiptHandle);
	} else if (stringEquals(localname, "MD5OfBody")) {
		setState(S_MD5OfMessageBody);
	} else if (stringEquals(localname, "Body")) {
		setState(S_Body);
	} else if (stringEquals(localname, "MetaData")) {
		setState(S_MetaData);
	}
}
void SQSReceiveMessageResultUnmarshaller::handleCharacters(const char* chars,
		int numChars) {
	String value(chars, numChars);
	if (hasState(S_MessageId)) {
		_curMessage->messageId = value;
	} else if (hasState(S_ReceiptHandle)) {
		_curMessage->receiptHandle = value;
	} else if (hasState(S_MD5OfMessageBody)) {
		_curMessage->MD5OfBody = value;
	} else if (hasState(S_MetaData)) {
		// TODO :)
		// uint64_t lMetaData = strtoul(value.cstr(), NULL, 0);
		// _curMessage._metaData = value;
	} else if (hasState(S_Body)) {
		// TODO partial & base64 decode support
		_curMessage->body += value;
	}
}
void SQSReceiveMessageResultUnmarshaller::handleEndElement(
		const char* localname) {
	if (stringEquals(localname, "MessageId")) {
		unsetState(S_MessageId);
	} else if (stringEquals(localname, "ReceiptHandle")) {
		unsetState(S_ReceiptHandle);
	} else if (stringEquals(localname, "MD5OfBody")) {
		unsetState(S_MD5OfMessageBody);
	} else if (stringEquals(localname, "MetaData")) {
		unsetState(S_MetaData);
	} else if (stringEquals(localname, "Body")) {
		unsetState(S_Body);
		// TODO partial & base64 decode support
	}
}

////////////////////////////////////////////////////////////////////////////////

SQSListQueuesResult* SQSListQueuesResultUnmarshaller::unmarshall(
		AWSHttpResponse* response) {
	REF<SQSListQueuesResult> result = new SQSListQueuesResult();
	if (unmarshaller(response, result))
		result->autorelease();
	else
		result = NULL;

	return result;
}

void SQSListQueuesResultUnmarshaller::handleStartElement(const char* localname,
		const char** attributes, int numAttributes) {
	if (stringEquals(localname, "ListQueuesResponse")) {
		// XXX Nothing to do
	} else if (stringEquals(localname, "QueueUrl")) {
		setState(S_QueueUrl);
	}
}
void SQSListQueuesResultUnmarshaller::handleCharacters(const char* chars,
		int numChars) {
	if (hasState(S_QueueUrl)) {
		String queueURL((const char*) chars, numChars);
		SQSListQueuesResult* result = (SQSListQueuesResult*) _result;
		result->getQueueUrls()->addLast(queueURL);
	}
}
void SQSListQueuesResultUnmarshaller::handleEndElement(const char* localname) {
	if (stringEquals(localname, "QueueUrl")) {
		unsetState(S_QueueUrl);
	}
}

SQSSendMessageResult* SQSSendMessageResultUnmarshaller::unmarshall(
		AWSHttpResponse* response) {
	REF<SQSSendMessageResult> result = new SQSSendMessageResult();
	if (unmarshaller(response, result))
		result->autorelease();
	else
		result = NULL;

	return result;
}

void SQSSendMessageResultUnmarshaller::handleStartElement(const char* localname,
		const char** attributes, int numAttributes) {
	if (stringEquals(localname, "SendMessageResponse")) {
		// XXX Nothing to do
	} else if (stringEquals(localname, "MessageId")) {
		setState(S_MessageId);
	} else if (stringEquals(localname, "MD5OfMessageBody")) {
		setState(S_MD5OfMessageBody);
	}
}
void SQSSendMessageResultUnmarshaller::handleCharacters(const char* chars,
		int numChars) {
	SQSSendMessageResult* result = (SQSSendMessageResult*) _result;
	String value(chars, numChars);
	if (hasState(S_MessageId)) {
		result->setMessageId(value);
	} else if (hasState(S_MD5OfMessageBody)) {
		result->setMD5OfMessageBody(value);
	}
}
void SQSSendMessageResultUnmarshaller::handleEndElement(const char* localname) {
	if (stringEquals(localname, "MessageId")) {
		unsetState(S_MessageId);
	} else if (stringEquals(localname, "MD5OfMessageBody")) {
		unsetState(S_MD5OfMessageBody);
	}
}

SQSDeleteMessageBatchResult* SQSDeleteMessageBatchResultUnmarshaller::unmarshall(
		AWSHttpResponse* response) {
	REF<SQSDeleteMessageBatchResult> result = new SQSDeleteMessageBatchResult();
	if (unmarshaller(response, result))
		result->autorelease();
	else
		result = NULL;

	return result;
}

void SQSDeleteMessageBatchResultUnmarshaller::handleStartElement(
		const char* localname, const char** attributes, int numAttributes) {
	if (stringEquals(localname, "DeleteMessageBatchResponse")) {
		// XXX Nothing to do
	} else if (stringEquals(localname, "DeleteMessageBatchResultEntry")) {
		_curResultEntry = new SQSDeleteMessageBatchResultEntry();
		setState(S_BatchResultEntry);
	} else if (stringEquals(localname, "BatchResultErrorEntry")) {
		_curErrorResultEntry = new SQSBatchResultErrorEntry();
		setState(S_BatchResultErrorEntry);
	} else if (stringEquals(localname, "Id")) {
		setState(S_MessageId);
	} else if (stringEquals(localname, "Code")) {
		setState(S_BatchResultErrorCode);
	} else if (stringEquals(localname, "Message")) {
		setState(S_BatchResultErrorMessage);
	} else if (stringEquals(localname, "SenderFault")) {
		setState(S_BatchResultErrorSenderFault);
	}
}

void SQSDeleteMessageBatchResultUnmarshaller::handleCharacters(
		const char* chars, int numChars) {
	String value(chars, numChars);
	if (hasState(S_BatchResultErrorEntry)) {
		if (hasState(S_MessageId)) {
			_curErrorResultEntry->id = value;
		} else if (hasState(S_BatchResultErrorCode)) {
			_curErrorResultEntry->code = value;
		} else if (hasState(S_BatchResultErrorMessage)) {
			_curErrorResultEntry->message = value;
		} else if (hasState(S_BatchResultErrorSenderFault)) {
			LOGW("SenderFault=%s", (const char*)value);
			_curErrorResultEntry->senderFault = (value.compareTo("true", true)
					== 0);
		}
	} else if (hasState(S_BatchResultEntry)) {
		if (hasState(S_MessageId)) {
			_curResultEntry->id = value;
		}
	}
}

void SQSDeleteMessageBatchResultUnmarshaller::handleEndElement(
		const char* localname) {
	if (stringEquals(localname, "DeleteMessageBatchResultEntry")) {
		unsetState(S_BatchResultEntry);
		_curResultEntry = NULL;
	} else if (stringEquals(localname, "BatchResultErrorEntry")) {
		unsetState(S_BatchResultErrorEntry);
		_curErrorResultEntry = NULL;
	} else if (stringEquals(localname, "Id")) {
		unsetState(S_MessageId);
	} else if (stringEquals(localname, "Code")) {
		unsetState(S_BatchResultErrorCode);
	} else if (stringEquals(localname, "Message")) {
		unsetState(S_BatchResultErrorMessage);
	} else if (stringEquals(localname, "SenderFault")) {
		unsetState(S_BatchResultErrorSenderFault);
	}
}
