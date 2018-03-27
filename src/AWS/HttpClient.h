/*
 * HttpClient.h
 *
 *  Created on: Jan 3, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_HTTPCLIENT_H_
#define TestTest1_AWS_HTTPCLIENT_H_

#include "../Foundation/Foundation.h"
#include <curl/curl.h>

/// Defines error codes for the HttpClient class.
enum HttpClientError {
	HTTPCE_Success = 0,	/// No error
	HTTPCE_UnspecifiedError = 1,	/// Unspecified error
	HTTPCE_FailedInitialize = 2,	/// Failed to initialize
	HTTPCE_CouldntConnect = 10,		/// Could't connect
	HTTPCE_RemoteAccessDenied = 11,	/// Access denied
	HTTPCE_IOError = 20,			/// Send / receive failed
	HTTPCE_Aborted = 21,			/// Aborted
	HTTPCE_PeerFailedVerification = 30,	/// Peer's certificate or fingerprint wasn't verified fine
	HTTPCE_SSLPinnedPubkeyNotMatch = 31,/// Specified pinned public key did not match
	HTTPCE_SSLConnectError = 32,	/// Wrong when connecting with SSL
};

enum HttpMethod {
	HTTPM_Get = 0, ///
	HTTPM_Post = 1, ///
};

class HttpClient;

/// The base HTTP request message from a client to a server includes.
class HttpRequest: public REFObject {
protected:
	friend class HttpClient;
	/// Initializes a new instance
	HttpRequest() {
	}

public:
	/// Sets the URL this request uses.
	void setUrl(const String& url) {
		_url = url;
	}

	/// Gets the URL this request uses.
	const String& getUrl() const {
		return _url;
	}

	/// Gets the table contains header fields.
	TreeMapT<String, String>& getHeaderFields() {
		return _headerFields;
	}

	/// Gets the HTTP method this request uses, such as M_Post, M_Get.
	virtual HttpMethod getMethod() const = 0;

protected:
	String _url;
	TreeMapT<String, String> _headerFields;
};

/// The HTTP get request message
class HttpGet: public HttpRequest {
public:
	/// Initializes a new instance.
	HttpGet() {
	}
	virtual ~HttpGet() {
	}

	/// Gets the HTTP method
	virtual HttpMethod getMethod() const {
		return HTTPM_Get;
	}
};

/// The HTTP post request message
class HttpPost: public HttpRequest {
public:
	/// Initializes a new instance.
	HttpPost() {
	}
	virtual ~HttpPost() {
	}

	/// Gets the HTTP method
	virtual HttpMethod getMethod() const {
		return HTTPM_Post;
	}
	/// Gets the post body.
	BufferT<uint8_t>& getBody() {
		return _body;
	}

protected:
	BufferT<uint8_t> _body;
};

/// The HTTP response message from a client to a server includes.
class HttpResponse: public REFObject {
protected:
	friend class HttpClient;
	/// Initializes a new instance
	HttpResponse() {
		_statusCode = 0;
		_headerFieldsEnded = false;
	}

public:
	virtual ~HttpResponse() {
	}

	/// Gets the collection that contains response header fields.
	const TreeMapT<String, String>& getHeaderFields() const {
		return _headerFields;
	}

	/// Gets the response body
	const BufferT<uint8_t>& getBody() const {
		return _body;
	}

	/// Gets the response HTTP status code
	/// All status codes defined in RFC1945 (HTTP/1.0), RFC2616 (HTTP/1.1),
	/// and RFC2518 (WebDAV) are listed.
	int getStatusCode() const {
		return _statusCode;
	}

protected:
	TreeMapT<String, String> _headerFields;
	bool _headerFieldsEnded;
	BufferT<uint8_t> _body;
	int _statusCode;
};

///
class HttpClient: public REFObject {
public:
	/// Initializes a new instance of HTTP client.
	HttpClient();
	virtual ~HttpClient();

	/// Executes a request synchronously
	HttpResponse* execute(HttpRequest* equest);

	/// Gets the error code for the last operation.
	HttpClientError getLastError() const {
		return _lastError;
	}
	/// Gets the error message for the last operation.
	const String& getLastErrorMessage() const {
		return _lastErrorMessage;
	}

protected:
	// Mapping CURLcode to one of HttpClientError values.
	static HttpClientError getErrorFromCURLcode(CURLcode code);

	// Per-request context during execution.
	class HttpRequestContext {
	public:
		HttpRequestContext(HttpClient* client, HttpRequest* request);
		virtual ~HttpRequestContext();
		HttpResponse* execute();

	protected:
		// CURL receiving callback
		static size_t receiveCallback(void *data, size_t size, size_t nmemb,
				void *args);
		size_t onReceive(const uint8_t* chunk, size_t chunkSize);
		// Parse HTTP header helpers
		size_t parseHeaderLines(const uint8_t* chunk, size_t chunkSize);
		void parseHeaderLine(const String& headerLine);

	private:
		String _url;
		curl_slist* _curlHeaders;

		// Temporary variables during execution.
		HttpClient* _client;
		HttpRequest* _request;
		REF<HttpResponse> _response;
		bool _headerFieldsEnded;
	};

private:
	HttpClientError _lastError;
	String _lastErrorMessage;

	CURL* _curlCtx;
	char _curlErrorBuffer[CURL_ERROR_SIZE];
};

#endif /* TestTest1_AWS_HTTPCLIENT_H_ */
