/*
 * JSONReader.h
 *
 *   Created on: Dec 13, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_JSON_JSONREADER_H_
#define __TEST_JSON_JSONREADER_H_

/// Error code definitions when reading JSON data.
enum JSONReadError {
	JRE_NoError = 0,	/// Success with no error occurs
	JRE_UnexpectedEOF,	/// Unexpected end of file
	JRE_UnexpectedToken,	/// Unexpected token reaches
	JRE_UnexpectedToken2,/// Unexpected token reaches, same as 'JRE_UnexpectedToken'
	JRE_UnexpectedCharacter2,	/// Unexpected character reaches
	JRE_MissingRoot,		/// Root node missing
};

class JSONNode;

/// Represents a reader that provides fast, forward-only access to JSON data.
class JSONReader : public REFObject {
	friend class JSONNode;
public:
	/// Token type definitions on parsing JSON data
	enum JSONTokenType {
		JT_EOF = 0,	/// End of file
		JT_ObjectBegin,	/// Object begin
		JT_ObjectEnd,	/// Object end
		JT_ArrayBegin,	/// Array begin
		JT_ArrayEnd,	/// Array end
		JT_String,		/// String value
		JT_Number,		/// Numeric value
		JT_True,		/// Boolean true value
		JT_False,		/// Boolean false value
		JT_Null,		/// Null value
		JT_ArraySeparator,	/// Array separator
		JT_MemberSeparator,	/// Member separator
		JT_Comment,	/// Comment, not support yet.
		JT_Error	/// Error occurs
	};

public:
	/// Initializes a new instance by specified string
	JSONReader(const char* psz, bool retain = false);
	/// Initializes a new instance by specified text reader
	JSONReader(TextReader* textReader);
	virtual ~JSONReader();

	/// Forward to next token
	/// @returns The token type
	///		call JSONReader::getRawValue() to retrieve raw string value of this
	///		token if necessary.
	JSONTokenType next();
	/// Gets the raw string value of next token
	String getRawValue() const;

	/// Gets the error code of the last operation
	JSONReadError getLastError() const;
	/// Gets the error message of the last operation
	const char* getLastErrorMessage() const;

private:
	bool parseString();
	bool parseNumber();

	int eatWhitespaces();
	JSONTokenType nextToken();

	// Reads more data to the character buffer, discarding already parsed chars.
	int readData(bool appendMode = false);
	// Compares 2 given string, and returns true if the characters are identical.
	static bool stringEqualN(const char* psz, const char* psz2, int length);
	// Determines whether the given character is an whitespace character.
	static bool isWhitespaceChar(char c);
	// Decodes the JSON string
	static String unescapeString(const char* chars, int offset, int length);

	static const char* getErrorMessageTemplate(JSONReadError error);

	void setLastError(JSONReadError error, ...);
	int getLinePos() const {
		return _charPos - _lineStartPos;
	}
	void onNewLine(int pos) {
		_lineNo++;
		_lineStartPos = pos - 1;
	}

private:
	// Parsing state
	bool _retain;
	char* _chars;
	int _charsLength;
	int _charPos;
	int _charsUsed;
	bool _isEof;	// EOF flag
	String _value;

	// Last error
	JSONReadError _lastError;
	String _lastErrorMessage;

	// Current line number & position
	int _lineNo;
	int _lineStartPos;

	REF<TextReader> _textReader;
};

#endif /* __TEST_JSON_JSONREADER_H_ */
