/*
 * TestMessageContext.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: Lucifer
 */

#include "TestMessageContext.h"

bool TestMessageContext::isExpired() const {
	// Get expiration by context type
	time_t expirMSec;
	switch (this->getType()) {
	case CT_Handshake:
		expirMSec = (30 * 1000);		// 30 seconds
		break;
	default:
		expirMSec = (5 * 60 * 1000);	// 5 minutes
	}
	int64_t currentMSec = DateTime::currentMillisecondsSince1970();
	return (currentMSec > (_startTimeMSec + expirMSec));
}
