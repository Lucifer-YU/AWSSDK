/*
 * AWSSigner.cpp
 *
 *  Created on: Jan 5, 2015
 *      Author: Lucifer
 */

#include "AWS.h"
#include "HttpUtils.h"
#include <openssl/hmac.h>

#undef LOGT
#define LOGT(...)

#define LOG_TAG "AWSSigner"

AWSSigner* AWSSignerFactory::getSigner(const String& serviceName,
		const String& regionName) {
	// lookup signer type
	AWSRegion* region = AWSRegion::getRegion(regionName);
	if (!region)
		return NULL;
	AWSSignerType signerType = region->getSignerType(serviceName);

	// create signer
	AWSSigner* signer = getSignerByType(signerType, serviceName);
	if (signer != NULL)
		signer->setRegionName(regionName);

	return signer;
}

AWSSigner* AWSSignerFactory::getSignerByType(AWSSignerType signerType,
		const String& serviceName) {
	REF<AWSSigner> signer;
	switch (signerType) {
	default:
		LOGW("Unrecognized signer type: %d", signerType);
		return NULL;
	case AWSST_NoOp:
		signer = new AWSNoOpSigner();
		break;
	case AWSST_QueryString:
		signer = new AWSQueryStringSigner();
		break;
	case AWSST_Version3:
		signer = new AWS3Signer();
		break;
	case AWSST_Version4:
		signer = new AWS4Signer();
		break;
	}
	signer->setServiceName(serviceName);
	signer->autorelease();
	return signer;
}

AWSSigner::AWSSigner() {
}

AWSSigner::~AWSSigner() {
}

String AWSSigner::signAndBase64Encode(const String& data, const uint8_t* key,
		int keySize, AWSSigningAlgorithm algorithm) {
	return signAndBase64Encode((const uint8_t*) data.cstr(), data.getLength(),
			key, keySize, algorithm);
}

String AWSSigner::signAndBase64Encode(const uint8_t* data, int dataSize,
		const uint8_t* key, int keySize, AWSSigningAlgorithm algorithm) {
	SharedBufferT<uint8_t> signature = sign(data, dataSize, key, keySize,
			algorithm);
	if (signature.getSize() == 0)
		return String();
	return HttpUtils::base64Encode(signature.getRawData(), signature.getSize());
}

SharedBufferT<uint8_t> AWSSigner::sign(const String& data, const uint8_t* key,
		int keySize, AWSSigningAlgorithm algorithm) {
	return sign((const uint8_t*) data.cstr(), data.getLength(),
			key, keySize, algorithm);
}

SharedBufferT<uint8_t> AWSSigner::sign(const uint8_t* data, int dataSize, const uint8_t* key,
		int keySize, AWSSigningAlgorithm algorithm) {
	// Reset last error message.
	_lastErrorMessage.setEmpty();

	// Select hash function.
	const EVP_MD* hf;
	if (algorithm == AWSSA_HmacSHA1) {
		hf = EVP_sha1();
	} if (algorithm == AWSSA_HmacSHA256) {
		hf = EVP_sha256();
	} else {
		_lastErrorMessage = "Unknown signing algorithm.";
		LOGE(_lastErrorMessage);
		return SharedBufferT<uint8_t>();	// returns empty buffer on failure.
	}

	// Computes the HMAC signature.
	SharedBufferT<uint8_t> result;
	uint8_t* buf = result.getBuffer(1024);
	unsigned int bufSize;
	HMAC(hf, key, keySize, data, dataSize, buf, &bufSize);
	result.releaseBuffer(bufSize);

	return result;
}

SharedBufferT<uint8_t> AWSSigner::hash(const String& data) {
	return hash((const uint8_t*) data.cstr(), data.getLength());
}

SharedBufferT<uint8_t> AWSSigner::hash(const uint8_t* data, int dataSize) {
	BFX_ASSERT(data || dataSize == 0);
	if (data == NULL)
		return SharedBufferT<uint8_t>();	// empty result

	uint8_t result[EVP_MAX_MD_SIZE];
	unsigned int resultSize = 0;

	EVP_MD_CTX evpCtx;
	EVP_DigestInit(&evpCtx, EVP_sha256());
	EVP_DigestUpdate(&evpCtx, data, dataSize);
	EVP_DigestFinal(&evpCtx, result, &resultSize);

	return SharedBufferT<uint8_t>(result, resultSize);
}

String AWSSigner::getCanonicalizedResourcePath(const String& resourcePath,
		bool urlEncode) {
	if (resourcePath.isEmpty()) {
		return "/";
	} else {
		String value = urlEncode ?
						HttpUtils::urlEncode(resourcePath, true) : resourcePath;
		if (value.startsWith("/")) {
			return value;
		} else {
			return "/" + value;
		}
	}
}

String AWSSigner::getCanonicalizedQueryString(AWSHttpRequest* request) {
	BFX_ASSERT(request);
    //
    // If we're using POST and we don't have any request payload content,
    // then any request query parameters will be sent as the payload, and
    // not in the actual query string.
    //

	if (request->getHttpMethod() == AHM_POST/* && request->getContent() == NULL*/)
		return ""; // use payload for query parameters
	return HttpUtils::encodeParameters(request->getParameters());
}

void AWSSigner::dbgHexPrint(const uint8_t* data, int dataSize) {
#ifdef _DEBUG
	String hexString = "[";
	for (int i = 0; i < dataSize; i++) {
		if (hexString.getLength() > 1)
			hexString.append(',');
		hexString.append(String::format("%d", (char) data[i]));
	}
	hexString.append(']');
	LOGT("derived Signing Key: \n\"%s\"", hexString.cstr());
#endif
}

bool AWSNoOpSigner::sign(AWSHttpRequest* request, AWSCredentials* credentials) {
	BFX_ASSERT(false);
	return false;
}

bool AWSQueryStringSigner::sign(AWSHttpRequest* request, AWSCredentials* credentials) {
	BFX_ASSERT(false);
	return false;
}

bool AWS3Signer::sign(AWSHttpRequest* request, AWSCredentials* credentials) {
	BFX_ASSERT(false);
	return false;
}

const String AWS4Signer::AWS4_TERMINATOR = "aws4_request";
const String AWS4Signer::AWS4_SIGNING_ALGORITHM = "AWS4-HMAC-SHA256";

class AWS4SignerRequestParams {
public:
	AWS4SignerRequestParams(AWSHttpRequest* request, const String& regionName,
			const String& serviceName) {
		BFX_ASSERT(request);
		BFX_ASSERT(!regionName.isEmpty());
		BFX_ASSERT(!serviceName.isEmpty());

		_request = request;
		_regionName = regionName;
		_serviceName = serviceName;

		DateTime time = DateTime().toUTC();
		_signingDateTimeMilli = time.getMillisecndsSince1970();
		LOGT("_signingDateTimeMilli=%lld", _signingDateTimeMilli);
		// yyyyMMdd
		_formattedSigningDate = time.format("%04d%02d%02d");
		_scope = generateScope(request, _formattedSigningDate, _serviceName,
				_regionName);
		// yyyyMMdd'T'HHmmss'Z'
		_formattedSigningDateTime = time.format("%04d%02d%02dT%02d%02d%02dZ");
	}
	String generateScope(AWSHttpRequest* request, const String& dateStamp,
			const String& serviceName, const String& regionName) {
		String scope = dateStamp;
		scope.append('/');
		scope.append(regionName);
		scope.append('/');
		scope.append(serviceName);
		scope.append('/');
		scope.append(AWS4Signer::AWS4_TERMINATOR);
		return scope;
	}

	AWSHttpRequest* _request;
	String _regionName;
	String _serviceName;
	int64_t _signingDateTimeMilli;
	String _formattedSigningDate;
	String _formattedSigningDateTime;
	String _scope;
};

AWS4Signer::AWS4Signer(bool doubleUrlEncode) :
		_doubleUrlEncode(doubleUrlEncode) {
}

AWS4Signer::~AWS4Signer() {
}

bool AWS4Signer::sign(AWSHttpRequest* request, AWSCredentials* credentials) {
	BFX_ASSERT(request);

	if (credentials == NULL)
		return true;		// anonymous credentials, don't sign

	AWS4SignerRequestParams signerParams(request, _regionName, _serviceName);

	// AWS4 requires that we sign the Host header so we have to have it in the
	// request by the time we sign.
	request->getHeaders()->set("Host",
			getHostFromUrlString(request->getEndpoint()));
	request->getHeaders()->set("X-Amz-Date",
			signerParams._formattedSigningDateTime);

	String contentSha256 = calculateContentHash(request);
	// request->getHeaders().set("x-amz-content-sha256", contentSha256);
	String canonicalRequest = createCanonicalRequest(request, contentSha256);
	String stringToSign = createStringToSign(canonicalRequest, signerParams);

	SharedBufferT<uint8_t> signingKey = deriveSigningKey(credentials,
			signerParams);
	SharedBufferT<uint8_t> signature = computeSignature(stringToSign,
			signingKey.getRawData(), signingKey.getSize(), signerParams);

	request->getHeaders()->set("Authorization",
			buildAuthorizationHeader(request, signature.getRawData(),
					signature.getSize(), credentials, signerParams));
	return true;
}

String AWS4Signer::getHostFromUrlString(const String& url) {
	String host = url;
	int pos = host.indexOf("://");
	if (pos != -1) {
		// remove leading protocol string.
		host = host.substring(pos + 3);
	}
	pos = host.indexOf('/');
	if (pos != -1) {
		// remove tailing path
		host = host.substring(0, pos);
	}
	return host;
}

String AWS4Signer::calculateContentHash(AWSHttpRequest* request) {
	if (request->getHttpMethod() == AHM_POST
			/* && request->getContent() == NULL*/) {
		// use payload for query parameters

		String payloadStr = HttpUtils::encodeParameters(
				request->getParameters());
		SharedBufferT<uint8_t> hashBytes = hash(payloadStr);
		String contentSha256 = HttpUtils::toHexString(hashBytes.getRawData(),
				hashBytes.getSize());

		LOGT("AWS4 Content Hash: \n\"%s\"", (const char* )contentSha256);
		return contentSha256;
	}
	// TODO: get binary request payload stream without query parameters
	BFX_ASSERT(false);
	return "";
}

String AWS4Signer::createCanonicalRequest(AWSHttpRequest* request,
            const String& contentSha256) {
	// Step 1 of the AWS Signature version 4 calculation.
	//
	// Refer to:
	// http://docs.aws.amazon.com/general/latest/gr/sigv4-create-canonical-request.html
	//

    // This would URL-encode the resource path for the first time.
	String path = HttpUtils::appendUri("/",	// request->getEndpoint()
			request->getResourcePath(), false);
	// TODO More HTTP methods support.
    String canonicalRequest = (request->getHttpMethod() == AHM_POST) ? "POST" : "GET";
    canonicalRequest.append("\n");
    // This would optionally double URL-encode the resource path
    canonicalRequest.append(getCanonicalizedResourcePath(path, _doubleUrlEncode));
    canonicalRequest.append("\n");
    canonicalRequest.append(getCanonicalizedQueryString(request));
    canonicalRequest.append("\n");
    canonicalRequest.append(getCanonicalizedHeaderString(request));
    canonicalRequest.append("\n");
    canonicalRequest.append(getSignedHeadersString(request));
    canonicalRequest.append("\n");
    canonicalRequest.append(contentSha256);

    LOGT("AWS4 Canonical Request: \n\"%s\"", (const char* )canonicalRequest);
    return canonicalRequest;
}

String AWS4Signer::createStringToSign(const String& canonicalRequest,
		AWS4SignerRequestParams& signerParams) {
	// Step 2 of the AWS Signature version 4 calculation
	//
	// Refer to:
	// http://docs.aws.amazon.com/general/latest/gr/sigv4-create-string-to-sign.html.
	String stringToSign = AWS4_SIGNING_ALGORITHM;
	stringToSign.append("\n");
	stringToSign.append(signerParams._formattedSigningDateTime);
	stringToSign.append("\n");
	stringToSign.append(signerParams._scope);
	stringToSign.append("\n");

	SharedBufferT<uint8_t> bytes = hash(canonicalRequest);
	stringToSign += HttpUtils::toHexString(bytes.getRawData(), bytes.getSize());

	LOGT("AWS4 String to Sign: \n\"%s\"", (const char* )stringToSign);
	return stringToSign;
}

SharedBufferT<uint8_t> AWS4Signer::deriveSigningKey(AWSCredentials* credentials,
            AWS4SignerRequestParams& signerRequestParams) {
	// Step 3 of the AWS Signature version 4 calculation. It involves deriving
	// the signing key and computing the signature.
	//
	// Refer to:
	// http://docs.aws.amazon.com/general/latest/gr/sigv4-calculate-signature.html
#if 0
	// Cache key
	const String cacheKey = computeSigningCacheKeyName(credentials,
			signerRequestParams);

	// Cache expiration
	const int64_t MillisPerSecond = 1000;
	const int64_t MillisPerDay = MillisPerSecond * 60 * 60 * 24;

	int64_t daysSinceEpochSigningDate =
			signerRequestParams._signingDateTimeMilli / MillisPerDay;
#endif

	// TODO Get cached signing key if possible.

	LOGT("Generating a new signing key as the signing key not available in the"
			" cache for the date : %lld",
			signerRequestParams._signingDateTimeMilli);

	SharedBufferT<uint8_t> signingKey = newSigningKey(credentials,
			signerRequestParams._formattedSigningDate,
			signerRequestParams._regionName, signerRequestParams._serviceName);

	// TODO Put the signing key back to cache.

	dbgHexPrint(signingKey.getRawData(), signingKey.getSize());

	return signingKey;
}

SharedBufferT<uint8_t> AWS4Signer::computeSignature(const String& stringToSign,
		const uint8_t* signingKey, int signingKeySize,
		AWS4SignerRequestParams& signerRequestParams) {
	// Step 3 of the AWS Signature version 4 calculation. It involves deriving
	// the signing key and computing the signature.
	//
	// Refer to:
	// http://docs.aws.amazon.com/general/latest/gr/sigv4-calculate-signature.html

	SharedBufferT<uint8_t> signature = AWSSigner::sign(stringToSign, signingKey, signingKeySize,
			AWSSA_HmacSHA256);

	dbgHexPrint(signature.getRawData(), signature.getSize());

	return signature;
}

String AWS4Signer::getCanonicalizedHeaderValue(const String& str) {
	String result;
	for (int i = 0; i < str.getLength(); i ++) {
		char c = str[i];
		result += isspace(c) ? ' ' : c;
	}
	return result;
}

// The case insensitive comparer.
class CaseInsensitiveComparer {
public:
	int operator()(const String& a, const String& b) const {
		return StringTraitsT<char>::stringCompareIgnore(a, b);
	}
};

String AWS4Signer::getCanonicalizedHeaderString(AWSHttpRequest* request) {
	BFX_ASSERT(request);

	typedef TreeMapT<String, String, CaseInsensitiveComparer> CanonicalizedHeaderMap;

	// Sort & canonicalize header names / values
	AWSStringMap* headers = request->getHeaders();
	CanonicalizedHeaderMap sortedHeaders;
	for (AWSStringMap::PENTRY entry = headers->getFirstEntry(); entry != NULL;
			entry = headers->getNextEntry(entry)) {
		String normalizedName = getCanonicalizedHeaderValue(
				entry->key.toLower());
		String normalizedValue = getCanonicalizedHeaderValue(entry->value);
		sortedHeaders.set(normalizedName, normalizedValue);
	}

	// Build header lines
	String result;
	for (CanonicalizedHeaderMap::PENTRY entry = sortedHeaders.getFirstEntry();
			entry != NULL; entry = sortedHeaders.getNextEntry(entry)) {
		result.append(entry->key);
		result.append(':');
		result.append(entry->value);
		result.append("\n");
	}

	return result;
}

String AWS4Signer::getSignedHeadersString(AWSHttpRequest* request) {
	// Sort header names
	ArrayListT<String> sortedHeaders;
	AWSStringMap* headers = request->getHeaders();
	for (AWSStringMap::PENTRY entry = headers->getFirstEntry(); entry != NULL;
			entry = headers->getNextEntry(entry)) {
		sortedHeaders.add(entry->key);
	}
	sortedHeaders.quickSort(0, sortedHeaders.getSize() - 1,
			CaseInsensitiveComparer());

	String result;
	for (int i = 0; i < sortedHeaders.getSize(); i++) {
		if (!result.isEmpty())
			result.append(';');
		result.append(sortedHeaders[i].toLower());
	}

	return result;
}

String AWS4Signer::computeSigningCacheKeyName(AWSCredentials* credentials,
		AWS4SignerRequestParams& signerRequestParams) {
	// Computes the name to be used to reference the signing key in the cache.

	String signingCacheKeyName = credentials->getSecretAccessKey();
	signingCacheKeyName.append('-');
	signingCacheKeyName.append(signerRequestParams._regionName);
	signingCacheKeyName.append('-');
	signingCacheKeyName.append(signerRequestParams._serviceName);

	return signingCacheKeyName;
}

// Generates a new signing key from the given parameters and returns it.
SharedBufferT<uint8_t> AWS4Signer::newSigningKey(AWSCredentials* credentials,
		const String& dateStamp, const String& regionName,
		const String& serviceName) {
	String kSecret = "AWS4" + credentials->getSecretAccessKey();
	SharedBufferT<uint8_t> kDate = AWSSigner::sign(dateStamp,
			(const uint8_t*) kSecret.cstr(), kSecret.getLength(),
			AWSSA_HmacSHA256);

	SharedBufferT<uint8_t> kRegion = AWSSigner::sign(regionName,
			kDate.getRawData(), kDate.getSize(), AWSSA_HmacSHA256);

	SharedBufferT<uint8_t> kService = AWSSigner::sign(serviceName,
			kRegion.getRawData(), kRegion.getSize(), AWSSA_HmacSHA256);

	return AWSSigner::sign(AWS4_TERMINATOR, kService.getRawData(),
			kService.getSize(), AWSSA_HmacSHA256);
}

// Creates the authorization header to be included in the request.
String AWS4Signer::buildAuthorizationHeader(AWSHttpRequest* request,
		const uint8_t* signature, int signatureSize,
		AWSCredentials* credentials, AWS4SignerRequestParams& signerParams) {
	const String signingCredentials = credentials->getAccessKeyId() + "/"
			+ signerParams._scope;

	const String credential = "Credential=" + signingCredentials;
	const String signerHeaders = "SignedHeaders="
			+ getSignedHeadersString(request);
	const String signatureHeader = "Signature="
			+ HttpUtils::toHexString(signature, signatureSize);

	String authHeader = AWS4_SIGNING_ALGORITHM;
	authHeader.append(' ');
	authHeader.append(credential);
	authHeader.append(',');
	authHeader.append(signerHeaders);
	authHeader.append(',');
	authHeader.append(signatureHeader);

	return authHeader;
}
