/*
 * AWSRegion.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Lucifer
 */

#ifndef TestTest1_AWS_AWSREGION_H_
#define TestTest1_AWS_AWSREGION_H_

class AWSRegion: public REFObject {
	AWSRegion(const String& name, const String& domain);
public:
	virtual ~AWSRegion();

	static AWSRegion* getRegion(const String& regionName);

	const String& getName() const;
	const String& getDomain() const;

	const String& getServiceEndpoint(const String& serviceName) const;
	bool isServiceSupported(const String& serviceName) const;
	bool hasHttpsEndpoint(const String& serviceName) const;
	bool hasHttpEndpoint(const String& serviceName) const;
	AWSSignerType getSignerType(const String& serviceName) const;

private:
	struct Endpoint {
		Endpoint() :
				_http(false), _https(false), _signerType(AWSST_NoOp) {
		}
		Endpoint(const String& serviceName, const String& hostname, bool http,
				bool https, AWSSignerType signerType) :
				_serviceName(serviceName), _hostname(hostname), _http(http), _https(
						https), _signerType(signerType) {
		}
		String _serviceName;
		String _hostname;
		bool _http;
		bool _https;
		AWSSignerType _signerType;
	};

private:
	const Endpoint* getEndpoint(const String& serviceName) const;

	static void ensureInitialized();
	static ArrayListT<REF<AWSRegion> > _regions;

	String _name;
	String _domain;
	ArrayListT<Endpoint> _endpoints;
};

#endif /* TestTest1_AWS_AWSREGION_H_ */
