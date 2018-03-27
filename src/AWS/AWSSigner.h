/*
 * AWSSigner.h
 *
 *  Created on: Jan 5, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_AWSSIGNER_H_
#define TestTest1_AWS_AWSSIGNER_H_

/// A credentials object provides access key ID and secret access key used for
/// accessing AWS services
class AWSCredentials: public REFObject {
public:
	AWSCredentials(const String& accessKeyId, const String& secretAccessKey) {
		_accessKeyId = accessKeyId;
		_secretAccessKey = secretAccessKey;
	}

	/// Returns the AWS access key ID
	const String& getAccessKeyId() const {
		return _accessKeyId;
	}

	/// Returns the AWS secret access key
	const String& getSecretAccessKey() const {
		return _secretAccessKey;
	}

private:
	String _accessKeyId;
	String _secretAccessKey;
};

enum AWSSignerType {
	AWSST_NoOp = 0,	///
	AWSST_QueryString = 1,	///
	AWSST_Version3 = 2,	///
	AWSST_Version4 = 3,	///
};

class AWSSigner;

class AWSSignerFactory {
private:
	AWSSignerFactory();	// no implemented
	virtual ~AWSSignerFactory();

public:
	static AWSSigner* getSigner(const String& serviceName,
			const String& regionName);
	static AWSSigner* getSignerByType(AWSSignerType signerType,
			const String& serviceName);

};

enum AWSSigningAlgorithm {
	AWSSA_HmacSHA1,	///
	AWSSA_HmacSHA256	///
};

class AWSHttpRequest;

///  A strategy for applying cryptographic signatures to a request
class AWSSigner : public REFObject {
public:
	AWSSigner();
	virtual ~AWSSigner();

	/// Sign the given request with the given set of credentials.
	virtual bool sign(AWSHttpRequest* request, AWSCredentials* credentials) = 0;

	/// Configure this signer with the name of the service it will be used to
	/// sign requests for.
	virtual void setServiceName(const String& value) = 0;

	/// Configure this signer with the name of the region it will be used to
	/// sign requests for.
	virtual void setRegionName(const String& value) = 0;

	bool isError() const {
		return (!_lastErrorMessage.isEmpty());
	}
	const char* getLastErrorMessage() const {
		return _lastErrorMessage;
	}

protected:
	/// Computes an RFC 2104-compliant HMAC signature for an array of bytes and
	/// returns the result as a Base64 encoded string.
	String signAndBase64Encode(const uint8_t* data, int dataSize,
			const uint8_t* key, int keySize, AWSSigningAlgorithm algorithm);
	String signAndBase64Encode(const String& data, const uint8_t* key,
			int keySize, AWSSigningAlgorithm algorithm);

	/// Computes an RFC 2104-compliant HMAC signature for an array of bytes.
	SharedBufferT<uint8_t> sign(const uint8_t* data, int dataSize,
			const uint8_t* key, int keySize, AWSSigningAlgorithm algorithm);
	SharedBufferT<uint8_t> sign(const String& data, const uint8_t* key,
			int keySize, AWSSigningAlgorithm algorithm);

	/// Hashes the string contents using the SHA-256
	SharedBufferT<uint8_t> hash(const uint8_t* data, int dataSize);
	SharedBufferT<uint8_t> hash(const String& data);

	String getCanonicalizedResourcePath(const String& resourcePath,
			bool urlEncode);
	String getCanonicalizedQueryString(AWSHttpRequest* request);

	void dbgHexPrint(const uint8_t* data, int dataSize);

private:
	String _lastErrorMessage;
};

class AWSNoOpSigner : public AWSSigner {
public:
	virtual bool sign(AWSHttpRequest* request, AWSCredentials* credentials);
	virtual void setServiceName(const String& serviceName) {}
	virtual void setRegionName(const String& regionName) {}
};

class AWSQueryStringSigner : public AWSSigner {
public:
	virtual bool sign(AWSHttpRequest* request, AWSCredentials* credentials);
	virtual void setServiceName(const String& serviceName) {}
	virtual void setRegionName(const String& regionName) {}
};

class AWS3Signer : public AWSSigner {
public:
	virtual bool sign(AWSHttpRequest* request, AWSCredentials* credentials);
	virtual void setServiceName(const String& serviceName) {}
	virtual void setRegionName(const String& regionName) {}
};

class AWS4SignerRequestParams;

/// Signer implementation that signs requests with the AWS4 signing protocol.
class AWS4Signer : public AWSSigner {
public:
	AWS4Signer(bool doubleUrlEncode = true);
	virtual ~AWS4Signer();

	/// Sign the given request with the given set of credentials.
	virtual bool sign(AWSHttpRequest* request, AWSCredentials* credentials);

	/// Configure this signer with the name of the service it will be used to
	/// sign requests for.
	virtual void setServiceName(const String& serviceName) {
		_serviceName = serviceName;
	}

	/// Configure this signer with the name of the region it will be used to
	/// sign requests for.
	virtual void setRegionName(const String& regionName) {
		_regionName = regionName;
	}

protected:
	// Calculate the hash of the request's payload
	String calculateContentHash(AWSHttpRequest* request);
	// Step 1 of the AWS Signature version 4 calculation.
	String createCanonicalRequest(AWSHttpRequest* request,
			const String& contentSha256);
	// Step 2 of the AWS Signature version 4 calculation
	String createStringToSign(const String& canonicalRequest,
			AWS4SignerRequestParams& signerParams);
	// Step 3 of the AWS Signature version 4 calculation. It involves deriving
	// the signing key and computing the signature.
	SharedBufferT<uint8_t> deriveSigningKey(AWSCredentials* credentials,
			AWS4SignerRequestParams& signerRequestParams);
	// Step 3 of the AWS Signature version 4 calculation. It involves deriving
	// the signing key and computing the signature.
	SharedBufferT<uint8_t> computeSignature(const String& stringToSign,
			const uint8_t* signingKey, int signingKeySize,
			AWS4SignerRequestParams& signerRequestParams);

	String getHostFromUrlString(const String& url);
	String getCanonicalizedHeaderString(AWSHttpRequest* request);
	String getSignedHeadersString(AWSHttpRequest* request);
	String getCanonicalizedHeaderValue(const String& str);

	// Computes the name to be used to reference the signing key in the cache.
    String computeSigningCacheKeyName(AWSCredentials* credentials,
			AWS4SignerRequestParams& signerRequestParams);
    // Generates a new signing key from the given parameters and returns it.
	SharedBufferT<uint8_t> newSigningKey(AWSCredentials* credentials,
			const String& dateStamp, const String& regionName,
			const String& serviceName);
	// Creates the authorization header to be included in the request.
	String buildAuthorizationHeader(AWSHttpRequest* request,
			const uint8_t* signature, int signatureSize,
			AWSCredentials* credentials, AWS4SignerRequestParams& signerParams);

public:
	static const String AWS4_TERMINATOR;
	static const String AWS4_SIGNING_ALGORITHM;
protected:

	// Whether double URL-encode the resource path when constructing the
	// canonical request. By default, we enable double url-encoding.
	bool _doubleUrlEncode;
	String _serviceName;
	String _regionName;
};

#endif /* TestTest1_AWS_AWSSIGNER_H_ */
