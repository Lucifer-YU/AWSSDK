/*
 * SQSModel.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */
#ifndef __AWS_SQSMODELS_H__
#define __AWS_SQSMODELS_H__

/// An SQS message.
struct SQSMessage: REFObject {
	String body;
	String MD5OfBody;
	String receiptHandle;
	String messageId;
};

/// Encloses a receipt handle and an identifier for it.
struct SQSDeleteMessageBatchRequestEntry: REFObject {
	String id;
	String receiptHandle;
};

/// Encloses the id an entry in DeleteMessageBatch.
struct SQSDeleteMessageBatchResultEntry : REFObject {
	String id;
};

/// This is used in the responses of batch API to give a detailed description
/// of the result of an action on each entry in the request.
struct SQSBatchResultErrorEntry: REFObject {
	SQSBatchResultErrorEntry() :
			senderFault(false) {
	}
	String id;
	bool senderFault;
	String code;
	String message;
};

typedef REFWrapper<LinkedListT<REF<SQSMessage> > > SQSMessageList;
typedef REFWrapper<LinkedListT<REF<SQSDeleteMessageBatchRequestEntry> > > SQSDeleteMessageBatchRequestEntryList;
typedef REFWrapper<LinkedListT<REF<SQSDeleteMessageBatchResultEntry> > > SQSDeleteMessageBatchResultEntryList;
typedef REFWrapper<LinkedListT<REF<SQSBatchResultErrorEntry> > > SQSBatchResultErrorEntryList;

#endif	// __AWS_SQSMODELS_H__
