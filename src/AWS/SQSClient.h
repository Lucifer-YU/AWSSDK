/*
 * SQSClient.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_SQSCLIENT_H_
#define TestTest1_AWS_SQSCLIENT_H_

#include "AWSHttpClient.h"

/// Providers a client for accessing SQS service
/// Refer to:
/// http://docs.aws.amazon.com/AWSSimpleQueueService/latest/APIReference/API_Operations.html
class SQSClient: public AWSClient {
public:
	/// Creates a new instance by using given access key id, secret key and region
	SQSClient(const String& accessKeyId, const String& secretAccessKey,
			AWSRegion* region);
	/// Creates a new instance by using given credentials and region
	SQSClient(AWSCredentials* credentials, AWSRegion* region);
	virtual ~SQSClient();

	/// Creates a new queue, or returns the URL of an existing one. When you
	/// request CreateQueue, you provide a name for the queue. To successfully
	/// create a new queue, you must provide a name that is unique within the
	/// scope of your own queues.
	SQSCreateQueueResult* createQueue(const String &queueName,
			int defaultVisibilityTimeout = -1);

	/// Deletes the queue specified by the queue URL, regardless of whether the
	/// queue is empty. If the specified queue does not exist, SQS returns a
	/// successful response.
	SQSDeleteQueueResult* deleteQueue(const String &queueUrl);

	/// Returns a list of your queues. The maximum number of queues that can
	/// be returned is 1000.
	SQSListQueuesResult* listQueues(const String &queueNamePrefix = "");
	SQSListQueuesResult* listQueues(const SQSListQueuesParams* params);

	/// Delivers a message to the specified queue. You now have the ability to
	/// send large payload messages that are up to 256KB (262,144 bytes) in size.
	SQSSendMessageResult* sendMessage(const String &queueUrl,
			const String &messageBody, int delaySeconds = 0);
	SQSSendMessageResult* sendMessage(const SQSSendMessageParams* params);

	/// Retrieves one or more messages, with a maximum limit of 10 messages,
	/// from the specified queue.
	SQSReceiveMessageResult* receiveMessage(const String &queueUrl,
			int mumberOfMessages = 1, int visibilityTimeout = -1);
	SQSReceiveMessageResult* receiveMessage(
			const SQSReceiveMessageParams* params);

	/// Deletes the specified message from the specified queue. You specify the
	/// message by using the message's receipt handle and not the message ID
	/// you received when you sent the message.
	SQSDeleteMessageResult* deleteMessage(const String &queueUrl,
			const String& receiptHandle);

	/// Deletes up to ten messages from the specified queue. The result of the
	/// delete action on each message is reported individually in the response.
	SQSDeleteMessageBatchResult* deleteMessageBatch(const String &queueUrl,
			const SQSDeleteMessageBatchRequestEntryList* entries);
	SQSDeleteMessageBatchResult* deleteMessageBatch(
			const SQSDeleteMessageBatchParams* params);

protected:
	// Invokes a request and returns a response.
	AWSHttpResponse* invoke(AWSHttpRequest* request);

private:
	REF<AWSHttpClient> _webClient;
};

#endif /* TestTest1_AWS_SQSCLIENT_H_ */
