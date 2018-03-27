/*
 * JSONWriter.h
 *
 *   Created on: Dec 14, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_JSON_JSONWRITER_H_
#define __TEST_JSON_JSONWRITER_H_

/// Error code definitions when writing JSON data.
enum JSONWriteError {
	JWE_NoError = 0,	/// Success with no errors
	JWE_MissingRoot,	/// Uncorrected JSON type of root node.
	JWE_IOError,		/// I/O error
};

class JSONNode;
/// Represents a writer that provides a fast, forward-only way to generate
/// streams that contain JSON data.
class JSONWriter: public REFObject {
	friend class JSONNode;
public:
	/// Creates a new instance with specified text writer
	JSONWriter(TextWriter* textWriter);
	virtual ~JSONWriter();

	/// Gets a value indicating whether this writer in compact mode.
	/// Compact mode, skip any extra whitespace characters in between each
	/// nodes, such as space, new line, etc...
	bool isCompactMode() const;
	/// Sets a value indicating whether this writer in compact mode.
	void setCompactMode(bool compact);

	/// Writes an object begin tag
	bool writeObjectBegin();
	/// Writes an object end tag
	bool writeEndObject();
	/// Writes an array begin tag
	bool writeStartArray();
	/// Writes an array end tag
	bool writeEndArray();

	/// Writes a string
	bool writeString(const String& value);
	/// Writes a numeric value, in raw string.
	bool writeNumber(const String& rawValue);
	/// Writes a boolean value
	bool writeBool(bool value);
	/// Writes a null
	bool writeNull();

	/// Writes a array separate tag
	bool writeArraySeparator();
	/// Writes a member separate tag
	bool writeMemberSeparator();

	/// Writes a space character
	bool writeSpace();
	/// Writes space character sequence of current indent
	bool writeIndent();
	/// Writes a new line (\r\n pair).
	bool writeNewLine();

	/// Gets the error code of the last operation
	JSONWriteError getLastError() const;
	/// Gets the error message of the last operation
	const char* getLastErrorMessage() const;

private:
	void pushIndent();
	void popIndent();

	static String escapeString(const String& str);

	static const char* getErrorMessageTemplate(JSONWriteError error);
	void setLastError(JSONWriteError error, ...);

private:
	JSONWriteError _lastError;
	String _lastErrorMessage;

	int _indent;
	bool _compactMode;

	REF<TextWriter> _textWriter;
};

#endif /* __TEST_JSON_JSONWRITER_H_ */
