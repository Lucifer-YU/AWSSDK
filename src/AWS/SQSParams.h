/*
 * SQSParams.h
 *
 *  Created on: Jan 21, 2015
 *      Author: Lucifer
 */

#ifndef AWS_SQSPARAMS_H_
#define AWS_SQSPARAMS_H_

class SQSParams: public REFObject {
public:
};

class SQSListQueuesParams : public SQSParams {
public:
	SQSListQueuesParams(const String& queueNamePrefix) {
		setQueueNamePrefix(queueNamePrefix);
	}
	/// Sets a string to use for filtering the list results. Only those queues
	/// whose name begins with the specified string are returned.
	void setQueueNamePrefix(const String& queueNamePrefix) {
		_queueNamePrefix = queueNamePrefix;
	}
	/// Sets a string to use for filtering the list results.
	const String& getQueueNamePrefix() const {
		return _queueNamePrefix;
	}
private:
	String _queueNamePrefix;
};

class SQSSendMessageParams: public SQSParams {
public:
	SQSSendMessageParams(const String& queueUrl) {
		setQueueUrl(queueUrl);
		_delaySeconds = -1;
	}
	/// Sets the URL of the SQS queue to take action on.
	void setQueueUrl(const String& queueUrl) {
		_queueUrl = queueUrl;
	}
	/// Gets the URL of the SQS queue to take action on.
	const String& getQueueUrl() const {
		return _queueUrl;
	}
	/// Sets the message to send. String maximum 256 KB in size. For a list of
	/// allowed characters, see the preceding important note.
	void setMessageBody(const String& messageBody) {
		_messageBody = messageBody;
	}
	/// Gets the message to send. String maximum 256 KB in size. For a list of
	/// allowed characters, see the preceding important note.
	const String& getMessageBody() const {
		return _messageBody;
	}
	/// Sets the number of seconds (0 to 900 - 15 minutes) to delay a specific
	/// message. Messages become available for processing after the delay time
	/// is finished. The default value for the queue applies.
	void setDelaySeconds(int delaySeconds) {
		_delaySeconds = delaySeconds;
	}
	/// Sets the number of seconds (0 to 900 - 15 minutes) to delay a specific
	/// message.
	int getDelaySeconds() const {
		return _delaySeconds;
	}
	bool hasMessageAttributes() const {
		return ((_messageAttributes != NULL)
				&& (_messageAttributes->getSize() > 0));
	}
	AWSStringMap* getMessageAttributes() const {
		if (_messageAttributes == NULL)
			const_cast<SQSSendMessageParams*>(this)->_messageAttributes =
					new AWSStringMap();
		return _messageAttributes;
	}
private:
	String _queueUrl;
	String _messageBody;
	int _delaySeconds;
	REF<AWSStringMap> _messageAttributes;
};

class SQSReceiveMessageParams: public SQSParams {
public:
	SQSReceiveMessageParams(const String& queueUrl) {
		setQueueUrl(queueUrl);
		_maxNumberOfMessages = -1;
		_visibilityTimeout = -1;
		_waitTimeSeconds = -1;
	}
	virtual ~SQSReceiveMessageParams() {
	}

	/// Sets the URL of the SQS queue to take action on.
	void setQueueUrl(const String& queueUrl) {
		BFX_ASSERT(!queueUrl.isEmpty());
		_queueUrl = queueUrl;
	}
	/// Gets the URL of the SQS queue to take action on.
	const String& getQueueUrl() const {
		return _queueUrl;
	}

	/// Gets a value that indicating whether has message attribute names.
	bool hasMessageAttributeNames() const {
		return ((_messageAttributeNames == NULL)
				|| (_messageAttributeNames->getSize() == 0));
	}
	/// Gets a pointer of map that containing message attribute names.
	AWSStringMap* getMessageAttributeNames() const {
		if (_messageAttributeNames == NULL) {
			const_cast<SQSReceiveMessageParams*>(this)->_messageAttributeNames =
					new AWSStringMap();
		}
		return _messageAttributeNames;
	}
	/// Gets the maximum number of messages to return.
	/// SQS will never returns more messages than this value but may return
	/// fewer. Values can be from 1 to 10. Default is 1.
	int getMaxNumberOfMessages() const {
		return _maxNumberOfMessages;
	}
	/// Sets the maximum number of messages to return.
	void setMaxNumberOfMessages(int maxNumberOfMessages) {
		_maxNumberOfMessages = maxNumberOfMessages;
	}
	/// Gets the duration (in seconds) that the received messages are hidden
	/// from subsequent retrieve requests after being retrieved by a
	/// ReceiveMessage request.
	int getVisibilityTimeout() const {
		return _visibilityTimeout;
	}
	/// Sets the duration (in seconds) that the received messages are hidden
	/// from subsequent retrieve requests after being retrieved by a
	/// ReceiveMessage request.
	void setVisibilityTimeout(int visibilityTimeout) {
		_visibilityTimeout = visibilityTimeout;
	}
	/// Gets the duration (in seconds) for which the call will wait for a
	/// message to arrive in the queue before returning. If a message is
	/// available, the call will return sooner than WaitTimeSeconds.
	int getWaitTimeSeconds() const {
		return _waitTimeSeconds;
	}
	/// Gets the duration (in seconds) for which the call will wait for a
	/// message to arrive in the queue before returning.
	void setWaitTimeSeconds(int waitTimeSeconds) {
		_waitTimeSeconds = waitTimeSeconds;
	}

private:
	// The URL of the Amazon SQS queue to take action on.
	String _queueUrl;
	// The list of message attribute names
	REF<AWSStringMap> _messageAttributeNames;
	// The maximum number of messages to return
	int _maxNumberOfMessages;
	// The duration (in seconds) that the received messages are hidden from
	// subsequent retrieve requests
	int _visibilityTimeout;
	// The duration (in seconds) for which the call will wait for a message
	// to arrive in the queue before returning. If a message is available,
	// the call will return sooner than WaitTimeSeconds.
	int _waitTimeSeconds;
};

class SQSDeleteMessageBatchParams: public SQSParams {
public:
	SQSDeleteMessageBatchParams(const String& queueUrl) {
		setQueueUrl(queueUrl);
	}
	/// Sets the URL of the Amazon SQS queue to take action on.
	void setQueueUrl(const String& queueUrl) {
		_queueUrl = queueUrl;
	}
	/// Gets the URL of the Amazon SQS queue to take action on.
	const String& getQueueUrl() const {
		return _queueUrl;
	}
	/// Gets a list of receipt handles for the messages to be deleted.
	SQSDeleteMessageBatchRequestEntryList* getEntries() const {
		if (_entries == NULL)
			const_cast<SQSDeleteMessageBatchParams*>(this)->_entries =
					new SQSDeleteMessageBatchRequestEntryList();
		return _entries;
	}
	bool hasEntries() const {
		return ((_entries != NULL) && (_entries->getSize() > 0));
	}

private:
	String _queueUrl;
	REF<SQSDeleteMessageBatchRequestEntryList> _entries;
};

class SQSParamsMarshaller {
protected:
	AWSHttpRequest* createHttpRequest();
};

class SQSReceiveMessageParamsMarshaller: public SQSParamsMarshaller {
public:
	AWSHttpRequest* marshall(const SQSReceiveMessageParams* params);
};

class SQSListQueuesParmsMarshaller: public SQSParamsMarshaller {
public:
	AWSHttpRequest* marshall(const SQSListQueuesParams* params);
};

class SQSSendMessageParamsMarshaller: public SQSParamsMarshaller {
public:
	AWSHttpRequest* marshall(const SQSSendMessageParams* params);
};

class SQSDeleteMessageBatchParamsMarshaller: public SQSParamsMarshaller {
public:
	AWSHttpRequest* marshall(const SQSDeleteMessageBatchParams* params);
};

#endif /* AWS_SQSPARAMS_H_ */
