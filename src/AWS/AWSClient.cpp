/*
 * AWSClient.cpp
 *
 *  Created on: Jan 6, 2015
 *      Author: Lucifer
 */

#include "AWS.h"

#define LOG_TAG "AWSClient"

AWSClient::AWSClient(const String& serviceName, AWSCredentials* credentials,
		AWSRegion* region) {
	BFX_ASSERT(!serviceName.isEmpty());
	BFX_ASSERT(credentials);
	BFX_ASSERT(region);

	_serviceName = serviceName;
	_credentials = credentials;
	init(region);
}

AWSClient::~AWSClient() {
}

void AWSClient::init(AWSRegion* region) {
	BFX_ASSERT(region != NULL);

	String serviceName = _serviceName;
	String serviceEndpoint;
	String protocol;
	if (region->isServiceSupported(serviceName)) {
		serviceEndpoint = region->getServiceEndpoint(serviceName);
		BFX_ASSERT(region->hasHttpEndpoint(serviceName) || region->hasHttpsEndpoint(serviceName));
		protocol = region->hasHttpsEndpoint(serviceName) ? "https" : "http";
	} else {
		serviceEndpoint = String::format("%s.%s.%s", (const char*) serviceName,
				(const char*) region->getName(),
				(const char*) region->getDomain());
		LOGW("{%s, %s} was undefined, trying to construct an endpoint using"
				" the standard pattern for this region: '%s'.",
				(const char* )serviceName, (const char* )region->getName(),
				(const char* )serviceEndpoint);
		protocol = "https";
	}

	// Initializes signer & endpoint URI.
	_signer = AWSSignerFactory::getSigner(serviceName, region->getName());
	if (_signer == NULL) {
		// Unrecognized signer type
		_lastError = AWSE_UnrecognizedSignerType;
		LOGE("Unrecognized signer type for region:%s, service:%s.",
				serviceName.cstr(), region->getName().cstr());
		return;
	}

	_endpoint = protocol + "://" + serviceEndpoint;
	LOGT("endpoint : '%s'", (const char* )_endpoint);
}

const String& AWSClient::getEndpoint() const {
	return _endpoint;
}

