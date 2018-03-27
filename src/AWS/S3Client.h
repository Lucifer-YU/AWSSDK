/*
 * S3Client.h
 *
 *  Created on: Jan 14, 2015
 *      Author: Lucifer
 */

#ifndef AWS_S3CLIENT_H_
#define AWS_S3CLIENT_H_

class S3Client: public AWSClient {
public:
	S3Client(AWSCredentials* credentials, AWSRegion* region);
	virtual ~S3Client();
};

#endif /* AWS_S3CLIENT_H_ */
