/*
 * AWSClientFactory.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#ifndef AWSCONNECTIONFACTORY_H_
#define AWSCONNECTIONFACTORY_H_

class SQSClient;
class AWSS3Client;
class AWSRegion;

class AWSClientFactory: public REFObject {
public:
	AWSClientFactory();
	virtual ~AWSClientFactory();

	void setRegion(AWSRegion* region);

	SQSClient* createSQSClient(const String& accessKeyId,
			const String& secretAccessKey) const;

	AWSS3Client* createS3Client(const String& accessKeyId,
			const String& secretAccessKey) const;

private:
	AWSRegion* _region;
};

#endif /* AWSCONNECTIONFACTORY_H_ */
