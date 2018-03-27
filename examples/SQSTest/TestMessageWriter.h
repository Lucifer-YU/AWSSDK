/*
 * MessageWriter.h
 *
 *  Created on: Dec 26, 2014
 *      Author: Lucifer
 */

#ifndef TestSDK1_TestMESSAGES_TestMESSAGEWRITER_H_
#define TestSDK1_TestMESSAGES_TestMESSAGEWRITER_H_

#include "TestMessage.h"

class TestMessageWriter: public REFObject {
public:
	TestMessageWriter(TextWriter* textWriter);
	virtual ~TestMessageWriter();

	bool write(TestMessage* message);

	String getLastErrorMessage() const {
		return _lastErrorMessage;
	}
	MessageError getLastError() const {
		return _lastError;
	}

private:
	REF<TextWriter> _textWriter;
	MessageError _lastError;
	String _lastErrorMessage;
};

#endif /* TestSDK1_TestMESSAGES_TestMESSAGEWRITER_H_ */
