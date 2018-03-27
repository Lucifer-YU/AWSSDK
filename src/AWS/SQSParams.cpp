/*
 * SQSParams.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: Lucifer
 */

#include "AWS.h"

AWSHttpRequest* SQSParamsMarshaller::createHttpRequest() {
	REF<AWSHttpRequest> request = new AWSHttpRequest("AmazonSQS");
	request->getParameters()->set("Version", "2012-11-05");
	request->autorelease();
	return request;
}

AWSHttpRequest* SQSReceiveMessageParamsMarshaller::marshall(
		const SQSReceiveMessageParams* params) {
	AWSHttpRequest* request = createHttpRequest();
	if (request == NULL)
		return NULL;
	request->getParameters()->set("Action", "ReceiveMessage");
	request->getParameters()->set("QueueUrl", params->getQueueUrl());
	if (params->getMaxNumberOfMessages() != -1) {
		request->getParameters()->set("MaxNumberOfMessages",
				String::format("%d", params->getMaxNumberOfMessages()));
	}
	if (params->getVisibilityTimeout() != -1) {
		request->getParameters()->set("VisibilityTimeout",
				String::format("%d", params->getVisibilityTimeout()));
	}
	if (params->getWaitTimeSeconds() != -1) {
		request->getParameters()->set("WaitTimeSeconds",
				String::format("%d", params->getWaitTimeSeconds()));
	}
	if (params->hasMessageAttributeNames()) {
		int attributeIndex = 1;
		AWSStringMap* attrNames = params->getMessageAttributeNames();
		for (AWSStringMap::PENTRY entry = attrNames->getFirstEntry();
				entry != NULL; entry = attrNames->getNextEntry(entry)) {
			request->getParameters()->set(
					String::format("MessageAttributeName.%d", attributeIndex),
					entry->value);
			attributeIndex++;
		}
	}
	return request;
}

AWSHttpRequest* SQSListQueuesParmsMarshaller::marshall(
		const SQSListQueuesParams* params) {
	AWSHttpRequest* request = createHttpRequest();
	if (request == NULL)
		return NULL;
	request->getParameters()->set("Action", "ListQueues");
	if (!params->getQueueNamePrefix().isEmpty()) {
		request->getParameters()->set("QueueNamePrefix",
				params->getQueueNamePrefix());
	}
	return request;
}

AWSHttpRequest* SQSSendMessageParamsMarshaller::marshall(
		const SQSSendMessageParams* params) {
	AWSHttpRequest* request = createHttpRequest();
	if (request == NULL)
		return NULL;
	request->getParameters()->set("Action", "SendMessage");
	request->getParameters()->set("QueueUrl", params->getQueueUrl());
	request->getParameters()->set("MessageBody", params->getMessageBody());
	if (params->getDelaySeconds() != -1) {
		request->getParameters()->set("DelaySeconds",
				String::format("%d", params->getDelaySeconds()));
	}
	if (params->hasMessageAttributes()) {
		AWSStringMap* attrs = params->getMessageAttributes();
		int messageAttributeIndex = 1;
		for (AWSStringMap::PENTRY entry = attrs->getFirstEntry(); entry != NULL;
				entry = attrs->getNextEntry(entry)) {
			request->getParameters()->set(
					String::format("MessageAttribute.%d.Name",
							messageAttributeIndex), entry->key);
			request->getParameters()->set(
					String::format("MessageAttribute.%d.Value.StringValue",
							messageAttributeIndex), entry->value);
			request->getParameters()->set(
					String::format("MessageAttribute.%d.Value.DataType",
							messageAttributeIndex), "String");
		}
	}
	return request;
}

AWSHttpRequest* SQSDeleteMessageBatchParamsMarshaller::marshall(
		const SQSDeleteMessageBatchParams* params) {
	BFX_ASSERT(params);
	AWSHttpRequest* request = createHttpRequest();
	if (request == NULL)
		return NULL;
	request->getParameters()->set("Action", "DeleteMessageBatch");
	request->getParameters()->set("QueueUrl", params->getQueueUrl());
	if (params->hasEntries()) {
		SQSDeleteMessageBatchRequestEntryList* entries = params->getEntries();
		int messageAttributeIndex = 1;
		for (SQSDeleteMessageBatchRequestEntryList::PENTRY entry =
				entries->getFirstEntry(); entry != NULL;
				entry = entries->getNextEntry(entry)) {
			request->getParameters()->set(
					String::format("DeleteMessageBatchRequestEntry.%d.Id",
							messageAttributeIndex), entry->value->id);
			request->getParameters()->set(
					String::format(
							"DeleteMessageBatchRequestEntry.%d.ReceiptHandle",
							messageAttributeIndex),
					entry->value->receiptHandle);
		}
	}
	return request;
}
