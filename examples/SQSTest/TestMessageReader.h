/*
 * TestMessageReader.h
 *
 *  Created on: Dec 26, 2014
 *      Author: Lucifer
 */

#ifndef TestSDK1_TestMESSAGES_TestMESSAGEREADER_H_
#define TestSDK1_TestMESSAGES_TestMESSAGEREADER_H_

#include "TestMessage.h"

class TestMessageReader: public REFObject {
public:
	TestMessageReader(TextReader* textReader);
	virtual ~TestMessageReader();

	TestMessage* read();

	const String& getLastErrorMessage() const {
		return _lastErrorMessage;
	}
	MessageError getLastError() const {
		return _lastError;
	}

private:
	REF<TextReader> _textReader;
	MessageError _lastError;
	String _lastErrorMessage;
};

#endif /* TestSDK1_TestMESSAGES_TestMESSAGEREADER_H_ */
