/*
 * S3Client.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: Lucifer
 */

#include "AWS.h"

S3Client::S3Client(AWSCredentials* credentials, AWSRegion* region) :
		AWSClient("s3", credentials, region) {
	// TODO Auto-generated constructor stub

}

S3Client::~S3Client() {
	// TODO Auto-generated destructor stub
}

