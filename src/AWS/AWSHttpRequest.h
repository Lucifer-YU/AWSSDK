/*
 * AWSHttpRequest.h
 *
 *  Created on: Jan 5, 2015
 *      Author: Lucifer
 */

#ifndef AWS_AWSREQUEST_H_
#define AWS_AWSREQUEST_H_

enum AWSHttpMethod {
	AHM_GET,	///
	AHM_POST,	///
};

/// Represents a request being sent to an Amazon Web Service. including the
/// parameters being sent as part of the request, the endpoint to which the
/// request should be sent, etc.
class AWSHttpRequest: public REFObject {
public:
	/// Creates a new instance with the specified service name.
	AWSHttpRequest(const String& serviceName) {
		_serviceName = serviceName;
		_httpMethod = AHM_POST;
	}
	virtual ~AWSHttpRequest() {
	}

	/// Gets the HTTP method (GET, POST, etc) to use when sending this request.
	AWSHttpMethod getHttpMethod() const {
		return _httpMethod;
	}
	/// Sets the HTTP method (GET, POST, etc) to use when sending this request.
	void setHttpMethod(AWSHttpMethod httpMethod) {
		_httpMethod = httpMethod;
	}

	/// Gets the service endpoint (ex: "https://ec2.amazonaws.com") to which
	/// this request should be sent.
	const String& getEndpoint() const {
		return _endpoint;
	}
	/// Sets the service endpoint (ex: "https://ec2.amazonaws.com") to which
	/// this request should be sent.
	void setEndpoint(const String& endpoint) {
		_endpoint = endpoint;
	}

	/// Sets the path to the resource being requested.
	void setResourcePath(const String& path) {
		_resourcePath = path;
	}
	/// Gets the path to the resource being requested.
	const String& getResourcePath() const {
		return _resourcePath;
	}

	/// Gets a collection that contains header fields.
	AWSStringMap* getHeaders() const {
		if (_headers == NULL)
			const_cast<AWSHttpRequest*>(this)->_headers = new AWSStringMap();
		return _headers;
	}
	/// Gets a value indicating header fields is not empty.
	bool hasHeaders() const {
		return ((_headers != NULL) && (_headers->getSize() > 0));
	}

	/// Gets a collection that contains parameters.
	AWSStringMap* getParameters() const {
		if (_parameters == NULL)
			const_cast<AWSHttpRequest*>(this)->_parameters = new AWSStringMap();
		return _parameters;
	}
	/// Gets a value that indicating parameter fields is not empty.
	bool hasParameters() const {
		return ((_parameters != NULL) && (_parameters->getSize() > 0));
	}

private:
	String _serviceName;

	AWSHttpMethod _httpMethod;
	String _endpoint;
	String _resourcePath;
	REF<AWSStringMap> _parameters;
	REF<AWSStringMap> _headers;
};

#endif /* AWS_AWSREQUEST_H_ */
