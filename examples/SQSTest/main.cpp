/*
 * main.cpp
 *
 *  Created on: Dec 9, 2014
 *      Author: Lucifer
 */

#include <stdio.h>
#include <AWS/AWS.h>
#include <assert.h>

#include "TestMessageContext.h"
#include "TestMessageHandshake.h"
#include "TestMessageWriter.h"

void handshakeCallback(int status, const char* psz) {
	printf("handshakeCallback(status:%d,psz:%s)\n", status, psz);
}

int main(int argc, char* argv[]) {
	// Initializes the current auto release pool.
	REFAutoreleasePool pool;

	// Create SQS client
	REF<AWSClientFactory> factory = new AWSClientFactory();
	factory->setRegion(AWSRegion::getRegion("cn-north-1"));
	SQSClient* client = factory->createSQSClient("my_accessid", "my_secretkey");

	// Get receive queue URL
	SQSListQueuesResult* lqrst = client->listQueues("my_send_queue_name");
	if (lqrst == NULL) {
		printf("Error occurs during get queue URL. error code: %d\n", client->getLastError());
		return -1;
	} else if (!lqrst->getErrorCode().isEmpty()) {
		printf("Server returns error, code: %s, message: %s\n", (const char*) lqrst->getErrorCode(), (const char*) lqrst->getErrorMessage());
		return -1;
	} else if (!lqrst->hasQueueUrls()) {
		printf("No matched queue URL found.\n");
		return -1;
	}
	String queueUrl = lqrst->getQueueUrls()->getFirst();

	// Login request.
	REF<TestMessageHandshakeRequest> message = new TestMessageHandshakeRequest();
	message->setPartnerId("zzzzzzzz");
	REF<TestMessageContext> ctx = new TestMessageContext(
			TestMessageContext::CT_Handshake, message->getRequestSerial(), "", (void*) handshakeCallback);

	REF<StringWriter> stringWriter = new StringWriter();
	REF<TestMessageWriter> msgWriter = new TestMessageWriter(stringWriter);
	if (!msgWriter->write(message)) {
		BFX_ASSERT(msgWriter->getLastError() == MSGERR_SerializationFailed);
		printf("Message serialization failed, %s.\n", msgWriter->getLastErrorMessage());
		return -1;
	}
	// Send SQS message.
	REF<SQSSendMessageParams> params = new SQSSendMessageParams(queueUrl);
	params->setMessageBody(stringWriter->getString());
	params->getMessageAttributes()->set("partnerId", "my_partnerid");
	SQSSendMessageResult* smrst = client->sendMessage(params);
	if (smrst == NULL) {
		printf("Error occurs during send SQS message. error code: %d\n", client->getLastError());
		return -1;
	} else if (!smrst->getErrorCode().isEmpty()) {
		printf("SQS server returns error, code: %s, message: %s\n", (const char*) smrst->getErrorCode(), (const char*) smrst->getErrorMessage());
		return -1;
	}

	return 0;
}
