/*
 * AWS.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_AWS_H_
#define TestTest1_AWS_AWS_H_

#include "../Foundation/Foundation.h"

enum AWSError {
	AWSE_NoError = 0,	/// Success
	AWSE_InvalidArguments, /// Invalid argument(s)
	AWSE_SignFailed,	/// Failed to sign a message
	AWSE_UnrecognizedHttpProtocol,	/// Bad or unsupported HTTP protocol
	AWSE_HttpRequestFailed,	/// HTTP request failed
	AWSE_UnrecognizedSignerType,	/// Unsupported message signer type
	AWSE_ParseXMLFailed,	/// Invalid XML or message format
};

/// Represents a nullable, and sharable string map.
typedef REFWrapper<TreeMapT<String, String> > AWSStringMap;

typedef REFWrapper<LinkedListT<String> > AWSStringList;

#include "AWSClientFactory.h"
#include "AWSSigner.h"
#include "AWSRegion.h"
#include "AWSClient.h"
#include "SQSModel.h"
#include "SQSParams.h"
#include "SQSResult.h"
#include "SQSClient.h"
#include "S3Client.h"

#endif /* TestTest1_AWS_AWS_H_ */
