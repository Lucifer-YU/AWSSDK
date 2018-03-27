/*
 * AWSHttpResponse.h
 *
 *  Created on: Jan 5, 2015
 *      Author: Lucifer
 */

#ifndef AWS_AWSRESPONSE_H_
#define AWS_AWSRESPONSE_H_

class AWSHttpResponse: public REFObject {
public:
	AWSHttpResponse() :
			_statusCode(-1) {
	}
	virtual ~AWSHttpResponse() {
	}

	int getStatusCode() const {
		return _statusCode;
	}
	void setStatusCode(int statusCode) {
		_statusCode = statusCode;
	}

	bool hasHeaders() const {
		return ((_headers != NULL) && (_headers->getSize() > 0));
	}
	AWSStringMap* getHeaders() const {
		if (_headers == NULL)
			const_cast<AWSHttpResponse*>(this)->_headers = new AWSStringMap();
		return _headers;
	}

	void setContent(const String& content) {
		_content = content;
	}
	String getContent() const {
		return _content;
	}

private:
	int _statusCode;
	REF<AWSStringMap> _headers;
	String _content;
};

#endif /* AWS_AWSRESPONSE_H_ */
