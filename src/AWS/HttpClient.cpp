/*
 * HttpClient.cpp
 *
 *  Created on: Jan 3, 2015
 *      Author: Lucifer
 */

#include "HttpClient.h"

#undef LOG_TAG
#define LOG_TAG "HttpClient"

HttpClient::HttpClient() {
	LOGT("Initializes HTTP client.");

	_lastError = HTTPCE_Success;

	// Initializes CURL environment.
	_curlCtx = curl_easy_init();
	if (_curlCtx == NULL) {
		_lastError = HTTPCE_FailedInitialize;
		_lastErrorMessage = "Initialize CURL context failed.";
		LOGE(_lastErrorMessage);
	}
	curl_easy_setopt(_curlCtx, CURLOPT_ERRORBUFFER, _curlErrorBuffer);
	curl_easy_setopt(_curlCtx, CURLOPT_HEADER, 1L);	// Retrieves response headers.
	curl_easy_setopt(_curlCtx, CURLOPT_TIMEOUT, 30L);
	curl_easy_setopt(_curlCtx, CURLOPT_COOKIESESSION, 1L);
#ifdef WIN32
	curl_easy_setopt(_curlCtx, CURLOPT_COOKIEFILE, "nul");
	curl_easy_setopt(_curlCtx, CURLOPT_SSL_VERIFYPEER, 0L);
#else
	curl_easy_setopt(_curlCtx, CURLOPT_COOKIEFILE, "/dev/null");
#endif
#ifdef _DEBUG
	curl_easy_setopt(_curlCtx, CURLOPT_VERBOSE, 1);
#endif
}

HttpClient::~HttpClient() {
	LOGT("Cleanup HTTP client.");

	// Cleanup CURL environment.
	if (_curlCtx) {
		curl_easy_cleanup(_curlCtx);
	}
}

HttpResponse* HttpClient::execute(HttpRequest* request) {
	LOGT("Executing HTTP request...");

	// Reset last error
	_lastError = HTTPCE_Success;
	_lastErrorMessage.setEmpty();

	HttpRequestContext ctx(this, request);
	return ctx.execute();

}

// Mapping CURLcode to one of HttpClientError values.
HttpClientError HttpClient::getErrorFromCURLcode(CURLcode code) {
	switch (code) {
	case CURLE_OK:
		return HTTPCE_Success;
	case CURLE_COULDNT_CONNECT:
	case CURLE_COULDNT_RESOLVE_HOST:
	case CURLE_NO_CONNECTION_AVAILABLE:
		return HTTPCE_CouldntConnect;
	case CURLE_REMOTE_ACCESS_DENIED:
		return HTTPCE_RemoteAccessDenied;
	case CURLE_WRITE_ERROR:
	case CURLE_READ_ERROR:
	case CURLE_SEND_ERROR:
	case CURLE_RECV_ERROR:
	case CURLE_CHUNK_FAILED:
		return HTTPCE_IOError;
	case CURLE_ABORTED_BY_CALLBACK:
		return HTTPCE_Aborted;
	case CURLE_PEER_FAILED_VERIFICATION:
		return HTTPCE_PeerFailedVerification;
	case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
		return HTTPCE_SSLPinnedPubkeyNotMatch;
	case CURLE_SSL_CONNECT_ERROR:
		return HTTPCE_SSLConnectError;
	default:
		return HTTPCE_UnspecifiedError;
	}
}

HttpClient::HttpRequestContext::HttpRequestContext(HttpClient* client,
		HttpRequest* request) {

	LOGT("Begin HTTP request context : %p.", this);

	_headerFieldsEnded = false;
	_client = client;
	_request = request;

	//
	// Update URL
	//
	_url = request->getUrl();
	LOGI("URL: %s", (const char* )_url);
	curl_easy_setopt(_client->_curlCtx, CURLOPT_URL, (const char* )_url);

	//
	// Update header fields
	//
	_curlHeaders = NULL;
	TreeMapT<String, String>& headers = request->getHeaderFields();
	for (TreeMapT<String, String>::PENTRY header = headers.getFirstEntry();
			header != NULL; header = headers.getNextEntry(header)) {
		// append single header field.
		String headerLine = header->key + ": " + header->value;
		_curlHeaders = curl_slist_append(_curlHeaders,
				(const char*) headerLine);
	}
	curl_easy_setopt(_client->_curlCtx, CURLOPT_HTTPHEADER, _curlHeaders);

	//
	// Special settings between get/post methods.
	//
	if (request->getMethod() == HTTPM_Get) {
		// Specify we want to GET data
		curl_easy_setopt(_client->_curlCtx, CURLOPT_HTTPGET, 1L);
	} else {
		BFX_ASSERT(request->getMethod() == HTTPM_Post);

		// Specify we want to POST data
		curl_easy_setopt(_client->_curlCtx, CURLOPT_HTTPGET, 0L);
		curl_easy_setopt(_client->_curlCtx, CURLOPT_POST, 1L);

		//
		// Update post body
		//
		HttpPost* post = (HttpPost*) request;
		BufferT<uint8_t>& data = post->getBody();
		curl_easy_setopt(_client->_curlCtx, CURLOPT_POSTFIELDS, data.getRawData());
		curl_easy_setopt(_client->_curlCtx, CURLOPT_POSTFIELDSIZE,
				(long ) data.getSize());
	}
}

HttpClient::HttpRequestContext::~HttpRequestContext() {
	LOGT("End HTTP request context : %p.", this);
	if (_curlHeaders) {
		curl_slist_free_all(_curlHeaders);
	}
}

HttpResponse* HttpClient::HttpRequestContext::execute() {
	_response = new HttpResponse();

	// Sets callback function & object.
	curl_easy_setopt(_client->_curlCtx, CURLOPT_WRITEFUNCTION,
			HttpClient::HttpRequestContext::receiveCallback);
	curl_easy_setopt(_client->_curlCtx, CURLOPT_WRITEDATA, this);

	// Perform the request
	CURLcode curlResult = curl_easy_perform(_client->_curlCtx);
	if (curlResult != CURLE_OK) {
		// Mapping curl result to our error code
		_client->_lastError = getErrorFromCURLcode(curlResult);
		_client->_lastErrorMessage = curl_easy_strerror(curlResult);
		LOGE(_client->_lastErrorMessage);
		return NULL;
	}
	long httpCode = 0;
	curl_easy_getinfo(_client->_curlCtx, CURLINFO_RESPONSE_CODE, &httpCode);
	_response->_statusCode = (int) httpCode;

	_response->autorelease();
	return _response;
}

size_t HttpClient::HttpRequestContext::receiveCallback(void *data, size_t size,
		size_t nmemb, void *args) {
	HttpRequestContext* thisContext = static_cast<HttpRequestContext*>(args);
	BFX_ASSERT(thisContext);

	const uint8_t* bytes = static_cast<uint8_t*>(data);
	int length = (int) (size * nmemb);

	return thisContext->onReceive(bytes, length);
}

size_t HttpClient::HttpRequestContext::parseHeaderLines(const uint8_t* chunk,
		size_t chunkSize) {
	// parse header lines
	size_t startPos = 0;
	size_t curPos = -1;
	while (++curPos < chunkSize) {
		switch (chunk[curPos]) {
		case 0x0A:
			goto OnNewLine;
		case 0x0D:
			if (curPos + 1 > chunkSize)
				continue;	// end of chunk
			if (chunk[curPos + 1] == 0x0A) {
				curPos++;
			} else {
				// TODO: EOL normalization of 0xD
				BFX_ASSERT(false);	// bug!!! if '\n', '\r' separates in 2 chunks
			}
			goto OnNewLine;
		}
		continue;

OnNewLine:
		_response->_body.append(chunk + startPos, curPos - startPos);
		String headerLine((const char*) _response->_body.getRawData(),
				_response->_body.getSize());
		_response->_body.clear();
		headerLine = headerLine.trim();
		startPos = curPos;
		if (headerLine.isEmpty()) {
			// empty line meaning header fields ended;
			_headerFieldsEnded = true;
			return (curPos + 1);
		}
		parseHeaderLine(headerLine);
	}

	// The line has not ended, just append new chunk to body.
	curPos--;
	_response->_body.append(chunk + startPos, curPos - startPos);

	return chunkSize;
}

void HttpClient::HttpRequestContext::parseHeaderLine(const String& headerLine) {
	int pos = headerLine.indexOf(':');
	if (pos <= 0) {
		LOGT("STATUS_LINE=%s", (const char* ) headerLine);
		return;	// Status line, ignored.
	}

	LOGT("HEADER_FIELD= %s", (const char* ) headerLine);
	String key = headerLine.substring(0, pos++);
	String value = headerLine.substring(pos, headerLine.getLength() - pos);

	_response->_headerFields.set(key, value.trim());
}

size_t HttpClient::HttpRequestContext::onReceive(const uint8_t* chunk,
		size_t chunkSize) {
	int chunkOffset = 0;

	if (!_headerFieldsEnded) {
		// parse header lines
		chunkOffset = parseHeaderLines(chunk, chunkSize);
	}
	_response->_body.append(chunk + chunkOffset, chunkSize - chunkOffset);

	return chunkSize;
}
