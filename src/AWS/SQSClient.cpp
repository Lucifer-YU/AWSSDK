/*
 * SQSClient.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#include "AWS.h"
#include "SQSResult.h"

#define LOG_TAG "SQSClient"

SQSClient::SQSClient(const String& accessKeyId,
		const String& secretAccessKey, AWSRegion* region) :
		AWSClient("sqs", new AWSCredentials(accessKeyId, secretAccessKey),
				region) {
	_webClient = new AWSHttpClient();
}

SQSClient::SQSClient(AWSCredentials* credentials, AWSRegion* region) :
		AWSClient("sqs", credentials, region) {
	_webClient = new AWSHttpClient();
}

SQSClient::~SQSClient() {
}

SQSCreateQueueResult* SQSClient::createQueue(const String& queueName,
		int defaultVisibilityTimeout) {
	BFX_ASSERT(false);	// not implemented.
	return NULL;
}

SQSDeleteQueueResult* SQSClient::deleteQueue(const String& queueUrl) {
	BFX_ASSERT(false);	// not implemented.
	return NULL;
}

SQSListQueuesResult* SQSClient::listQueues(const String& queueNamePrefix) {
	REF<SQSListQueuesParams> params = new SQSListQueuesParams(queueNamePrefix);
	return listQueues(params);
}

SQSListQueuesResult* SQSClient::listQueues(const SQSListQueuesParams* params) {
	BFX_ASSERT(params);
	AWSHttpRequest* request = SQSListQueuesParmsMarshaller().marshall(params);
	if (request == NULL) {
		_lastError = AWSE_InvalidArguments;
		LOGE("Failed to initialize request, invalid argument(s).");
		return NULL;
	}

	AWSHttpResponse* response = invoke(request);
	if (response == NULL) {
		// NOTE The error code already been set.
		return NULL;
	}

	SQSListQueuesResultUnmarshaller().unmarshall(response);
	SQSListQueuesResult* result = SQSListQueuesResultUnmarshaller().unmarshall(
			response);
	if (result == NULL) {
		_lastError = AWSE_ParseXMLFailed;
		LOGE("Error occurs during parse response body.");
	}

	return result;
}

SQSSendMessageResult* SQSClient::sendMessage(const String& queueUrl,
		const String &messageBody, int delaySeconds) {
	REF<SQSSendMessageParams> params = new SQSSendMessageParams(queueUrl);
	params->setMessageBody(messageBody);
	params->setDelaySeconds(delaySeconds);
	return sendMessage(params);
}

SQSSendMessageResult* SQSClient::sendMessage(
		const SQSSendMessageParams* params) {
	BFX_ASSERT(params);
	REF<AWSHttpRequest> request = SQSSendMessageParamsMarshaller().marshall(
			params);
	if (request == NULL) {
		_lastError = AWSE_InvalidArguments;
		LOGE("Failed to initialize request, invalid argument(s).");
		return NULL;
	}

	AWSHttpResponse* response = invoke(request);
	if (response == NULL) {
		return NULL;	// NOTE The error code already been set.
	}

	SQSSendMessageResult* result =
			SQSSendMessageResultUnmarshaller().unmarshall(response);
	if (result == NULL) {
		_lastError = AWSE_ParseXMLFailed;
		LOGE("Error occurs during parse response body.")
		;
	}

	return result;
}

SQSReceiveMessageResult* SQSClient::receiveMessage(const String& queueUrl,
		int mumberOfMessages, int visibilityTimeout) {
	REF<SQSReceiveMessageParams> params = new SQSReceiveMessageParams(queueUrl);
	params->setMaxNumberOfMessages(mumberOfMessages);
	params->setVisibilityTimeout(visibilityTimeout);
	return receiveMessage(params);
}

SQSReceiveMessageResult* SQSClient::receiveMessage(
		const SQSReceiveMessageParams* params) {
	BFX_ASSERT(params != NULL);

	AWSHttpRequest* request = SQSReceiveMessageParamsMarshaller().marshall(
			params);
	if (request == NULL) {
		_lastError = AWSE_InvalidArguments;
		LOGE("Failed to initialize request, invalid argument(s).");
		return NULL;
	}

	AWSHttpResponse* response = invoke(request);
	if (response == NULL) {
		return NULL;	// NOTE The error code already set in invoke(...).
	}

	SQSReceiveMessageResult* result =
			SQSReceiveMessageResultUnmarshaller().unmarshall(response);
	if (result == NULL) {
		_lastError = AWSE_ParseXMLFailed;
		LOGE("Error occurs during parse response body.");
	}

	return result;
}

SQSDeleteMessageBatchResult* SQSClient::deleteMessageBatch(
		const String &queueUrl,
		const SQSDeleteMessageBatchRequestEntryList* entries) {

	REF<SQSDeleteMessageBatchParams> params = new SQSDeleteMessageBatchParams(
			queueUrl);
	for (SQSDeleteMessageBatchRequestEntryList::PENTRY entry =
			entries->getFirstEntry(); entry != NULL;
			entry = entries->getNextEntry(entry)) {
		params->getEntries()->addLast(entry->value);
	}
	return deleteMessageBatch(params);
}

SQSDeleteMessageBatchResult* SQSClient::deleteMessageBatch(
		const SQSDeleteMessageBatchParams* params) {
	BFX_ASSERT(params);
	AWSHttpRequest* request = SQSDeleteMessageBatchParamsMarshaller().marshall(
			params);
	if (request == NULL) {
		_lastError = AWSE_InvalidArguments;
		LOGE("Failed to initialize request, invalid argument(s).");
		return NULL;
	}

	AWSHttpResponse* response = invoke(request);
	if (response == NULL) {
		return NULL;	// NOTE The error code already been set.
	}

	SQSDeleteMessageBatchResult* result =
			SQSDeleteMessageBatchResultUnmarshaller().unmarshall(response);
	if (result == NULL) {
		_lastError = AWSE_ParseXMLFailed;
		LOGE("Error occurs during parse response body.");
	}

	return result;
}

SQSDeleteMessageResult* SQSClient::deleteMessage(const String &queueUrl,
		const String& receiptHandle) {
	BFX_ASSERT(false);	// not implemented.
	return NULL;
}

AWSHttpResponse* SQSClient::invoke(AWSHttpRequest* request) {
	request->setEndpoint(getEndpoint());
	// TODO more initialization here
	_webClient->setCredentials(_credentials);
	_webClient->setSigner(_signer);
	AWSHttpResponse* response = _webClient->execute(request);
	if (response == NULL) {
		_lastError = _webClient->getLastError();
		LOGE("Failed to send request.");
		return NULL;
	}

	LOGT("RESP_CONTENT=%s", response->getContent().cstr());

	return response;
}
