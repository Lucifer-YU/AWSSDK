/*
 * AWSHttpClient.h
 *
 *  Created on: Jan 6, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_AWSHTTPCLIENT_H_
#define TestTest1_AWS_AWSHTTPCLIENT_H_

#include "HttpClient.h"
#include "AWSHttpRequest.h"
#include "AWSHttpResponse.h"

class AWSHttpClient: public REFObject {
public:
	AWSHttpClient();
	virtual ~AWSHttpClient();

	/// Sets the signer for this client.
	void setSigner(AWSSigner* signer) {
		_signer = signer;
	}
	/// Sets the credentials for this client.
	void setCredentials(AWSCredentials* credentials) {
		_credentials = credentials;
	}

	/// Executes the request and returns the result.
	AWSHttpResponse* execute(AWSHttpRequest* request);

	AWSError getLastError() const {
		return _lastError;
	}

private:
	// Executes a HTTP request, without retries even on failed.
	AWSHttpResponse* executeOnce(AWSHttpRequest* request);
	// Uses response status code to determine whether the request is successful.
	bool isRequestSuccessful(HttpResponse* httpResponse);

	// Creates a HTTP request object by given AWS HTTP request object.
	HttpRequest* createHttpRequst(AWSHttpRequest* request);
	// Creates a AWS HTTP response object from incoming HTTP response.
	AWSHttpResponse* createResponse(HttpResponse* httpResponse);

	// Puts User-Agent header field into specified AWS HTTP request object.
	void setUserAgent(AWSHttpRequest* request) {
		request->getHeaders()->set("User-Agent", "BFX-AWS-CPPClient/1.0.0");
	}

private:
	AWSSigner* _signer;
	AWSCredentials* _credentials;
	REF<HttpClient> _httpClient;

	AWSError _lastError;
};

#endif /* TestTest1_AWS_AWSHTTPCLIENT_H_ */
