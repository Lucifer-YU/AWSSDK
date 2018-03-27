/*
 * TestMessageContext.h
 *
 *  Created on: Dec 27, 2014
 *      Author: Lucifer
 */

#ifndef MESSAGES_MESSAGEREQUESTCONTEXT_H_
#define MESSAGES_MESSAGEREQUESTCONTEXT_H_

#include "Foundation/Foundation.h"

class TestMessageContext: public REFObject {
public:
	enum ContextType {
		CT_Handshake,
		CT_Pay,
		CT_Advertising,
	};

public:
	TestMessageContext(ContextType type, uint64_t serial,
			String attachment, void* callback) {
		_type = type;
		_serial = serial;
		_attachment = attachment;
		_startTimeMSec = DateTime::currentMillisecondsSince1970();
		_callback = callback;
	}
	virtual ~TestMessageContext() {
	}

	//
	// Properties
	//
	ContextType getType() const {
		return _type;
	}
	uint64_t getSerial() const {
		return _serial;
	}
	String getAttachment() const {
		return _attachment;
	}
	int64_t getStartTime() const {
		return _startTimeMSec;
	}
	void* getCallback() const {
		return _callback;
	}

	bool isExpired() const;

private:
	ContextType _type;

	uint64_t _serial;
	String _attachment;
	void* _callback;
	int64_t _startTimeMSec;
};

#endif /* MESSAGES_MESSAGEREQUESTCONTEXT_H_ */
