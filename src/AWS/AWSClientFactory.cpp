/*
 * AWSClientFactory.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#include "AWS.h"

#define LOG_TAG "AWSClientFactory"

AWSClientFactory::AWSClientFactory() {
	_region = AWSRegion::getRegion("cn-north-1");
	BFX_ASSERT(_region);
}

AWSClientFactory::~AWSClientFactory() {
}

void AWSClientFactory::setRegion(AWSRegion* region) {
	BFX_ASSERT(region);
	_region = region;
}

SQSClient* AWSClientFactory::createSQSClient(
		const String& accessKeyId, const String& secretAccessKey) const {
	BFX_ASSERT(!accessKeyId.isEmpty());
	BFX_ASSERT(!secretAccessKey.isEmpty());

	REF<SQSClient> client = new SQSClient(accessKeyId, secretAccessKey,
			_region);
	// TODO check the connection is valid???
	client->autorelease();
	return client;
}
