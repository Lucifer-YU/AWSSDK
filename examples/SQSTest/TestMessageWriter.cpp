/*
 * TestMessageWriter.cpp
 *
 *  Created on: Dec 26, 2014
 *      Author: Lucifer
 */

#include "TestMessageWriter.h"

#define LOG_TAG "MessageWriter"

TestMessageWriter::TestMessageWriter(TextWriter* textWriter) {
	BFX_ASSERT(textWriter);
	_textWriter = textWriter;
	_lastError = MSGERR_Success;
}

TestMessageWriter::~TestMessageWriter() {
	_textWriter->close();
}

bool TestMessageWriter::write(TestMessage* message) {
	LOGI("MessageWriter::write(message:%p)", message);

	BFX_ASSERT(message);
	// reset error code
	_lastError = MSGERR_Success;
	_lastErrorMessage.setEmpty();

	JSONObject* root = message->serialize();
	if (root == NULL) {
		_lastError = MSGERR_SerializationFailed;
		_lastErrorMessage = String::format("%s, failed to serialize message",
				(const char*) message->getLastErrorMessage());
		LOGE(_lastErrorMessage);
		return false;
	}
	REF<JSONWriter> jw = new JSONWriter(_textWriter);
	jw->setCompactMode(true);
	if (!JSONNode::toJSONWriter(jw, root)) {
		_lastError = MSGERR_WriteError;
		_lastErrorMessage = String::format(
				"%s, unable to write JSON object to string.",
				(const char*) jw->getLastErrorMessage());
		LOGE(_lastErrorMessage);
		return false;
	}

	bool success = _textWriter->flush();
	BFX_ASSERT(success);

	return true;
}
