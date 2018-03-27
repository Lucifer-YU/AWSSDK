/*
 * AWSHttpClient.cpp
 *
 *  Created on: Jan 6, 2015
 *      Author: Lucifer
 */

#include "AWS.h"
#include "AWSHttpClient.h"
#include "HttpClient.h"
#include "HttpUtils.h"

#define LOG_TAG "AWSHttpClient"

AWSHttpClient::AWSHttpClient() :
		_signer(NULL), _credentials(NULL), _lastError(AWSE_NoError) {

	_httpClient = new HttpClient();
}

AWSHttpClient::~AWSHttpClient() {
}

AWSHttpResponse* AWSHttpClient::execute(AWSHttpRequest* request) {
	// Apply whatever request options we know how to handle, such as user-agent.
	setUserAgent(request);

	// TODO: Retries if necessary.
	return executeOnce(request);
}

AWSHttpResponse* AWSHttpClient::executeOnce(AWSHttpRequest* request) {
	BFX_ASSERT(request);

	// Sign the request if both signer and credentials were provided
	if (_signer && _credentials) {
		if (!_signer->sign(request, _credentials)) {
			_lastError = AWSE_SignFailed;
			LOGE("Failed to sign the given request");
			return NULL;
		}
	}

	// Create a new HttpRequest object
	REF<HttpRequest> httpRequest = createHttpRequst(request);
	if (httpRequest == NULL) {
		// We don't need to set error code here.
		LOGE("Unable to create HTTP request.");
		return NULL;
	}
	HttpResponse* httpResponse = _httpClient->execute(httpRequest);
	if (httpResponse == NULL) {
		_lastError = AWSE_HttpRequestFailed;
		LOGE("(%d) %s, Failed to communicate with server.", _httpClient->getLastError(),
			_httpClient->getLastErrorMessage().cstr());
		return NULL;
	}
	if (!isRequestSuccessful(httpResponse)) {
		// NOTE We should set last error???
		LOGW("Request unsuccessful, response status code: %d.",
				httpResponse->getStatusCode());
		// NOTE don't return NULL, it's fine.
	}

	return createResponse(httpResponse);
}
AWSHttpResponse* AWSHttpClient::createResponse(HttpResponse* httpResponse) {
	BFX_ASSERT(httpResponse);

	REF<AWSHttpResponse> response = new AWSHttpResponse();
	response->setStatusCode(httpResponse->getStatusCode());

	// Copy headers
	const TreeMapT<String, String>& httpHeaders = httpResponse->getHeaderFields();
	for (TreeMapT<String, String>::PENTRY entry = httpHeaders.getFirstEntry();
			entry != NULL; entry = httpHeaders.getNextEntry(entry)) {
		response->getHeaders()->set(entry->key, entry->value);
	}

	// Copy contents
	const BufferT<uint8_t>& httpBody = httpResponse->getBody();
	String content((const char*) httpBody.getRawData(), httpBody.getSize());
	response->setContent(content);

	response->autorelease();
	return response;
}

HttpRequest* AWSHttpClient::createHttpRequst(AWSHttpRequest* request) {
	REF<HttpRequest> httpRequest;
	String url = HttpUtils::appendUri(request->getEndpoint(),
			request->getResourcePath(), true);
	String encodedParams = HttpUtils::encodeParameters(
			request->getParameters());

	LOGI("PARAM: %s", encodedParams.cstr());

	if (request->getHttpMethod() == AHM_POST) {
		REF<HttpPost> httpPost = new HttpPost();
		// Sets parameters to post body.
		httpPost->getBody().append((const uint8_t*) encodedParams.cstr(),
				encodedParams.getLength());
		httpRequest = (HttpPost*) httpPost;
	} else if (request->getHttpMethod() == AHM_POST) {
		httpRequest = new HttpGet();
		// Sets parameters to query string.
		url.append('?');
		url.append(encodedParams);
	} else {
		_lastError = AWSE_UnrecognizedHttpProtocol;
		LOGE("Unrecognized protocol specified...");
		BFX_ASSERT(false);
		return NULL;
	}

	httpRequest->setUrl(url);
	// Copy over all headers already in our request
	AWSStringMap* headers = request->getHeaders();
	for (AWSStringMap::PENTRY header = headers->getFirstEntry();
			header != NULL; header = headers->getNextEntry(header)) {
		LOGI("HEADER: %s=%s", (const char* )header->key,
				(const char* )header->value);
		httpRequest->getHeaderFields().set(header->key, header->value);
	}

	httpRequest->autorelease();
	return httpRequest;
}

bool AWSHttpClient::isRequestSuccessful(HttpResponse* httpResponse) {
	BFX_ASSERT(httpResponse);
	// If we get back any 2xx status code, then we know we should treat the
	// service call as successful.
	const int SC_OK = 200;
	int status = httpResponse->getStatusCode();
	return status / 100 == SC_OK / 100;
}
