/*
 * AWSRegion.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#include "AWS.h"

ArrayListT<REF<AWSRegion> > AWSRegion::_regions;

#define LOG_TAG "AWSRegion"

void AWSRegion::ensureInitialized() {
	if (_regions.getSize() == 0) {
		LOGI("Initializing all regions...");
		// Load regions
		REF<AWSRegion> region = new AWSRegion("cn-north-1", "amazonaws.com.cn");
		region->_endpoints.add(
				Endpoint("sqs", "sqs.cn-north-1.amazonaws.com.cn", true, true,
						AWSST_Version4));
		region->_endpoints.add(
				Endpoint("s3", "s3.cn-north-1.amazonaws.com.cn", true, true,
						AWSST_Version4));
		_regions.add(region);
	}
}

AWSRegion* AWSRegion::getRegion(const String& regionName) {
	ensureInitialized();
	// Looking for region by given region name
	for (int i = 0; i < _regions.getSize(); i++) {
		AWSRegion* region = _regions[i];
		if (region->_name == regionName)
			return region;
	}
	return NULL;
}

AWSRegion::AWSRegion(const String& name, const String& domain) {
	_name = name;
	_domain = domain;
}

AWSRegion::~AWSRegion() {
}

const String& AWSRegion::getName() const {
	return _name;
}
const String& AWSRegion::getDomain() const {
	return _domain;
}

const String& AWSRegion::getServiceEndpoint(const String& serviceName) const {
	const Endpoint* ep = getEndpoint(serviceName);
	BFX_ASSERT(ep);
	static String __empty;
	return (ep == NULL) ? __empty : ep->_hostname;
}

bool AWSRegion::isServiceSupported(const String& serviceName) const {
	const Endpoint* ep = getEndpoint(serviceName);
	return (ep == NULL) ? false : true;
}

bool AWSRegion::hasHttpsEndpoint(const String& serviceName) const {
	const Endpoint* ep = getEndpoint(serviceName);
	BFX_ASSERT(ep);
	return (ep == NULL) ? false : ep->_https;
}

bool AWSRegion::hasHttpEndpoint(const String& serviceName) const {
	const Endpoint* ep = getEndpoint(serviceName);
	BFX_ASSERT(ep);
	return (ep == NULL) ? false : ep->_http;
}

AWSSignerType AWSRegion::getSignerType(const String& serviceName) const {
	const Endpoint* ep = getEndpoint(serviceName);
	BFX_ASSERT(ep);
	return (ep == NULL) ? AWSST_NoOp : ep->_signerType;
}

const AWSRegion::Endpoint* AWSRegion::getEndpoint(
		const String& serviceName) const {
	LOGT("looking up endpoint for service name : '%s'",
			(const char* )serviceName);

	for (int i = 0; i < _endpoints.getSize(); i++) {
		const Endpoint& endpoint = _endpoints.getAt(i);
		if (endpoint._serviceName == serviceName) {
			return &endpoint;
		}
	}
	return NULL;
}
