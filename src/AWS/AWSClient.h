/*
 * AWSClient.h
 *
 *  Created on: Jan 6, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_AWSCLIENT_H_
#define TestTest1_AWS_AWSCLIENT_H_

/// Abstract base class for AWS clients.
class AWSClient: public REFObject {
public:
	AWSClient(const String& serviceName, AWSCredentials* credentials,
			AWSRegion* region);
	virtual ~AWSClient();

	const String& getServiceName() const;

	/// Gets the endpoint for this client
	const String& getEndpoint() const;

	const AWSError getLastError() const {
		return _lastError;
	}

private:
	/// Sets the region for this client.
	void init(AWSRegion* region);

protected:
	String _serviceName;
	String _endpoint;

	REF<AWSCredentials> _credentials;
	REF<AWSSigner> _signer;

	AWSError _lastError;
};

#endif /* TestTest1_AWS_AWSCLIENT_H_ */
